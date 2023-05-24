#include "pch.h"
#include "MemoryPool.h"
#include "MemoryPool.h"

/*-----------------
	MemoryPool
------------------*/

MemoryPool::MemoryPool(int32 allocSize) : _allocSize(allocSize)
{
}

MemoryPool::~MemoryPool()
{
	while (_queue.empty() == false)
	{
		MemoryHeader* header = _queue.front();
		_queue.pop();
		::free(header);
	}
}

void MemoryPool::Push(MemoryHeader* ptr)
{
	WRITE_LOCK;
	// allocSize가 0이면 사용하지 않는 상태로 간주한다.
	ptr->allocSize = 0;

	// Pool에 메모리 반납
	_queue.push(ptr);

	_allocCount.fetch_sub(1);
}

MemoryHeader* MemoryPool::Pop()
{
	MemoryHeader* header = nullptr;

	{
		// 풀에 메모리 있는지 없는지 체크해아하니까 쓰기 락 걸어놓음.
		WRITE_LOCK;
		// Pool에 여분이 있는지?
		if (_queue.empty() == false)
		{
			// 있으면 하나 꺼내온다
			header = _queue.front();
			_queue.pop();
		}
	}

	// 없으면 새로 만들다
	if (header == nullptr)
	{
		header = reinterpret_cast<MemoryHeader*>(::malloc(_allocSize));
	}
	else
	{
		ASSERT_CRASH(header->allocSize == 0);
	}

	_allocCount.fetch_add(1);

	return header;
}
