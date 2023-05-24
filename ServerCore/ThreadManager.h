#pragma once
/*
ǥ�� �����带 �����ϴ� �Ŵ���
*/
#include <thread>
#include <functional> // �ݹ��Լ����� ����
/*------------------
	ThreadManager
-------------------*/
class ThreadManager
{
public:
	ThreadManager();
	~ThreadManager();
	void Launch(function<void(void)> callback);
	void Join(); // thread�� ���������� ��ٷ��ִ� join

	//TLS�� �ʱ�ȭ ����
	static void InitTLS();
	static void DestoryTLS();

private:
	Mutex _lock;
	vector<thread> _threads;
};

