#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "DeadLockProfiler.h"
#include "Memory.h"
/*
�������� ����� �Ŵ������� Ŭ������
*/
ThreadManager* GThreadManager = nullptr;
Memory* GMemory = nullptr;
DeadLockProfiler* GDeadLockProfiler = nullptr;
class CoreGlobal
{
public:
	CoreGlobal()
	{
		cout << "core global" << endl;
		GThreadManager = new ThreadManager();
		GMemory = new Memory();
		GDeadLockProfiler = new DeadLockProfiler();
	}
	~CoreGlobal()
	{
		delete GThreadManager;
		delete GMemory;
		delete GDeadLockProfiler;

	}
}CoreGlobal; //CoreGlobal Core; ó�� �Ʒ��� �����ص� ������ �˾Ƽ� �������. �����ڵ� ������ cpp���� ����� ���ÿ� ����.

