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
		// ~1024바이트까지 32단위, ~2048까지 128단위, ~4096까지 256단위
		POOL_COUNT = (1024 / 32) + (1024 / 128) + (2048 / 256),
		// 4KB까지만하고 이 이상은 생으로 기본힙할당기 이용하자.
		MAX_ALLOC_SIZE = 4096 
	};

public:
	Memory();
	~Memory();

	void* Allocate(int32 size);
	void	Release(void* ptr);

private:
	vector<MemoryPool*> _pools;

	// 메모리 크기 <-> 메모리 풀
	// O(1) 빠르게 찾기 위한 테이블
	MemoryPool* _poolTable[MAX_ALLOC_SIZE + 1];
};



// 객체 생성은 메모리 할당과 객체의 생성자 호출 두단계로 나뉘어 있다. 인자 개수는 가변적으로 허용하기 위해 variadic template
template<typename Type, typename... Args>
Type* xnew(Args&&... args)
{
	// 1. memory 할당
	//Type* memory = static_cast<Type*>(BaseAllocator::Alloc(sizeof(Type)));
	Type* memory = static_cast<Type*>(X_ALLOC(sizeof(Type)));
	// 2. 객체의 생성자 호출. Placement new
	// 복사, 이동 상관없이 전달하기 위해 forward variadic 유지하기 위한 ...
	new(memory)Type(std::forward<Args>(args)...); 
	return memory;
}

template<typename Type>
void xdelete(Type* obj)
{
	// 객체 대상으로 소멸자 호출
	obj->~Type();
	// 메모리 반납
	X_RELEASE(obj);
	//BaseAllocator::Release(obj);
}