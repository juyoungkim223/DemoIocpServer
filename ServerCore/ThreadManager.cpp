#include "pch.h"
#include "ThreadManager.h"
#include "CoreTLS.h"
#include "CoreGlobal.h"

/*-----------
ThreadManager
-------------*/

ThreadManager::ThreadManager()
{
	// MainThread가 사용하는 TLS
	InitTLS();

}


ThreadManager::~ThreadManager()
{
	Join();
}

//function은 람다뿐만 아니라 온갖 함수 형태를 다 받을 수 있다. 함수포인터보다 std::function 사용하는 것이 좋다. function<반환(인풋)>
void ThreadManager::Launch(function<void(void)> callback)
{
	LockGuard guard(_lock);
	//스레드 람다로 실행
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
	LThreadId = SThreadId.fetch_add(1); // 1부터 시작해서 차례로 증가
}

// InitTLS 에서 동적으로 생성되는 애들 날려주는 역할
void ThreadManager::DestoryTLS()
{

}
