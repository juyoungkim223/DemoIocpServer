#include "pch.h"
#include "Lock.h"
#include "DeadLockProfiler.h"
void Lock::WriteLock(const char* name)
{
#if _DEBUG
	GDeadLockProfiler->PushLock(name);
#endif
	// 동일한 스레드가 소유하고 있다면 무조건 성공시키자 ( 재귀적 호출인 경우)
	const uint32 lockThreadId = (_lockFlag.load() & WRITE_THREAD_MASK) >> 16;
	if (LThreadId == lockThreadId) // TLS id와 플래그 비트이동 같다면
	{
		_writeCount++; // 나만 사용
		return;
	}
	// 아무도 소유(상호배타적인 소유권: write) 및 공유(read 하는 상황)하고 있지 않을 때(W,R 모두 0인상황), 경합해서 소유권을 얻는다.
	const int64 beginTick = ::GetTickCount64();
	const uint32 desired = ((LThreadId << 16) & WRITE_THREAD_MASK);

	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
		{
			uint32 expected = EMPTY_FLAG;
			if (_lockFlag.compare_exchange_strong(OUT expected, desired))
			{
				_writeCount++;
				return;
			}
		}
		// 5천번 테스트했으면 소유권 내려놓고 컨텍스트 스위칭 일어나고 다시 while에 의해 실행된다. 하지만 너무오래걸리면 크래시낸다.
		if (::GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
			CRASH("LOCK_TIMEOUT");
		this_thread::yield();
	}
}

void Lock::WriteUnlock(const char* name)
{
#if _DEBUG
	GDeadLockProfiler->PopLock(name);
#endif
	// ReadLock 다 풀기 전에는 WriteUnlock 불가능
	// 0이 아니면 어떤 스레드가 사용하고 있는 것. 쓰레드 아이디는 메인스레드인 1부터 시작이니까
	if((_lockFlag.load() & READ_COUNT_MASK) != 0)
		CRASH("INVALID_UNLOCK_ORDER");

	const int32 lockCount = --_writeCount;
	if (lockCount == 0)
		_lockFlag.store(EMPTY_FLAG);
}

void Lock::ReadLock(const char* name)
{
#if _DEBUG
	GDeadLockProfiler->PushLock(name);
#endif
	// 동일한 스레드가 소유(write lock)하고 있다면 무조건 성공
	const uint32 lockThreadId = (_lockFlag.load() & WRITE_THREAD_MASK) >> 16;
	if (LThreadId == lockThreadId) // TLS id와 플래그 비트이동 같다면
	{
		_lockFlag.fetch_add(1); // 나만 사용
		return;
	}
	// 아무도 소유하고 있지 않을 때 경합해서 공유 카운트를 올린다
	const int64 beginTick = ::GetTickCount64();

	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
		{
			// Write 플래그와 관련된 앞의16비트를 0으로 밀고 비교한다는 의미
			uint32 expected = (_lockFlag.load() & READ_COUNT_MASK); 
			if (_lockFlag.compare_exchange_strong(OUT expected, expected + 1))
				return;
		}
		// 컨텍스트 스위칭 일어나고 다시 while에 의해 실행된다. 하지만 너무오래걸리면 크래시낸다.
		if (::GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
			CRASH("LOCK_TIMEOUT");
		this_thread::yield();
	}
}

void Lock::ReadUnlock(const char* name)
{
#if _DEBUG
	GDeadLockProfiler->PopLock(name);
#endif
	// ReadLock을 하기전에 ReadUnlock을 하는 묘한 상황임. 발생할 가능성이 없는 상황이지만 방어코드
	if((_lockFlag.fetch_sub(1) & READ_COUNT_MASK) == 0)
		CRASH("MULTIPLE_UNLOCK");

}

