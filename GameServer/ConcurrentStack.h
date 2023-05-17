#pragma once
#include <mutex>

template<typename T>

class LockFreeStack
{
	struct Node; // 전방선언
	struct CountedNodePtr
	{
		// int64로 한번에 표현할 수 있도록 처리하는 등의 꼼수가들어가게된다.
		int32 externalCount = 0; //e.g 16
		Node* ptr = nullptr; //e.g 48 
	};

	struct Node
	{
		// make_shared로 포인트를 만들어준다.
		Node(const T& value) : data(make_shared<T>(value))
		{

		}
		shared_ptr<T> data;
		atomic<int32> internalCount = 0;
		CountedNodePtr next;
	};
public:
	// 스택 구조 [][][][] <--- 데이터 들어옴
	//          [head]
	// 만들어서 head에 넣어줌.
	// 왼쪽이 바닥, 오른쪽에서 들어오는 것
	void Push(const T& value)
	{
		CountedNodePtr node;
		node.ptr = new Node(value);
		node.externalCount = 1;
		
		node.ptr->next = _head;
		// node.ptr->next와 head가 같으면 head에 node를 넣어줌
		while (_head.compare_exchange_weak(node.ptr->next, node) == false)
		{

		}
	}
	// head가 가르키는 곳에서 하나 빼옴
	shared_ptr<T> TryPop()
	{
		CountedNodePtr oldHead = _head;
		while (true)
		{
			// 참조권 획득 ( 현시점 기준 + 1 한 애가 이김) 은행 번호표
			IncreaseHeadCount(oldHead);
			Node* ptr = oldHead.ptr;

			if (ptr == nullptr)
				return shared_ptr<T>();
			// 소유권 획득 (ptr->next로 head를바꿔치기 한 애가 이김)
			if (_head.compare_exchange_strong(oldHead, ptr->next))
			{
				shared_ptr<T> res;
				res.swap(ptr->data);

				//external: 1 -> 2(+1) 참조권
				//internla: 0
				const int32 countIncrease = oldHead.externalCount - 2;
				if (ptr->internalCount.fetch_add(countIncrease) == -countIncrease)
					delete ptr;
				return res;
			}
			else if (ptr->internalCount.fetch_sub(1) == 1)
			{
				//참조권을 얻었으나 소유권은 실패 => 뒷수습은 내가 delete로한다.
				delete ptr;
			}
		}
	}

private:
	void IncreaseHeadCount(CountedNodePtr& oldCounter)
	{
		while (true)
		{
			CountedNodePtr newCounter = oldCounter;
			newCounter.externalCount++;
			if (_head.compare_exchange_strong(oldCounter, newCounter))
			{
				oldCounter.externalCount = newCounter.externalCount;
				break;
			}
		}
	}
private:
	atomic<CountedNodePtr> _head;

};
