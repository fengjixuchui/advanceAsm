// �ڴ����_�����������̵��ڴ�.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <Windows.h>


int _tmain(int argc, _TCHAR* argv[])
{
	DWORD	dwPid		= 0;
	HANDLE	hCalc		= NULL;
	LPVOID	pRemoteBuff = NULL;

	// ���ݴ��ھ����ȡ����pid
	GetWindowThreadProcessId( FindWindow( NULL , _T( "������" ) ) , &dwPid );
	if( dwPid == 0 ) {
		STARTUPINFO si = { sizeof( STARTUPINFO ) };
		PROCESS_INFORMATION pi = { 0 };
		CreateProcess( _T( "win32calc.exe" ) , 0 , 0 , 0 , 0 , 0 , 0 , 0 , &si , &pi );
		hCalc = pi.hProcess;
	}
	else {
		hCalc = OpenProcess( PROCESS_VM_OPERATION | PROCESS_VM_WRITE ,
							 FALSE ,
							 dwPid );
		if( hCalc == NULL )
			return 0;
	}

	// �����������п����ڴ�ռ�
	pRemoteBuff = VirtualAllocEx( hCalc ,  /* Ҫ�����ڴ�ռ�Ľ��̾��,���������PROCESS_VM_OPERATIONȨ�� */
								  NULL ,   /* ��ָ����ַ�������ڴ�ռ�,��NULL��ʾ��ϵͳ�Լ�ѡ�� */
								  4096 ,   /* ���ٵ�С */
								  MEM_RESERVE | MEM_COMMIT , /* ���ٿռ�ı�־ */
								  PAGE_EXECUTE_READWRITE     /* �ڴ��ҳ�ı������� */
								  );

	if( pRemoteBuff == NULL ) {
		printf( "��Զ�̽��̿����ڴ�ʧ��\n" );
		return 0;
	}


	// ������д���ڴ�
	DWORD dwWrite = 0;
	WriteProcessMemory( hCalc , 
						pRemoteBuff , 
						"Hello 15PB" , 
						20 , 
						&dwWrite );

	printf( "����ʹ��windbg���Ӽ�����,ʹ����������鿴�ڴ��ַ: da 0x%08X\n" , pRemoteBuff );
	system( "pause" );

	// �ͷ��ڴ�
	VirtualFreeEx( hCalc , pRemoteBuff , 0 , MEM_RELEASE );
	// �رվ��
	CloseHandle( hCalc );

	return 0;
}

