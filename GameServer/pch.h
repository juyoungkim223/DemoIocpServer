#pragma once
//Gameserver���� ���� Ȱ��Ǵ� ���
//�̸� �����ؼ� ������ ����ϴ� ����
// ������ �̰��� �����Ǹ� ������ ���� �κ��� ������ ����.
// ������ ������.

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

//��ó���� �ܰ迡�� �����.
// properties - General - VC directories - SolutionDir\Libraries�� �����Ǿ� �ִ�.
//debug mode
#ifdef _DEBUG
#pragma comment(lib, "Debug\\ServerCore.lib")
#else
#pragma comment(lib, "Release\\ServerCore.lib")
#endif

#include "CorePch.h"