#pragma once
#include "Types.h"

/*--------------------
Reader Writer SpinLock
----------------------*/

/*--------------------
[WWWWWWWW][WWWWWWWW][RRRRRRRR][RRRRRRRR], ���������ϰ� �ִٸ� WWW~ �κп� �� LThreadId�� ����.
W: WriteFlag(Exclusive Lock Owner ThreadId) 16��Ʈ
R: ReadFlog (Shared Lock Count) 16��Ʈ

����1.
LThreadId�� [32][32][0][1]�� �����尡 Write�߿�
LThreadId�� [64][64][0][1]�� �����尡 WriteLock�� ȣ���ϸ�
���� 16��Ʈ�� ���׸� �����ϱ� [0][1] == [0][1] ? �� �Ǵµ� �׷� ���� ������� �ν��Ͽ� Lock�� ��ԵǴ� �� �ƴ���
�亯1.
LThreadId�� [32][32][0][1]�� �����尡 Write�߿�
LThreadId�� [64][64][0][1]�� �����尡 ������ �� �����ϴ�.
�ٸ� �ְ� Write���̶�� [64][64][0][1]���� �� �κ��� [0][1]
�� ReadCount�� 1�� �� �� �����ϴ�.
----------------------*/
// W -> W (O)
// W -> R (O) : Write ��Ȳ���� ���� �����尡 Read�� ����
// R -> W (X) : Read ��Ȳ���� ���� �����尡 Write�� �Ұ���
class Lock
{
	enum : uint32
	{
		ACQUIRE_TIMEOUT_TICK = 10000,
		MAX_SPIN_COUNT = 5000,
		WRITE_THREAD_MASK = 0XFFFF'0000,
		READ_COUNT_MASK = 0X0000'FFFF,
		EMPTY_FLAG = 0X0000'0000
	};

public:
	void WriteLock(const char* name);
	void WriteUnlock(const char* name);
	void ReadLock(const char* name);
	void ReadUnlock(const char* name);
private:
	Atomic<uint32> _lockFlag; // WRITE, READ ī��Ʈ �뵵
	uint16 _writeCount = 0; // WRITE ī��Ʈ �뵵
};

/*---------------
LockGuards
-----------------*/

class ReadLockGuard
{
public:
	//RAAI ����, �����ڿ��� �� ��� �Ҹ��ڿ��� �� ����
	ReadLockGuard(Lock& lock, const char* name) : _lock(lock), _name(name) { _lock.ReadLock(name); }
	~ReadLockGuard() { _lock.ReadUnlock(_name); }
private:
	Lock& _lock;
	const char* _name;
};
class WriteLockGuard
{
public:
	//RAAI ����, �����ڿ��� �� ��� �Ҹ��ڿ��� �� ����
	WriteLockGuard(Lock& lock, const char* name) : _lock(lock), _name(name) { _lock.WriteLock(name); }
	~WriteLockGuard() { _lock.WriteUnlock(_name); }
private:
	Lock& _lock;
	const char* _name;
};