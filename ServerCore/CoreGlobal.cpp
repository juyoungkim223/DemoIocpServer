#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "DeadLockProfiler.h"
#include "Memory.h"
/*
전역으로 사용할 매니저류의 클래스들
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
}CoreGlobal; //CoreGlobal Core; 처럼 아래에 선언해도 생성자 알아서 만들어짐. 엔진코드 내부의 cpp에서 선언과 동시에 생성.

