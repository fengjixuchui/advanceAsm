// ע��_Զ���߳�ע��.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <Windows.h>

bool injectDll( DWORD dwPid , const char* pszDllPath );

int _tmain(int argc, _TCHAR* argv[])
{
	DWORD	dwPid;
	char	szDllPath[ MAX_PATH ] = { "E:\\code\\���ϴ���\\���ı��_�ڴ����\\Debug\\testDll2.dll"};


	printf( "����Ҫע�뵽�Ľ���PID:" );
	scanf_s( "%d[*]" , &dwPid );

	printf( "����Ҫע�뵽��DLL:" );
	//scanf_s( "%s" , szDllPath , MAX_PATH );

	injectDll( dwPid , szDllPath );

	return 0;
}


// 
bool injectDll( DWORD dwPid , const char* pszDllPath ) {

	/**
	* Զ��ע���Ŀ��: ��һ��DLLע�뵽�������̵ĵ�ַ�ռ���.
	* ע�뷽��:
	* ����֪ʶ:
	* 1. windwos����һ������Զ���̵߳�API. ���API�ܹ���
	*    Ŀ������д���һ���߳�. �ڴ����߳�ʱ,�ܹ����Լ�ָ
	*	 ���̵߳Ļص�����, ����������ĵ�ַ������Ŀ�����
	*    �ĵ�ַ�ռ���. �̱߳���������֮��, ��������ͻᱻ
	*	 ִ��.
	* 2. ��һ��DLL�����̼��غ�, ����ϵͳ���������ڴ��з���
	*    һ��ռ���������, �����DLL�ٴα��������̼���ʱ,
	*	 ϵͳ�����ٴη��������ڴ����������DLL,���ǽ����DLL
	*	 ���ڵ������ڴ�ӳ�䵽�½��̵������ַ�ռ���.
	*	 ���,ϵͳDLL��ÿ�ο���֮��,���ǵļ��ص�ַ���ǲ����,
	*	 ����,���н��̵�ϵͳDLL�ļ��ػ�ַ������ͬ��,��Ϊ����
	*	 �ļ��ػ�ַ��ͬ,��ÿһ��API�ĵ�ַ�����κν�����Ҳ����
	*    ��ͬ��.
	*    ʹ��CreateRemoteThread����,��Ŀ������д����߳�.
	*	 CreateRemoteThread��Ҫָ���̻߳ص�����,����ص�
	*    ����ֻ��һ������, ��LoadLibrary���ϵͳAPI�պ�Ҳ
	*    ֻ��һ������.��LoadLibrary��һ��ϵͳDLL�еĺ���,
	*	 �������н����еĵ�ַ����ͬһ��, ����Ϊ��������.
	*	 ����һ��, �����Ǵ���Զ���߳�,LoadLibrary�ͻᱻ��
	*	 ��,��������ֻ��Ҫ��LoadLibrary������һ��DLL·��
	*    �ͳɹ���. ������ַ������뱣����Ŀ����̵ĵ�ַ�ռ���.
	*	 ��Ϊ,Զ���̵߳Ļص�����LoadLibrary��Ȼ���κν�����
	*    �ĵ�ַ����ͬһ��, ����ִ����������������,���,����
	*    �������õ��ĵ�ַҲ�����������ڽ��̵ĵ�ַ.
	* ע�����:
	* 1. ʹ��VirtualAllocEx��Ŀ������п����ڴ�ռ�.
	* 2. ʹ��WriteProcessMemory��DLL·��д�뵽Ŀ�������
	*	 �����ڴ�ռ���.
	* 3. ����Զ���߳�, ʹ��LoadLibrary������Ϊ�̵߳Ļص�����,
	*    ʹ��VirtualAllocEx���ٳ����ڴ�ռ��׵�ַ��Ϊ�ص������Ĳ���
	* 4. �ȴ��߳��˳�.
	* 5. ����VirtualAllocEx���ٳ������ڴ�ռ�
	*/

	bool	bRet			= false;
	HANDLE	hProcess		= 0;
	HANDLE	hRemoteThread	= 0;
	LPVOID	pRemoteBuff		= NULL;
	SIZE_T 	dwWrite			= 0 ;
	DWORD	dwSize			= 0;



	// �򿪽���
	hProcess = OpenProcess(
		PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION |PROCESS_VM_WRITE ,/*�����̺߳�д�ڴ�Ȩ��*/
		FALSE ,
		dwPid /*����ID*/
		);

	if( hProcess == NULL ) {
		printf( "�򿪽���ʧ��,�������ڱ������Ȩ��̫��,���Թ���Ա��������ٳ���\n" );
		goto _EXIT;
	}


	// 1. ��Զ�̽����Ͽ����ڴ�ռ�
	pRemoteBuff = VirtualAllocEx( hProcess ,
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
	dwSize = strlen( pszDllPath ) + 1;
	WriteProcessMemory( hProcess ,
					   pRemoteBuff,	/* Ҫд��ĵ�ַ */
					   pszDllPath,	/* Ҫд������ݵĵ�ַ */
					   dwSize,		/* д����ֽ��� */
					   &dwWrite		/* ���:����ʵ��д����ֽ��� */
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
		(LPTHREAD_START_ROUTINE)LoadLibraryA ,  /* �̻߳ص����� */
		pRemoteBuff ,							/* �ص��������� */
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

