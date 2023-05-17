#include "pch.h"
#include "ThreadManager.h"
#include "CoreTLS.h"
#include "CoreGlobal.h"

/*-----------
ThreadManager
-------------*/

ThreadManager::ThreadManager()
{
	// MainThread�� ����ϴ� TLS
	InitTLS();

}


ThreadManager::~ThreadManager()
{
	Join();
}

//function�� ���ٻӸ� �ƴ϶� �°� �Լ� ���¸� �� ���� �� �ִ�. �Լ������ͺ��� std::function ����ϴ� ���� ����. function<��ȯ(��ǲ)>
void ThreadManager::Launch(function<void(void)> callback)
{
	LockGuard guard(_lock);
	//������ ���ٷ� ����
	_threads.push_back(thread([=]()
		{
			InitTLS();
			callback();
			DestoryTLS();
		}));
}

void ThreadManager::Join()
{
	for (thread& t : _threads)
	{
		if (t.joinable())
			t.join();
	}
	_threads.clear();
}

void ThreadManager::InitTLS()
{
	static Atomic<uint32> SThreadId = 1;
	cout << "Thread ID : " << SThreadId << "!!!!" << endl;
	LThreadId = SThreadId.fetch_add(1); // 1���� �����ؼ� ���ʷ� ����
}

// InitTLS ���� �������� �����Ǵ� �ֵ� �����ִ� ����
void ThreadManager::DestoryTLS()
{

}
