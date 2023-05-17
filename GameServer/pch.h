#pragma once
//Gameserver에서 자주 활용되는 헤더
//미리 빌드해서 빠르게 사용하는 장점
// 단점은 이곳이 수정되면 굉장히 많은 부분이 영향을 받음.
// 하지만 빠르다.

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

//전처리기 단계에서 실행됨.
// properties - General - VC directories - SolutionDir\Libraries는 설정되어 있다.
//debug mode
#ifdef _DEBUG
#pragma comment(lib, "Debug\\ServerCore.lib")
#else
#pragma comment(lib, "Release\\ServerCore.lib")
#endif

#include "CorePch.h"