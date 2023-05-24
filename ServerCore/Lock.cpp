#include "pch.h"
#include "Lock.h"
#include "DeadLockProfiler.h"
void Lock::WriteLock(const char* name)
{
#if _DEBUG
	GDeadLockProfiler->PushLock(name);
#endif
	// ������ �����尡 �����ϰ� �ִٸ� ������ ������Ű�� ( ����� ȣ���� ���)
	const uint32 lockThreadId = (_lockFlag.load() & WRITE_THREAD_MASK) >> 16;
	if (LThreadId == lockThreadId) // TLS id�� �÷��� ��Ʈ�̵� ���ٸ�
	{
		_writeCount++; // ���� ���
		return;
	}
	// �ƹ��� ����(��ȣ��Ÿ���� ������: write) �� ����(read �ϴ� ��Ȳ)�ϰ� ���� ���� ��(W,R ��� 0�λ�Ȳ), �����ؼ� �������� ��´�.
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
		// 5õ�� �׽�Ʈ������ ������ �������� ���ؽ�Ʈ ����Ī �Ͼ�� �ٽ� while�� ���� ����ȴ�. ������ �ʹ������ɸ��� ũ���ó���.
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
	// ReadLock �� Ǯ�� ������ WriteUnlock �Ұ���
	// 0�� �ƴϸ� � �����尡 ����ϰ� �ִ� ��. ������ ���̵�� ���ν������� 1���� �����̴ϱ�
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
	// ������ �����尡 ����(write lock)�ϰ� �ִٸ� ������ ����
	const uint32 lockThreadId = (_lockFlag.load() & WRITE_THREAD_MASK) >> 16;
	if (LThreadId == lockThreadId) // TLS id�� �÷��� ��Ʈ�̵� ���ٸ�
	{
		_lockFlag.fetch_add(1); // ���� ���
		return;
	}
	// �ƹ��� �����ϰ� ���� ���� �� �����ؼ� ���� ī��Ʈ�� �ø���
	const int64 beginTick = ::GetTickCount64();

	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
		{
			// Write �÷��׿� ���õ� ����16��Ʈ�� 0���� �а� ���Ѵٴ� �ǹ�
			uint32 expected = (_lockFlag.load() & READ_COUNT_MASK); 
			if (_lockFlag.compare_exchange_strong(OUT expected, expected + 1))
				return;
		}
		// ���ؽ�Ʈ ����Ī �Ͼ�� �ٽ� while�� ���� ����ȴ�. ������ �ʹ������ɸ��� ũ���ó���.
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
	// ReadLock�� �ϱ����� ReadUnlock�� �ϴ� ���� ��Ȳ��. �߻��� ���ɼ��� ���� ��Ȳ������ ����ڵ�
	if((_lockFlag.fetch_sub(1) & READ_COUNT_MASK) == 0)
		CRASH("MULTIPLE_UNLOCK");

}

