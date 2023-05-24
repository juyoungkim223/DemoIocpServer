
/*
참조하고 있는 포인터의 Ref 카운트를 조정하는 역할
직접 delete 하지 않고 ReleaseRef를 통해서 delete 한다.
*/
class RefCountable
{
public:
	RefCountable() : _refCount(1) {}
	virtual ~RefCountable() {}
	int32 GetRefcount() { return _refCount; }
	int32 AddRef() { return ++_refCount; }
	int32 ReleaseRef()
	{
		int32 refCount = --_refCount;
		if (refCount == 0)
		{
			delete this;
		}
		return refCount;
	}
protected:
	atomic<int32> _refCount;
};

/*
레퍼런스 카운팅이 되는 포인터를 알아서 관리해준다.
SharedPtr, RefCountable 상속받아서 만든 클래스가 0 이되어서 참조하게 되지 못하는 문제를 해결해준다.
*/

template<typename T>
class TSharedPtr
{
public:
	TSharedPtr() {}
	TSharedPtr(T* ptr) { Set(ptr); }
	// 복사, 다른 애가 넘겨준 포인터 관리 right-hand-shade?
	TSharedPtr(const TSharedPtr& rhs) { Set(rhs._ptr); }
	// 이동
	TSharedPtr(const TSharedPtr&& rhs)
	{
		_ptr = rhs._ptr;
		rhs._ptr = nullptr;
	}
	// 상속 관계 복사
	template<typename U>
	TSharedPtr(const TSharedPtr<U>& rhs)
	{
		Set(static_cast<T*>(rhs._ptr));
	}
	~TSharedPtr() { Release(); }

public:
	// 복사 연산자
	TSharedPtr& operator=(const TSharedPtr& rhs)
	{
		if (_ptr != rhs._ptr)
		{
			Release();
			Set(rhs._ptr);
		}
		return *this;
	}

	// 이동 연산자
	TSharedPtr& operator=(TSharedPtr&& rhs)
	{
		Release();
		_ptr = rhs._ptr;
		rhs._ptr = nullptr;
		return *this;
	}

	bool		operator==(const TSharedPtr& rhs) const { return _ptr == rhs._ptr; }
	bool		operator==(T* ptr) const { return _ptr == ptr; }
	bool		operator!=(const TSharedPtr& rhs) const { return _ptr != rhs._ptr; }
	bool		operator!=(T* ptr) const { return _ptr != ptr; }
	bool		operator<(const TSharedPtr& rhs) const { return _ptr < rhs._ptr; }
	T* operator*() { return _ptr; }
	const T* operator*() const { return _ptr; }
	operator T* () const { return _ptr; }
	T* operator->() { return _ptr; }
	const T* operator->() const { return _ptr; }

	bool IsNull() { return _ptr == nullptr; }

private:
	inline void Set(T* ptr)
	{
		_ptr = ptr;
		if (ptr)
			ptr->AddRef();
	}

	inline void Release()
	{
		if (_ptr != nullptr)
		{
			_ptr->ReleaseRef();
			_ptr = nullptr;
		}
	}

private:
	T* _ptr = nullptr;
};