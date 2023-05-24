#pragma once

class Allocator
{

};

/*------------
BaseAllocator
---------------*/
class BaseAllocator
{
public:
	static void* Alloc(int32 size);
	static void Release(void* ptr);
};



/*------------
StompAllocator
---------------*/
class StompAllocator
{
	enum
	{
		PAGE_SIZE = 0x1000 // 10진수 4096
	};
public:
	static void* Alloc(int32 size);
	static void Release(void* ptr);
};

/*-------------------
	PoolAllocator
-------------------*/

class PoolAllocator
{
public:
	static void* Alloc(int32 size);
	static void		Release(void* ptr);
};

/*------------
STL Allocator
---------------*/
template<typename T>
class StlAllocator
{
public:
	using value_type = T;
	StlAllocator() {}
	template<typename Other>
	StlAllocator(const StlAllocator<Other>&) {}
	T* allocate(size_t count)
	{
		// 총 메모리 사이즈
		const int32 size = static_cast<int32>(count * sizeof(T));
		return static_cast<T*>(X_ALLOC(size));
	}
	void deallocate(T* ptr, size_t count)
	{
		X_RELEASE(ptr);
	}
};