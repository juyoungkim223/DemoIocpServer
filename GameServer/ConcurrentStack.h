#pragma once
#include <mutex>

template<typename T>

class LockFreeStack
{
	struct Node; // ���漱��
	struct CountedNodePtr
	{
		// int64�� �ѹ��� ǥ���� �� �ֵ��� ó���ϴ� ���� �ļ������Եȴ�.
		int32 externalCount = 0; //e.g 16
		Node* ptr = nullptr; //e.g 48 
	};

	struct Node
	{
		// make_shared�� ����Ʈ�� ������ش�.
		Node(const T& value) : data(make_shared<T>(value))
		{

		}
		shared_ptr<T> data;
		atomic<int32> internalCount = 0;
		CountedNodePtr next;
	};
public:
	// ���� ���� [][][][] <--- ������ ����
	//          [head]
	// ���� head�� �־���.
	// ������ �ٴ�, �����ʿ��� ������ ��
	void Push(const T& value)
	{
		CountedNodePtr node;
		node.ptr = new Node(value);
		node.externalCount = 1;
		
		node.ptr->next = _head;
		// node.ptr->next�� head�� ������ head�� node�� �־���
		while (_head.compare_exchange_weak(node.ptr->next, node) == false)
		{

		}
	}
	// head�� ����Ű�� ������ �ϳ� ����
	shared_ptr<T> TryPop()
	{
		CountedNodePtr oldHead = _head;
		while (true)
		{
			// ������ ȹ�� ( ������ ���� + 1 �� �ְ� �̱�) ���� ��ȣǥ
			IncreaseHeadCount(oldHead);
			Node* ptr = oldHead.ptr;

			if (ptr == nullptr)
				return shared_ptr<T>();
			// ������ ȹ�� (ptr->next�� head���ٲ�ġ�� �� �ְ� �̱�)
			if (_head.compare_exchange_strong(oldHead, ptr->next))
			{
				shared_ptr<T> res;
				res.swap(ptr->data);

				//external: 1 -> 2(+1) ������
				//internla: 0
				const int32 countIncrease = oldHead.externalCount - 2;
				if (ptr->internalCount.fetch_add(countIncrease) == -countIncrease)
					delete ptr;
				return res;
			}
			else if (ptr->internalCount.fetch_sub(1) == 1)
			{
				//�������� ������� �������� ���� => �޼����� ���� delete���Ѵ�.
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
