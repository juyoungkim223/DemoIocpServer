#pragma once
/*
전역으로 사용할 매니저류의 클래스들
*/

extern class ThreadManager* GThreadManager; // 전방선언은 포인터형으로 전역변수
extern class Memory* GMemory;
extern class DeadLockProfiler* GDeadLockProfiler;