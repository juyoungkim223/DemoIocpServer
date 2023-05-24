#pragma once
class PlayerManager
{	
	USE_LOCK;

public:
	void PlayerThenAccount();
	void Lock();
};

//전역으로 생성
extern PlayerManager GPlayerManager;
