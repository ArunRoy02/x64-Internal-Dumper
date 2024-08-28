#pragma once

namespace syscalls
{
	void* allocate_memory ( size_t size, uint32_t protect );
	void free_memory ( void* buffer );
	NTSTATUS QueryMemoryInformation ( void* baseAddress, MEMORY_BASIC_INFORMATION& mbi, size_t mem_info_size );
}