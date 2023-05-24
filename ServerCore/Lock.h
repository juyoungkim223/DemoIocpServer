#pragma once
#include "Types.h"

/*--------------------
Reader Writer SpinLock
----------------------*/

/*--------------------
[WWWWWWWW][WWWWWWWW][RRRRRRRR][RRRRRRRR], 내가소유하고 있다면 WWW~ 부분에 내 LThreadId가 들어간다.
W: WriteFlag(Exclusive Lock Owner ThreadId) 16비트
R: ReadFlog (Shared Lock Count) 16비트

질문1.
LThreadId가 [32][32][0][1]인 스레드가 Write중에
LThreadId가 [64][64][0][1]인 스레드가 WriteLock을 호출하면
뒤의 16비트를 깡그리 날리니깐 [0][1] == [0][1] ? 가 되는데 그럼 같은 스레드로 인식하여 Lock을 잡게되는 것 아닌지
답변1.
LThreadId가 [32][32][0][1]인 스레드가 Write중에
LThreadId가 [64][64][0][1]인 스레드가 존재할 수 없습니다.
다른 애가 Write중이라면 [64][64][0][1]에서 뒷 부분인 [0][1]
즉 ReadCount가 1이 될 수 없습니다.
----------------------*/
// W -> W (O)
// W -> R (O) : Write 상황에서 동일 스레드가 Read는 가능
// R -> W (X) : Read 상황에서 동일 스레드가 Write는 불가능
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
	Atomic<uint32> _lockFlag; // WRITE, READ 카운트 용도
	uint16 _writeCount = 0; // WRITE 카운트 용도
};

/*---------------
LockGuards
-----------------*/

class ReadLockGuard
{
public:
	//RAAI 패턴, 생성자에서 락 잡고 소멸자에서 락 해제
	ReadLockGuard(Lock& lock, const char* name) : _lock(lock), _name(name) { _lock.ReadLock(name); }
	~ReadLockGuard() { _lock.ReadUnlock(_name); }
private:
	Lock& _lock;
	const char* _name;
};
class WriteLockGuard
{
public:
	//RAAI 패턴, 생성자에서 락 잡고 소멸자에서 락 해제
	WriteLockGuard(Lock& lock, const char* name) : _lock(lock), _name(name) { _lock.WriteLock(name); }
	~WriteLockGuard() { _lock.WriteUnlock(_name); }
private:
	Lock& _lock;
	const char* _name;
};