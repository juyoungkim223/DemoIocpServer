#pragma once

/*-----------------
	MemoryHeader
------------------*/

struct MemoryHeader
{
	// [MemoryHeader][Data] MemoryHeader, Data 2���� �������� ������ �����Ѵ�.
	MemoryHeader(int32 size) : allocSize(size) { }

	static void* AttachHeader(MemoryHeader* header, int32 size)
	{
		new(header)MemoryHeader(size); // placement new
		return reinterpret_cast<void*>(++header); // �����Ϳ������� �����Ͱ� [Data]�� ����Ŵ
	}

	static MemoryHeader* DetachHeader(void* ptr)
	{
		MemoryHeader* header = reinterpret_cast<MemoryHeader*>(ptr) - 1; // �����Ϳ������� �����Ͱ� [MemoryHeader]�� ����Ŵ
		return header;
	}

	int32 allocSize;
	// TODO : �ʿ��� �߰� ����
};
/*-----------------
	MemoryPool
------------------*/

class MemoryPool
{
public:
	MemoryPool(int32 allocSize);
	~MemoryPool();

	// pool���ٰ� ����� �޸� �ݳ�
	void Push(MemoryHeader* ptr);
	// ���� ��� �ÿ��� Ǯ���� pop�Ѵ�.
	MemoryHeader* Pop();

private:
	int32 _allocSize = 0;
	// �޸� Ǯ�� ��ü ����
	atomic<int32> _allocCount = 0;

	USE_LOCK;
	queue<MemoryHeader*> _queue;
};

