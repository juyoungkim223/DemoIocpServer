#include "pch.h"
#include "AccountManager.h"
#include "UserManager.h"
void AccountManager::ProcessLogin()
{
	//accountLock, ��Ƽ������ ȯ�濡�� ���ư����ϴϱ� ������
	lock_guard<mutex> guard(_mutex);

	// userLock: getUser���ο� ������ ����
	// ������� �ΰ��� ���� ������.
	User* user = UserManager::Instance()->GetUser(100);

	// TODO
}