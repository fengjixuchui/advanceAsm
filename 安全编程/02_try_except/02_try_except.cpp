// 02_try_except.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <windows.h>

int* g_pNum = NULL;


void fun()
{
	__try{
		*(int*)0 = 0;
	}
	__except (EXCEPTION_CONTINUE_SEARCH){

	}
}

// �쳣���˺���, ���ڴ�������г��ֵ��쳣.
int seh(EXCEPTION_POINTERS* pExce)
{
	
	printf("��%08X��������%08X�쳣\n",
		pExce->ExceptionRecord->ExceptionAddress,
		pExce->ExceptionRecord->ExceptionCode);
	printf("EAX:%08X ECX:%08X\n",
		pExce->ContextRecord->Eax,
		pExce->ContextRecord->Ecx);

	pExce->ContextRecord->Eax = (DWORD)new int;
	return EXCEPTION_CONTINUE_EXECUTION;
}


int _tmain(int argc, _TCHAR* argv[])
{
	//ִ�д������(except��)
	EXCEPTION_EXECUTE_HANDLER;

	//��������
	// ���쳣���ݵ���һ���try��except,���쳣������ִ��
	EXCEPTION_CONTINUE_SEARCH;
	
	//����ִ��
	// ����ִ�в����쳣������ָ��
	EXCEPTION_CONTINUE_EXECUTION;

	__try{
		fun();
		//*(int*)0 = 0;
		*g_pNum = 10;
		printf("try��\n");
	}
	__except ( seh(GetExceptionInformation())){
		printf("finally��\n");
	}


	printf("main()\n");
	__try{
		*(int*)0 = 0;
		printf("try��\n");
	}
	__except (EXCEPTION_CONTINUE_EXECUTION){
		printf("finally��\n");
	}

	return 0;
}

