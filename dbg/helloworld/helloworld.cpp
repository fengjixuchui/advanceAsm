// helloworld.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <windows.h>

int main()
{
	for(int i = 0; i<10;++i)
		printf("hello world\n");

	__try {
		*(int*)0 = 0;
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		printf("__except\n");
	}
	printf("exit\n");
    return 0;
}

