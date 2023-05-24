#include "pch.h"
#include "Allocator.h"
#include "Memory.h"
/*-------------------
	BaseAllocator
-------------------*/

void* BaseAllocator::Alloc(int32 size)
{
	return malloc(size);
}

void BaseAllocator::Release(void* ptr)
{
	::free(ptr);
}
/*-------------------
	StompAllocator
-------------------*/
void* StompAllocator::Alloc(int32 size)
{
	//4바이트 할당 요청: 4-> PAGE_SIZE인 최소 4096이 할당된다.
	//단점: 8바이트만 할당을 원해도 4096의 배수로 할당되는 단점이 있따.
	const int64 pageCount = (size + PAGE_SIZE - 1) / PAGE_SIZE;
	const int64 dataOffset = pageCount * PAGE_SIZE - size;
	void* baseAddress = ::VirtualAlloc(NULL, pageCount * PAGE_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	//바이트단위 계산을 위해 int8로 변환
	return static_cast<void*>(static_cast<int8*>(baseAddress) + dataOffset);
}

void StompAllocator::Release(void* ptr)
{
	const int64 address = reinterpret_cast<int64>(ptr);
	const int64 baseAddress = address - (address % PAGE_SIZE);
	::VirtualFree(reinterpret_cast<void*>(baseAddress), 0, MEM_RELEASE);
}


/*-------------------
	PoolAllocator
-------------------*/

void* PoolAllocator::Alloc(int32 size)
{
	return GMemory->Allocate(size);
}

void PoolAllocator::Release(void* ptr)
{
	GMemory->Release(ptr);
}