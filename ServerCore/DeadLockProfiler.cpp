#include "pch.h"
#include "DeadLockProfiler.h"

void DeadLockProfiler::PushLock(const char* name)
{
	LockGuard guard(_lock);

	// ���̵� ã�ų� �߱�
	int32 lockId = 0;

	auto findIt = _nameToId.find(name);
	// ���̵� ���� �߱�
	if (findIt == _nameToId.end())
	{
		lockId = static_cast<int32>(_nameToId.size()); 
		_nameToId[name] = lockId;
		_idToName[lockId] = name;
	}
	// ���̵� ã��
	else 
	{
		lockId = findIt->second;
	}
	// ��� �ִ� ���� �־��ٸ�
	if (_lockStack.empty() == false)
	{
		// ������ �߰ߵ��� ���� ���̽���� ����� ���� �ٽ� Ȯ���Ѵ�.
		const int32 prevId = _lockStack.top();
		//��������� ������ �� �ְ� �س���, �� ��Ȳ�� ������ƴϴϱ� �����Ų��.
		if (lockId != prevId)
		{
			set<int32>& history = _lockHistory[prevId];
			// ó�� �߰��� ���̸� �����丮 �߰� & ����Ŭ Ȯ��
			if (history.find(lockId) == history.end())
			{
				history.insert(lockId);
				CheckCycle();
			}
		}
	}

	_lockStack.push(lockId);
}

// lockStack���� lock ������
void DeadLockProfiler::PopLock(const char* name)
{
	//multi-thread ȯ���̴ϱ�
	LockGuard guard(_lock);
	if (_lockStack.empty())
		CRASH("MULTIPLE UNLOCK");

	int32 lockId = _nameToId[name];
	// Ȥ�ó� Ǫ��,�� ������ ������ �������� �̻����� ����
	if(_lockStack.top() != lockId)
		CRASH("INVALID UNLOCK");


	_lockStack.pop();
}

void DeadLockProfiler::CheckCycle()
{
	const int32 lockCount = static_cast<int32>(_nameToId.size());
	_discoveredOrder = vector<int32>(lockCount, -1);
	_discoveredCount = 0;
	_finished = vector<bool>(lockCount, false);
	_parent = vector<int32>(lockCount, -1);

	for (int32 lockId = 0; lockId < lockCount; lockId++)
		Dfs(lockId);

	_discoveredOrder.clear();
	_finished.clear();
	_parent.clear();

}

void DeadLockProfiler::Dfs(int32 here)
{
	// ���� ���� �߰�
	if (_discoveredOrder[here] != -1)
		return;
	_discoveredOrder[here] = _discoveredCount++;

	// ��� ������ ������ ��ȸ�Ѵ�.
	auto findIt = _lockHistory.find(here);
	if (findIt == _lockHistory.end())
	{
		_finished[here] = true;
		return;
	}

	set<int32>& nextSet = findIt->second;
	for (int32 there : nextSet)
	{
		// ���� �湮�� ���� ���ٸ� �湮
		if (_discoveredOrder[there] == -1)
		{
			_parent[there] = here;
			Dfs(there);
			continue;
		}

		// here�� there���� ���� �߰ߵǾ��ٸ�, there�� here���� ���߿� �߰ߵ� ����: ������
		if (_discoveredOrder[here] < _discoveredOrder[there])
			continue;

		// �������� �ƴϰ� dfs(there)�� ���� �������� �ʾҴٸ� there�� here ���� ���� �߰ߵ� ����: ������
		if (_finished[there] == false)
		{
			printf("%s -> %s\n", _idToName[here], _idToName[there]);

			int32 now = here;
			// ����Ŭ ���
			while (true)
			{
				printf("%s -> %s\n", _idToName[_parent[now]], _idToName[now]);
				now = _parent[now];
				if (now == there)
					break;

			}


			CRASH("DEADLOCK_DETECTED");
		}
	}
	_finished[here] = true;
}
