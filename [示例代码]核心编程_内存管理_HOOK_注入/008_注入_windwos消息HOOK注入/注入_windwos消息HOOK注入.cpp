// ע��_windwos��ϢHOOKע��.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "../messageHookDll/messageHookDll.h"
#include <Windows.h>

int _tmain(int argc, _TCHAR* argv[])

{
	cb_installHook	pfnInstallHook = NULL;

	HMODULE hHookDll = LoadLibrary( L"messageHook.dll" );
	if( hHookDll == NULL ) {
		printf( "dll����ʧ��\n" );
		return 0;
	}

	pfnInstallHook = GetProcAddress( hHookDll , FUNCTION_INSTALLHOOK );
	if( pfnInstallHook == NULL ) {
		printf( "��ȡdll��������ʧ��\n" );
		return 0;
	}

	pfnInstallHook( );

	system( "pause" );
	return 0;
}

