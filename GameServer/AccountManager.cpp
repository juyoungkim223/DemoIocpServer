#include "pch.h"
#include "AccountManager.h"
#include "PlayerManager.h"

AccountManager GAccountManager;

void AccountManager::AccountThenPlayer()
{
	WRITE_LOCK;
	//write 락 잡고 player lock 사이에 시간 텀을 두어서 playerManager에서 어카운트락을 잡게한다.
	//this_thread::sleep_for(1s);
	GPlayerManager.Lock();
}

void AccountManager::Lock()
{
	WRITE_LOCK;
}