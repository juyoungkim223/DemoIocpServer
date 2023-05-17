#include "pch.h"
#include "AccountManager.h"
#include "UserManager.h"
void AccountManager::ProcessLogin()
{
	//accountLock, 멀티스레드 환경에서 돌아가야하니까 락가드
	lock_guard<mutex> guard(_mutex);

	// userLock: getUser내부에 락가드 적용
	// 여기까지 두개의 락을 가진다.
	User* user = UserManager::Instance()->GetUser(100);

	// TODO
}