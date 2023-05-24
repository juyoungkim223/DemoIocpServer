#include "pch.h"
#include "DeadLockProfiler.h"

void DeadLockProfiler::PushLock(const char* name)
{
	LockGuard guard(_lock);

	// 아이디를 찾거나 발급
	int32 lockId = 0;

	auto findIt = _nameToId.find(name);
	// 아이디 새로 발급
	if (findIt == _nameToId.end())
	{
		lockId = static_cast<int32>(_nameToId.size()); 
		_nameToId[name] = lockId;
		_idToName[lockId] = name;
	}
	// 아이디 찾음
	else 
	{
		lockId = findIt->second;
	}
	// 잡고 있는 락이 있었다면
	if (_lockStack.empty() == false)
	{
		// 기존에 발견되지 않은 케이스라면 데드락 여부 다시 확인한다.
		const int32 prevId = _lockStack.top();
		//재귀적으로 락잡을 수 있게 해놨고, 그 상황은 데드락아니니까 통과시킨다.
		if (lockId != prevId)
		{
			set<int32>& history = _lockHistory[prevId];
			// 처음 발견한 락이면 히스토리 추가 & 사이클 확인
			if (history.find(lockId) == history.end())
			{
				history.insert(lockId);
				CheckCycle();
			}
		}
	}

	_lockStack.push(lockId);
}

// lockStack에서 lock 꺼내기
void DeadLockProfiler::PopLock(const char* name)
{
	//multi-thread 환경이니까
	LockGuard guard(_lock);
	if (_lockStack.empty())
		CRASH("MULTIPLE UNLOCK");

	int32 lockId = _nameToId[name];
	// 혹시나 푸시,팝 순서가 꼬여서 락스택이 이상해진 상태
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
	// 현재 지점 발견
	if (_discoveredOrder[here] != -1)
		return;
	_discoveredOrder[here] = _discoveredCount++;

	// 모든 인접한 정점을 순회한다.
	auto findIt = _lockHistory.find(here);
	if (findIt == _lockHistory.end())
	{
		_finished[here] = true;
		return;
	}

	set<int32>& nextSet = findIt->second;
	for (int32 there : nextSet)
	{
		// 아직 방문한 적이 없다면 방문
		if (_discoveredOrder[there] == -1)
		{
			_parent[there] = here;
			Dfs(there);
			continue;
		}

		// here가 there보다 먼저 발견되었다면, there는 here보다 나중에 발견된 정점: 순방향
		if (_discoveredOrder[here] < _discoveredOrder[there])
			continue;

		// 순방향이 아니고 dfs(there)가 아직 종료하지 않았다면 there가 here 보다 먼저 발견된 정점: 역방향
		if (_finished[there] == false)
		{
			printf("%s -> %s\n", _idToName[here], _idToName[there]);

			int32 now = here;
			// 사이클 출력
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
