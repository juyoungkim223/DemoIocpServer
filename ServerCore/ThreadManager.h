#pragma once
/*
표준 스레드를 관리하는 매니저
*/
#include <thread>
#include <functional> // 콜백함수에서 사용됨
/*------------------
	ThreadManager
-------------------*/
class ThreadManager
{
public:
	ThreadManager();
	~ThreadManager();
	void Launch(function<void(void)> callback);
	void Join(); // thread가 끝날때까지 기다려주는 join

	//TLS의 초기화 역할
	static void InitTLS();
	static void DestoryTLS();

private:
	Mutex _lock;
	vector<thread> _threads;
};

