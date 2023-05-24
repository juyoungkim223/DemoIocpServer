#pragma once

/*-----------------
	MemoryHeader
------------------*/

struct MemoryHeader
{
	// [MemoryHeader][Data] MemoryHeader, Data 2가지 영역으로 나눠서 설계한다.
	MemoryHeader(int32 size) : allocSize(size) { }

	static void* AttachHeader(MemoryHeader* header, int32 size)
	{
		new(header)MemoryHeader(size); // placement new
		return reinterpret_cast<void*>(++header); // 포인터연산으로 포인터가 [Data]를 가르킴
	}

	static MemoryHeader* DetachHeader(void* ptr)
	{
		MemoryHeader* header = reinterpret_cast<MemoryHeader*>(ptr) - 1; // 포인터연산으로 포인터가 [MemoryHeader]를 가르킴
		return header;
	}

	int32 allocSize;
	// TODO : 필요한 추가 정보
};
/*-----------------
	MemoryPool
------------------*/

class MemoryPool
{
public:
	MemoryPool(int32 allocSize);
	~MemoryPool();

	// pool에다가 사용한 메모리 반납
	void Push(MemoryHeader* ptr);
	// 실제 사용 시에는 풀에서 pop한다.
	MemoryHeader* Pop();

private:
	int32 _allocSize = 0;
	// 메모리 풀의 객체 개수
	atomic<int32> _allocCount = 0;

	USE_LOCK;
	queue<MemoryHeader*> _queue;
};

