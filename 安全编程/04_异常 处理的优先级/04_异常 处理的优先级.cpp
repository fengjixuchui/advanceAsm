// 04_�쳣 ��������ȼ�.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <windows.h>

LONG WINAPI vch(EXCEPTION_POINTERS* pExcept){
	printf("vch\n");
	return EXCEPTION_CONTINUE_SEARCH;
}

LONG WINAPI veh(EXCEPTION_POINTERS* pExcept){
	printf("veh\n");
	return EXCEPTION_CONTINUE_SEARCH;
}

LONG WINAPI seh(EXCEPTION_POINTERS* pExcept){
	printf("seh\n");
	return EXCEPTION_CONTINUE_SEARCH;
}


LONG WINAPI ueh(EXCEPTION_POINTERS* pExcept){
	printf("ueh\n");
	return EXCEPTION_CONTINUE_SEARCH;
}

int _tmain(int argc, _TCHAR* argv[])
{
	AddVectoredContinueHandler(TRUE, vch);
	AddVectoredExceptionHandler(TRUE, veh);
	// ��64λϵͳ��, �����򱻵���ʱ,UEH���ᱻ����
	// �������ԲŻᱻ����.
	// ��32λϵͳ��,������ʱҲ�ᱻ����.
	SetUnhandledExceptionFilter(ueh);
	__try{
		*(int*)0 = 0;
	}
	__except (seh(GetExceptionInformation())){

	}
	return 0;
}

