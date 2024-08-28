#include "includes.h"

#define NtCurrentProcess() ( (HANDLE)(LONG_PTR) -1 )
#define ZwCurrentProcess() NtCurrentProcess()   

extern "C" NTSTATUS ZwAllocateVirtualMemory ( HANDLE, PVOID*, ULONG_PTR, PSIZE_T, ULONG, ULONG );
extern "C" NTSTATUS ZwFreeVirtualMemory ( HANDLE, PVOID*, PSIZE_T, ULONG );
extern "C" NTSTATUS ZwQueryVirtualMemory ( HANDLE, PVOID, uint32_t, PVOID, SIZE_T, PSIZE_T );

void* syscalls::allocate_memory ( size_t size, uint32_t protect )
{
	void* alloc_buffer = nullptr;
	size_t alloc_size = size;

	ZwAllocateVirtualMemory ( ZwCurrentProcess ( ), &alloc_buffer, 0, &alloc_size, MEM_RESERVE | MEM_COMMIT, protect );

	return alloc_buffer;
}

void syscalls::free_memory ( void* buffer )
{
	void* free_buffer = buffer;
	size_t free_size = 0;

	ZwFreeVirtualMemory ( ZwCurrentProcess ( ), &free_buffer, &free_size, MEM_RELEASE );
}

NTSTATUS syscalls::QueryMemoryInformation ( void* baseAddress, MEMORY_BASIC_INFORMATION& mbi, size_t mem_info_size )
{
    return ZwQueryVirtualMemory ( ZwCurrentProcess ( ), baseAddress, 0, &mbi, mem_info_size, nullptr );
}