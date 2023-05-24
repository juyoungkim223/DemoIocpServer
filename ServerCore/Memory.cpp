#include "pch.h"
#include "Memory.h"
#include "MemoryPool.h"

/*-------------
	Memory
---------------*/

Memory::Memory()
{
	int32 size = 0;
	int32 tableIndex = 0;
	// 32개
	for (size = 32; size <= 1024; size += 32)
	{
		MemoryPool* pool = new MemoryPool(size);
		_pools.push_back(pool);
		// helper테이블 채워줌
		while (tableIndex <= size)
		{
			_poolTable[tableIndex] = pool;
			tableIndex++;
		}
	}
	// 8개
	for (size = 1024; size <= 2048; size += 128)
	{
		MemoryPool* pool = new MemoryPool(size);
		_pools.push_back(pool);

		while (tableIndex <= size)
		{
			_poolTable[tableIndex] = pool;
			tableIndex++;
		}
	}
	// 8개
	for (size = 2048; size <= 4096; size += 256)
	{
		MemoryPool* pool = new MemoryPool(size);
		_pools.push_back(pool);
		// 메모리 사이즈가 2048 ~ 4096까지 사이즈가 256 씩 증가하며 메모리 풀에 넣어둔다. ex) 2048~ 2048+256 까지는 2048 + 256 크기의 메모리 사용
		while (tableIndex <= size)
		{
			_poolTable[tableIndex] = pool;
			tableIndex++;
		}
	}
}
// 프로그램 종료 시
Memory::~Memory()
{
	for (MemoryPool* pool : _pools)
		delete pool;

	_pools.clear();
}
// 메모리풀에서 꺼내옴 param size: 헤더를 사용하지 않은 정보
void* Memory::Allocate(int32 size)
{
	MemoryHeader* header = nullptr;
	// allocsize = size + 메모리헤더 크기
	const int32 allocSize = size + sizeof(MemoryHeader);

	if (allocSize > MAX_ALLOC_SIZE)
	{
		// 메모리 풀링 최대 크기를 벗어나면 일반 할당
		header = reinterpret_cast<MemoryHeader*>(::malloc(allocSize));
	}
	else
	{
		// 메모리 풀에서 꺼내온다
		header = _poolTable[allocSize]->Pop();
	}

	return MemoryHeader::AttachHeader(header, allocSize);
}

// 메모리 풀에다가 반납
void Memory::Release(void* ptr)
{
	MemoryHeader* header = MemoryHeader::DetachHeader(ptr);

	const int32 allocSize = header->allocSize;
	ASSERT_CRASH(allocSize > 0);

	if (allocSize > MAX_ALLOC_SIZE)
	{
		// 메모리 풀링 최대 크기를 벗어나면 일반 해제
		::free(header);
	}
	else
	{
		// 메모리 풀에 반납한다
		_poolTable[allocSize]->Push(header);
	}
}
