// 05_�ֹ���װSEH�ڵ�.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <windows.h>

EXCEPTION_DISPOSITION NTAPI seh(struct _EXCEPTION_RECORD *ExceptionRecord,PVOID EstablisherFrame,struct _CONTEXT *ContextRecord,PVOID DispatcherContext)
{
	printf("seh\n");
	// ����ִ��
	return ExceptionContinueExecution;
}

int _tmain(int argc, _TCHAR* argv[])
{
//	EXCEPTION_REGISTRATION_RECORD node;
	/*
	  * �����쳣�� , ����ϵͳʹ��fs�μĴ����ҵ�TEB, 
	  * ͨ��TEB.ExceptionList �ҵ�SEH�����ͷ�ڵ�, 
	  * ͨ���ڵ��м�¼���쳣�������ĵ�ַ���øú���.
	*/
// 	node.Handler = seh;
// 	node.Next = NULL;

	_asm
	{
		push seh; // ��SEH�쳣�������ĵ�ַ��ջ
		push fs:[0];//��SEHͷ�ڵ�ĵ�ַ��ջ
		;// esp + 0 -- > [fs:0]; node.Next;
		;// esp + 4 -- > [seh]; node.handler;
		mov fs:[0], esp;// fs:[0] = &node;
	}


	*(int*)0 = 0;


	// ƽ��ջ�ռ�
	// ��ԭFS:[0]ԭʼ��ͷ�ڵ�
	_asm{
		pop fs : [0]; // ��ջ��������(ԭ�쳣ͷ�ڵ�ĵ�ַ)�ָ���FS:[0],Ȼ����ƽ��4���ֽڵ�ջ
		add esp, 4; // ƽ��ʣ�µ�4�ֽڵ�ջ.
	}
	return 0;
}

