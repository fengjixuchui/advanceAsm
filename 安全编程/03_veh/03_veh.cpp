// 03_veh.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <windows.h>


LONG WINAPI veh(EXCEPTION_POINTERS* pExce)
{
	printf("veh\n");
	// ����ִ��, ˵���쳣�ѱ�����,�����쳣��ָ���
	// ������ִ��
	EXCEPTION_CONTINUE_EXECUTION;
	// ����һ��veh�ڵ㴦���쳣.
	return EXCEPTION_CONTINUE_SEARCH;
}

LONG WINAPI seh(EXCEPTION_POINTERS* pExce){
	printf("seh\n");
	// ����һ��veh�ڵ㴦���쳣.
	return EXCEPTION_CONTINUE_SEARCH;
}

int _tmain(int argc, _TCHAR* argv[])
{
	//1. ���쳣������ע�ᵽϵͳ
	AddVectoredExceptionHandler(TRUE, veh);
	__try{

		*(int*)0 = 0;
	}
	__except (seh(GetExceptionInformation())){

	}

	return 0;
}

