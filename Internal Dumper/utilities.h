#pragma once

namespace utilities
{
	void MapExe ( uintptr_t Module, uint32_t Size, uintptr_t& oModule );
	uint32_t GetModuleSize ( void** BaseAdress );

	void DumpModule ( uintptr_t moduleBase, const char* directoryPath, const char* folderName );

	void Dumper ( bool dumpAllModules );
}