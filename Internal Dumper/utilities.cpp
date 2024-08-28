#include "includes.h"

void utilities::MapExe ( uintptr_t module, uint32_t size, uintptr_t& oModule )
{
	void* allocatedMem = syscalls::allocate_memory ( size, PAGE_READWRITE );

	MEMORY_BASIC_INFORMATION mbi = {};

	void* curAddress = reinterpret_cast< void* >(module);
	void* endAddress = reinterpret_cast< void* >(module + size);

	while ( curAddress < endAddress )
	{
		auto status = syscalls::QueryMemoryInformation ( curAddress, mbi, sizeof ( mbi ) );
		if ( status == 0 )
		{
			//if (((mbi.Protect & PAGE_GUARD) == 0) && (mbi.Protect != PAGE_NOACCESS)) // modify flags depending on target process protection	

			if ( (mbi.State == MEM_COMMIT) && ((mbi.Protect & PAGE_GUARD) == 0) && (mbi.Protect != PAGE_NOACCESS) && ((mbi.AllocationProtect & PAGE_NOCACHE) != PAGE_NOCACHE) )
			{
				uintptr_t offset = reinterpret_cast< uintptr_t >(curAddress) - module;
				uintptr_t addrOffset = reinterpret_cast< uintptr_t >(allocatedMem) + offset;

				memcpy ( reinterpret_cast< void* >(addrOffset), curAddress, mbi.RegionSize );
			}
		}

		curAddress = reinterpret_cast< PBYTE >(mbi.BaseAddress) + mbi.RegionSize;
	}

	// Get DOS Header of our Allocated Dump
	auto dosHeaderNew = reinterpret_cast< PIMAGE_DOS_HEADER >(allocatedMem);

	// Get NT Header of our Allocated Dump
	auto ntHeaderNew = reinterpret_cast< PIMAGE_NT_HEADERS >(dosHeaderNew->e_lfanew + reinterpret_cast< uintptr_t >(allocatedMem));

	// Get Section Header of our Allocated Dump
	PIMAGE_SECTION_HEADER sectionHeader = IMAGE_FIRST_SECTION ( ntHeaderNew );

	// Iterate through each section and fix size and address
	for ( int i = 0; i < ntHeaderNew->FileHeader.NumberOfSections; i++, sectionHeader++ )
	{
		sectionHeader->SizeOfRawData = sectionHeader->Misc.VirtualSize;
		sectionHeader->PointerToRawData = sectionHeader->VirtualAddress;
	}

	oModule = reinterpret_cast< uintptr_t >(allocatedMem);
}

uint32_t utilities::GetModuleSize ( void** baseAddress )
{
	auto dosHeader = reinterpret_cast< IMAGE_DOS_HEADER* >(baseAddress);
	auto ntHeader = reinterpret_cast< IMAGE_NT_HEADERS* >(dosHeader->e_lfanew + reinterpret_cast< uint8_t* >(dosHeader));

	return ntHeader->OptionalHeader.SizeOfImage;
}

void utilities::DumpModule ( uintptr_t moduleBase, const char* directoryPath, const char* folderName )
{
	uint32_t moduleSize = GetModuleSize ( reinterpret_cast< void** >(moduleBase) );

	uintptr_t mapBase = 0;
	MapExe ( moduleBase, moduleSize, mapBase );

	// Get module name
	char moduleName[MAX_PATH];
	if ( !GetModuleFileNameA ( reinterpret_cast< HMODULE >(moduleBase), moduleName, MAX_PATH ) )
	{
		return;
	}

	// Extract filename (without path)
	char* filename = strrchr ( moduleName, '\\' );
	if ( filename )
	{
		filename++;  // Skip the backslash
	}
	else
	{
		filename = moduleName;
	}

	// Build final path with folder name appended to the module name
	char finalPath[MAX_PATH];
	snprintf ( finalPath, sizeof ( finalPath ), _xorstr ( "%s\\%s_%s" ), directoryPath, folderName, filename );

	// Open file for writing
	HANDLE hFile = CreateFileA ( finalPath, GENERIC_WRITE, 0, nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr );
	if ( hFile != INVALID_HANDLE_VALUE )
	{
		DWORD bytesWritten = 0;
		WriteFile ( hFile, reinterpret_cast< void* >(mapBase), moduleSize, &bytesWritten, nullptr );

		CloseHandle ( hFile );
	}

	syscalls::free_memory ( &mapBase );
}

void utilities::Dumper ( bool dumpAllModules )
{
	char currentProcessPath[MAX_PATH];
	if ( !GetModuleFileNameA ( NULL, currentProcessPath, MAX_PATH ) )
		return;

	// Extract the executable name from the path
	char* processName = strrchr ( currentProcessPath, '\\' );
	if ( processName )
	{
		processName++; // Skip the backslash
	}
	else
	{
		processName = currentProcessPath; // Use the entire path if no backslash is found
	}

	// Strip extension from the process name, if present
	char* extension = strrchr ( processName, '.' );
	if ( extension )
	{
		*extension = '\0'; // Terminate string at the extension start
	}

	// Get current date and time
	SYSTEMTIME time;
	GetLocalTime ( &time );

	char timestamp[32];
	sprintf_s ( timestamp, sizeof ( timestamp ), _xorstr ( "_%04d-%02d-%02d_%02d-%02d-%02d" ), time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond );

	// Create a directory path on the desktop
	char desktopPath[MAX_PATH];
	SHGetFolderPathA ( NULL, CSIDL_DESKTOP, NULL, 0, desktopPath );

	strcat_s ( desktopPath, _xorstr ( "\\" ) );
	strcat_s ( desktopPath, processName );
	strcat_s ( desktopPath, timestamp );

	// Create the directory if it doesn't exist
	if ( !CreateDirectoryA ( desktopPath, NULL ) && GetLastError ( ) != ERROR_ALREADY_EXISTS )
		return;

	char folderName[128];
	sprintf_s ( folderName, _xorstr ( "%s%s" ), processName, timestamp ); // Format folder name for use in file naming

	if ( dumpAllModules )
	{
		HANDLE hProcess = GetCurrentProcess ( );
		HMODULE hMods[1024];
		DWORD cbNeeded;

		if ( EnumProcessModules ( hProcess, hMods, sizeof ( hMods ), &cbNeeded ) )
		{
			for ( int i = 0; i < (cbNeeded / sizeof ( HMODULE )); i++ )
			{
				uintptr_t moduleBase = reinterpret_cast< uintptr_t >(hMods[i]);
				utilities::DumpModule ( moduleBase, desktopPath, folderName );
			}
		}
	}
	else
	{
		// Dump only the main module of the process
		// nullptr for current otherwise fill in module name to target specific module
		uintptr_t moduleBase = reinterpret_cast< uintptr_t >( GetModuleHandleA ( nullptr ) );
		utilities::DumpModule ( moduleBase, desktopPath, folderName );
	}
}