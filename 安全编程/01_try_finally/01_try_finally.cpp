// 01_try_finally.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <windows.h>



int _tmain(int argc, _TCHAR* argv[])
{
	__try{
		printf("try��\n");
		//return 0;
		*(int*)0 = 0;
		__leave; // ��������ʽ�뿪try�Ĺؼ���
	}
	__finally{
		printf("finally��\n");
	}
	printf("main\n");
	return 0;
}

