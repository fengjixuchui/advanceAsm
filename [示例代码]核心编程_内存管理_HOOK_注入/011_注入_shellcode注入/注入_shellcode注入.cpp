// ע��_shellcodeע��.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <Windows.h>

bool injectShellcode( DWORD dwPid , const BYTE* pShellCode , DWORD dwSize ) ;

int _tmain(int argc, _TCHAR* argv[])
{

	BYTE shellcode[] = 
	{	
		"\xE8\x00\x00\x00\x00"	// call $+5
		"\x58"					// pop	eax
		"\x6A\x00"				// push	0
		"\x83\xC0\x17"			// add eax,0x17
		"\x50"					// push eax
		"\x83\xC0\x14"			// add eax,0x14
		"\x50"					// push eax
		"\x6A\x00"				// push 0
		"\xB8\x30\x88\x20\x74"	// mov eax,MessageBoxA
		"\xFF\xD0"				// call eax
		"\xC2\x04\x00"			// ret 0x04
		"����shellcode���ʺ�\0"
		"��Һ�,����shellcode!!!"
	};

	// ��MessageBoxA�����ĵ�ַд�뵽shellcode��.
	*(DWORD*)( shellcode + 19 ) = (DWORD)&MessageBoxA;


	DWORD	dwPid;
	printf( "����Ҫע�뵽�Ľ���PID:" );
	scanf_s( "%d[*]" , &dwPid );

	// ע����ƺõ�shellcode
	injectShellcode( dwPid , shellcode ,sizeof(shellcode));

	return 0;
}

bool injectShellcode( DWORD dwPid , const BYTE* pShellCode , DWORD dwSize ) {

	// shellcodeע��ķ�ʽ��DLLע��ķ�ʽ���.
	// ����shellcodeע���Ҫ��Ƚϸ�, ��Ҫ�Լ���дshellcode
	// ע�벽��:
	// 1. ��Զ�̽����п����ڴ�ռ�
	// 2. ��shellcodeд�뵽Զ�̽��̵��ڴ�ռ���
	// 3. ����Զ���߳�,����д�뵽Զ�̽����ڴ�shellcode���׵�ַ
	//    ��Ϊ�̻߳ص������ĵ�ַ.
	// 4. �ȴ��߳��˳�
	// 5. ����Զ�̽����ڴ�.

	bool	bRet = false;
	HANDLE	hProcess = 0;
	HANDLE	hRemoteThread = 0;
	LPVOID	pRemoteBuff = NULL;
	DWORD	dwWrite = 0 ;



	// �򿪽���
	hProcess = OpenProcess(
		PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE ,/*�����̺߳�д�ڴ�Ȩ��*/
		FALSE ,
		dwPid /*����ID*/
		);

	if( hProcess == NULL ) {
		printf( "�򿪽���ʧ��,�������ڱ������Ȩ��̫��,���Թ���Ա��������ٳ���\n" );
		goto _EXIT;
	}


	// 1. ��Զ�̽����Ͽ����ڴ�ռ�
	pRemoteBuff = VirtualAllocEx(
		hProcess ,
		NULL ,
		64 * 1024 , /*��С:64Kb*/
		MEM_COMMIT ,/*Ԥ�����ύ*/
		PAGE_EXECUTE_READWRITE/*�ɶ���д��ִ�е�����*/
		);
	if( pRemoteBuff == NULL ) {
		printf( "��Զ�̽����Ͽ��ٿս�ʧ��\n" );
		goto _EXIT;
	}

	// 2. ��DLL·��д�뵽�¿����ڴ�ռ���
	WriteProcessMemory(
		hProcess ,
		pRemoteBuff ,			  /* Ҫд��ĵ�ַ */
		pShellCode ,		      /* Ҫд������ݵĵ�ַ */
		dwSize ,				  /* д����ֽ��� */
		&dwWrite				  /* ���:����ʵ��д����ֽ��� */
		);
	if( dwWrite != dwSize ) {
		printf( "д��DLL·��ʧ��\n" );
		goto _EXIT;
	}

	//3. ����Զ���߳�,
	//   Զ���̴߳����ɹ���,DLL�ͻᱻ����,DLL�����غ�DllMain����
	//	 �ͻᱻִ��,�����Ҫִ��ʲô����,����DllMain�е��ü���.
	hRemoteThread = CreateRemoteThread(
		hProcess ,
		0 , 0 ,
		(LPTHREAD_START_ROUTINE)pRemoteBuff ,  /* �̻߳ص����� */
		0 ,							           /* �ص��������� */
		0 , 0 );

	// �ȴ�Զ���߳��˳�.
	// �˳��˲��ͷ�Զ�̽��̵��ڴ�ռ�.
	WaitForSingleObject( hRemoteThread , -1 );


	bRet = true;


_EXIT:
	// �ͷ�Զ�̽��̵��ڴ�
	VirtualFreeEx( hProcess , pRemoteBuff , 0 , MEM_RELEASE );
	// �رս��̾��
	CloseHandle( hProcess );

	return bRet;


}

