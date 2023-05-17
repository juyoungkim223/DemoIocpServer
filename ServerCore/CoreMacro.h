#pragma once
//사용할 define을 여기다 넣어줌.
// 전처리 과정에서 쓰이는 매크로, 상수들
// define을 여러줄에 하고 싶으면 \을 적어줘야함
// nullptr아니라고 가정하라 라는 명령, nullptr을 컴파일러가 잡을 수 있으니까
// 크래시를 내고 싶을 때 사용

/*
말 그대로 아무것도 안 하는 키워드입니다 (?)
참조값을 넘기면 이게 단순 복사를 하기 싫어서 넘기는 것인지,
원본값이 바뀌는 것인지가 알기 힘들 때가 있어
참조값을 수정 목적으로 바꾸는 경우 힌트를 주기 위해 OUT 을 붙입니다.
*/
#define OUT 


/*----------------
* LOCK
-----------------*/

#define USE_MANY_LOCKS(count) Lock _locks[count]; // 여러개 사용한다면
#define USE_LOCK USE_MANY_LOCKS(1) // 하나만 사용한다면
#define READ_LOCK_IDX(idx) ReadLockGuard readLockGurad_##idx(_locks[idx]); // 몇번째 인덱스의 락을 잡아줄 건지
#define READ_LOCK READ_LOCK_IDX(0)
#define WRITE_LOCK_IDX(idx) WriteLockGuard writeLockGurad_##idx(_locks[idx]);
#define WRITE_LOCK WRITE_LOCK_IDX(0)
/*----------------
* CRASH
-----------------*/

#define CRASH(cause)							\
{												\
	uint32* crash = nullptr;					\
	__analysis_assume(crash != nullptr);		\
	*crash = 0x111;								\
}

#define ASSERT_CRASH(expr)						\
{												\
	if(!(expr))									\
	{											\
		CRASH("ASSERT CRASH")					\
		__analysis_assume(expr);				\
	}											\
}