#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
/*
�������� ����� �Ŵ������� Ŭ������
*/
ThreadManager* GThreadManager = nullptr;
class CoreGlobal
{
public:
	CoreGlobal()
	{
		cout << "core global" << endl;
		GThreadManager = new ThreadManager();
	}
	~CoreGlobal()
	{
		delete GThreadManager;
	} 
}CoreGlobal; //CoreGlobal Core; ó�� �Ʒ��� �����ص� ������ �˾Ƽ� �������. �����ڵ� ������ cpp���� ����� ���ÿ� ����.

