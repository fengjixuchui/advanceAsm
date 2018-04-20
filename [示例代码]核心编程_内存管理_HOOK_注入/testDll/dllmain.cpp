// dllmain.cpp : ���� DLL Ӧ�ó������ڵ㡣
#include "stdafx.h"
#include "detours/detours.h"

#ifdef _X64
#pragma comment(lib,"detours/lib.X64/detours.lib")
#else
#pragma comment(lib,"detours/lib.X86/detours.lib")
#endif // _X64

// ���庯��ָ������
typedef DWORD( WINAPI* fnMyMessageBox )( HWND , TCHAR* , TCHAR* , DWORD );

fnMyMessageBox g_pSrcMessageBox = (fnMyMessageBox)&MessageBoxW;

DWORD WINAPI MyMessageBox( HWND , TCHAR* , TCHAR* , DWORD ) {

	g_pSrcMessageBox( 0 , L"���Ӻ���" , L"����" , 0 );
	return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	{
		DetourTransactionBegin( );
		DetourUpdateThread( GetCurrentThread( ) );

		// HOOK
		DetourAttach( (PVOID*)&g_pSrcMessageBox ,
					  MyMessageBox );

		DetourTransactionCommit( );

	}
	break;
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

