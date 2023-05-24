#pragma once
//����� define�� ����� �־���.
// ��ó�� �������� ���̴� ��ũ��, �����
// define�� �����ٿ� �ϰ� ������ \�� ���������
// nullptr�ƴ϶�� �����϶� ��� ���, nullptr�� �����Ϸ��� ���� �� �����ϱ�
// ũ���ø� ���� ���� �� ���

/*
�� �״�� �ƹ��͵� �� �ϴ� Ű�����Դϴ� (?)
�������� �ѱ�� �̰� �ܼ� ���縦 �ϱ� �Ⱦ �ѱ�� ������,
�������� �ٲ�� �������� �˱� ���� ���� �־�
�������� ���� �������� �ٲٴ� ��� ��Ʈ�� �ֱ� ���� OUT �� ���Դϴ�.
*/
#define OUT 


/*----------------
* LOCK
-----------------*/

#define USE_MANY_LOCKS(count) Lock _locks[count]; // ������ ����Ѵٸ�
#define USE_LOCK USE_MANY_LOCKS(1) // �ϳ��� ����Ѵٸ�
#define READ_LOCK_IDX(idx) ReadLockGuard readLockGurad_##idx(_locks[idx], typeid(this).name()); // ���° �ε����� ���� ����� ����
#define READ_LOCK READ_LOCK_IDX(0)
#define WRITE_LOCK_IDX(idx) WriteLockGuard writeLockGurad_##idx(_locks[idx], typeid(this).name());
#define WRITE_LOCK WRITE_LOCK_IDX(0)

/*----------------
* MEMORY
-----------------*/
#ifdef _DEBUG
#define X_ALLOC(size) PoolAllocator::Alloc(size)
#define X_RELEASE(ptr) PoolAllocator::Release(ptr)
#else
#define X_ALLOC(size) StompAllocator::Alloc(size)
#define X_RELEASE(ptr) StompAllocator::Release(ptr)
#endif

/*----------------
* CRASH
-----------------*/

#define CRASH(cause)							\
{												\
	uint32* crash = nullptr;					\
	__analysis_assume(crash != nullptr);		\
	*crash = 0x111;								\
}

#define ASSERT_CRASH(expr)						\
{												\
	if(!(expr))									\
	{											\
		CRASH("ASSERT CRASH")					\
		__analysis_assume(expr);				\
	}											\
}