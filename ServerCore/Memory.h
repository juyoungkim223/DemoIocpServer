#pragma once

#include "Allocator.h"

class MemoryPool;

/*-------------
	Memory
---------------*/

class Memory
{
	enum
	{
		// ~1024����Ʈ���� 32����, ~2048���� 128����, ~4096���� 256����
		POOL_COUNT = (1024 / 32) + (1024 / 128) + (2048 / 256),
		// 4KB�������ϰ� �� �̻��� ������ �⺻���Ҵ�� �̿�����.
		MAX_ALLOC_SIZE = 4096 
	};

public:
	Memory();
	~Memory();

	void* Allocate(int32 size);
	void	Release(void* ptr);

private:
	vector<MemoryPool*> _pools;

	// �޸� ũ�� <-> �޸� Ǯ
	// O(1) ������ ã�� ���� ���̺�
	MemoryPool* _poolTable[MAX_ALLOC_SIZE + 1];
};



// ��ü ������ �޸� �Ҵ�� ��ü�� ������ ȣ�� �δܰ�� ������ �ִ�. ���� ������ ���������� ����ϱ� ���� variadic template
template<typename Type, typename... Args>
Type* xnew(Args&&... args)
{
	// 1. memory �Ҵ�
	//Type* memory = static_cast<Type*>(BaseAllocator::Alloc(sizeof(Type)));
	Type* memory = static_cast<Type*>(X_ALLOC(sizeof(Type)));
	// 2. ��ü�� ������ ȣ��. Placement new
	// ����, �̵� ������� �����ϱ� ���� forward variadic �����ϱ� ���� ...
	new(memory)Type(std::forward<Args>(args)...); 
	return memory;
}

template<typename Type>
void xdelete(Type* obj)
{
	// ��ü ������� �Ҹ��� ȣ��
	obj->~Type();
	// �޸� �ݳ�
	X_RELEASE(obj);
	//BaseAllocator::Release(obj);
}