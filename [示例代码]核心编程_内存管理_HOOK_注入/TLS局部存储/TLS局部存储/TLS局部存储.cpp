// TLS�ֲ��洢.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <windows.h>

// ��ͨ��ȫ�ֱ���
int g_nNum2 = 0xFFFFFFFF;

// TLS����
__declspec ( thread ) int  g_nTlsNum = 0x11111111;
__declspec ( thread ) char g_szTlsStr[ ] = "TLS g_nNum = 0x%p ...\r\n";

// TLS�ص�����A
void NTAPI t_TlsCallBack_A( PVOID DllHandle , DWORD Reason , PVOID Red )
{
	g_nTlsNum = 0;
    if( DLL_THREAD_DETACH == Reason ) // ����߳��˳����ӡ��Ϣ
        printf( "t_TlsCallBack_A -> ThreadDetach!\r\n" );

	if(DLL_THREAD_ATTACH == Reason) {
		HWND hDbg = FindWindow(L"Qt5QWindowIcon", L"x32dbg");
		if(hDbg != NULL) {
			ExitProcess(0);
		}
	}
    return;
}

// TLS�ص�����B
void NTAPI t_TlsCallBack_B( PVOID DllHandle , DWORD Reason , PVOID Red )
{
    if( DLL_THREAD_DETACH == Reason ) // ����߳��˳����ӡ��Ϣ
        printf( "t_TlsCallBack_B -> ThreadDetach!\r\n" );

    //ExitProcess( 0 );
    return;
}

/*
* ע��TLS�ص�������".CRT$XLB"�ĺ����ǣ�
* CRT����ʹ��C RunTime����
* X��ʾ��ʶ�����
* L��ʾTLS callback section
* B��ʵҲ����ΪB-Y������һ����ĸ
* ��һ��˵���ǣ��������ᰴ�������ĸ��
* ˳������������ӵĶα��浽pe�ļ��С�
*/
#pragma data_seg(".CRT$XLB")
PIMAGE_TLS_CALLBACK p_thread_callback[ ] = 
{
	t_TlsCallBack_A ,  // TLS�̻߳ص�����
	t_TlsCallBack_B ,
    NULL 
};
#pragma data_seg()

DWORD WINAPI t_ThreadFun( PVOID pParam )
{
    printf( "t_Thread ->  first printf:" );
    printf( g_szTlsStr , g_nTlsNum );
    printf( "[1]��ͨ��ȫ�ֱ���:%08X\n" , g_nNum2 );
    
    g_nNum2 = 0;
    //ע������
    // �����������߳��޸Ĵ˴�
    g_nTlsNum = 0x22222222; 


    printf( "t_Thread -> second printf:" );
    printf( g_szTlsStr , g_nTlsNum );
    printf( "[2]��ͨ��ȫ�ֱ���:%08X\n" , g_nNum2 );
    return 0;
}
/*
    ��һ���̱߳�������ʱ��
    TLS�̻߳ص���������ͨ�̻߳ص�������ִ��˳��:
    1. �Ƚ�TLS�̻߳ص�������������
        1.1 TLS�̻߳ص�����ע����ٸ�,�͵��ö��ٸ�,���Ұ��ն����˳�����
    2. �ٵ����̻߳ص�����

    ��һ���߳��˳���ʱ��
    1. ��TLS�̻߳ص�������������
        1.1 TLS�̻߳ص�����ע����ٸ�,�͵��ö��ٸ�,���Ұ��ն����˳�����
*/

int _tmain( int argc , _TCHAR* argv[ ] )
{
    printf( "_tmain -> TlsDemo.exe is runing...\r\n\r\n" );

	WaitForSingleObject(CreateThread(NULL, 0, t_ThreadFun, NULL, 0, 0),
						-1);

	CreateThread(NULL, 0, t_ThreadFun, NULL, 0, 0);


    //Sleep( 500 );  // ˯��100��������ȷ����һ���߳�ִ�����
    //printf( "\r\n" );
    //CreateThread( NULL , 0 , t_ThreadFun , NULL , 0 , 0 );
	//IMAGE_TLS_DIRECTORY

    system( "pause" );
    return 0;
}
