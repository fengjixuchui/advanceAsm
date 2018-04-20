// messageHookDll.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <windows.h>
#include "messageHookDll.h"
#include <stdio.h>
#include <stdlib.h>

HHOOK	g_hHook;
HMODULE	g_hModule;



LRESULT CALLBACK KeyboardProc( int code , WPARAM wParam , LPARAM lParam ) {

	// �ж��Ƿ�wParam��lParam���м�����Ϣ��
	//�ǵĻ���ִ�д�ӡ����
	if( code == HC_ACTION ) {

		// ��256���������״̬������ָ���Ļ������У�
		//����ɹ������
		BYTE KeyState[ 256 ] = { 0 };
		if( GetKeyboardState( KeyState ) ) {

			
			// �õ���16�C23λ������������
			UINT  keyCode = ( lParam >> 16 ) & 0x00ff;
			WCHAR wKeyCode = 0;
			// ת����ASCII��
			ToAscii( (UINT)wParam ,keyCode ,KeyState ,(LPWORD)&wKeyCode , 0 );


			// ��ȡǰ�˴���
			TCHAR szWndTitle[ 512 ];
			HWND hWnd = GetForegroundWindow( );
			// ��ȡ���ڱ���.
			GetWindowText( hWnd , szWndTitle , 512 );


			// �����ӡ����
			WCHAR szInfo[ 516 ] = { 0 };
			swprintf_s( szInfo , _countof( szInfo ) , L">>>>   [%s] : %c <<<<" , szWndTitle , (WCHAR)wKeyCode );

			OutputDebugString( szInfo );
		}
	}

	return CallNextHookEx( g_hHook , code , wParam , lParam );
}
MESSAGEHOOKDLL_API DWORD g_tid = 0;

// ��װ����
MESSAGEHOOKDLL_API int installHook( ) {

	g_hHook = SetWindowsHookEx(
		WH_KEYBOARD ,				/* ҪHOOK����Ϣ���� */
		KeyboardProc ,				/* ���ӻص� */
		g_hModule ,					/* ���ӻص��������ڵ�ģ����,������ʹ�õ��Ǳ�DLL�ľ�� */
		g_tid						/* Ҫ��ס���߳�ID,Ϊ0ʱ��ס�����߳� */
		);
	return g_hHook != NULL;
}


// ж�ع���
MESSAGEHOOKDLL_API int uninstallHook( ) {

	return UnhookWindowsHook( WH_KEYBOARD , KeyboardProc ) != 0;
}


BOOL APIENTRY DllMain( HMODULE hModule ,DWORD  ul_reason_for_call ,LPVOID lpReserved) 
{
	switch( ul_reason_for_call ) {
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		{
			g_hModule = hModule;
		}

		break;


		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			// DLL��ж��ʱж�ع���
			// uninstallHook( );
			MessageBox( NULL , L"DLL��ж��" , L"��ʾ" , 0 );
			break;
	}
	return TRUE;
}

