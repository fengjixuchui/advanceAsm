// �ڴ����_�޸��ڴ��ҳ����.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <Windows.h>


DWORD handleException( void *pBuff , DWORD dwOldProtect ) {

	// ���ڴ������޸Ļ�ԭ��������
	VirtualProtect( pBuff , 10 * sizeof( DWORD ) , dwOldProtect , &dwOldProtect );

	// ����EXCEPTION_CONTINUE_EXECUTIONʱ, �����쳣�������ٴ�ִ�д���.
	return EXCEPTION_CONTINUE_EXECUTION;
}

int _tmain(int argc, _TCHAR* argv[])
{
	// 1. �õ����̾��
	HANDLE hDict = OpenProcess(PROCESS_VM_READ| PROCESS_VM_WRITE| PROCESS_VM_OPERATION,
							   FALSE,
							   1452);
	// 2. ��ȡ�����ڴ�
	TCHAR buff[512] = { 0 };
	DWORD dwRead;
	ReadProcessMemory( hDict ,
		(LPVOID)0x5AE6538 ,
					   buff ,
					   20 ,
					   &dwRead );
	wprintf(L"%s", buff);

	//3. д�����ڴ�
	WriteProcessMemory(hDict,
						(LPVOID)0x5AE6538,
					   L"������",
					   7,
					   &dwRead);
	LPBYTE pDictBuff ;
	pDictBuff = (LPBYTE)VirtualAllocEx( hDict ,
										NULL ,
										64 * 1024 ,
										MEM_RESERVE | MEM_COMMIT ,
										PAGE_READWRITE);

	// �ͷ�Զ�̽����ڴ�.
	VirtualFreeEx(hDict, pDictBuff, 64 * 1024, MEM_DECOMMIT);

	// ֱ�Ӷ�д���������ڴ��ַʱ, ʵ�����޸ĵ��Ǳ����̵�
	// �����ڴ��ַ.
	//*pDictBuff = 10;
	//VirtualProtectEx(hDict,
	//				(LPVOID)0x5AE6538,
	//				 10,
	//				 PAGE_READONLY,
	//				 &dwRead);


	// �޸��ڴ��ҳ

	DWORD* pBuff = new DWORD;
	*pBuff = 0x12345678;

	DWORD dwOldProtect = 0;
	// ����ǰִ�д�����ڴ��ҳ�����޸�Ϊֻ��
	VirtualProtect( pBuff, /*��Ҫ���ڴ��ҳ���ȶ���.���������,�������Զ�����ȡ��*/
					sizeof(DWORD), /*��Ҫ���ڴ��ҳ���ȶ���,���������,���Զ�����ȡ֤*/
					PAGE_READONLY, 
					&dwOldProtect /*���ɵ��ڴ��ҳ�������*/
					);

	HANDLE hProc = OpenProcess(PROCESS_VM_OPERATION,
							   FALSE,
							   512) ;

	VirtualProtectEx(hProc,/*Ҫ�޸ĵ�Ŀ����̾��*/
					(LPVOID)0x1000,/*Ŀ������е������ַ*/
					 sizeof(DWORD), /*��Ҫ���ڴ��ҳ���ȶ���,���������,���Զ�����ȡ֤*/
					 PAGE_READONLY,
					 &dwOldProtect /*���ɵ��ڴ��ҳ�������*/
					);

	//__try {

		// �����޸��ڴ�, ����λ�ʧ��,��Ϊû��д��Ȩ��
		*pBuff = 0x87654321;
		
	//}
	//__except( handleException(pBuff,dwOldProtect) ) {
	//}

	
	return 0;
}

