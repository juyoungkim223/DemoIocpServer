#pragma once
#include <mutex>
class Account
{

};

class AccountManager
{
public:
	// singleton
	static AccountManager* Instance()
	{
		static AccountManager instance;
		return &instance;
	}

	Account* GetAccount(int32 id)
	{
		// ���ôٹ������� ������ �� �ְ�
		lock_guard<mutex> guard(_mutex);
		return nullptr;
	}

	void ProcessLogin();
private:
	mutex _mutex;
};

