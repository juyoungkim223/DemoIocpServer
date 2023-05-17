#include <iostream>
#include "pch.h"
#include "CorePch.h"
#include <thread>
#include <atomic> // linux, windows �ȯ�濡���� �����ϴ� atomic
#include <mutex>
#include <Windows.h>
#include <future>
#include "CoreMacro.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"

//���� Ŭ������ ����ؼ� ����
class TestLock
{
	USE_LOCK; // ���� �ϳ��� ����ϰڴٴ� �ǹ�. Lock _locks[1] �� �ڵ尡 ����
public:
	int32 TestRead()
	{
		READ_LOCK;
		if (_queue.empty())
			return -1;
		return _queue.front();
	}
	void TestPush()
	{
		WRITE_LOCK;
		_queue.push(rand() % 100);
	}
	void TestPop()
	{
		WRITE_LOCK;
		if (_queue.empty() == false)
			_queue.pop();
	}
private:
	queue<int32> _queue;
};
TestLock testLock;
void ThreadWrite()
{
	while (true)
	{
		testLock.TestPush();
		this_thread::sleep_for(1ms);
		testLock.TestPop();
	}
}
void ThreadRead()
{
	while (true)
	{
		int32 value = testLock.TestRead();
		cout << value << endl;
		this_thread::sleep_for(1ms);
	}
}
int main()
{
	//ASSERT_CRASH(a != 3);
	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch(ThreadWrite);
	}
	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch(ThreadRead);
	}
	GThreadManager->Join();
}