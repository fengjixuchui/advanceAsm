//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// �������û�г����ڰ�ť, ���Լ������º�.
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////



#include "resource.h"
#include <shlobj.h>

BOOL    IsAdmin( HANDLE hProcess );

INT_PTR CALLBACK DlgProc( HWND hWnd , UINT uMsg , WPARAM wParam , LPARAM lParam );

BOOL    IsAdmin( HANDLE hProcess );


 int WINAPI WinMain( _In_ HINSTANCE hInstance , _In_opt_ HINSTANCE hPrevInstance , _In_ LPSTR lpCmdLine , _In_ int nShowCmd )
 {
     DialogBox( hInstance , MAKEINTRESOURCE( IDD_DIALOG1 ) , NULL , DlgProc );
 }

 INT_PTR CALLBACK DlgProc( HWND hWnd , UINT uMsg , WPARAM wParam , LPARAM lParam )
 {
 
     switch( uMsg ) {
 
         case WM_INITDIALOG:
         {
             HWND hButton = GetDlgItem( hWnd , BTN_RunAsAdmin );
 
             // �жϵ�ǰ�Ƿ����Թ���Ա������еĽ���.
             if( !IsAdmin( GetCurrentProcess( ) ) ) {
 
                // �������, ��һ����ť����Ϊ�����Ʊ�־�İ�ť.
                Button_SetElevationRequiredState( hButton , TRUE );
             }
             else {
 
                 // ����Ѿ��Թ���Ա�������,�򽫰�ť����
                 ShowWindow( hButton , SW_HIDE );
             }
         }
         break;
 
         case WM_COMMAND:
         {
             switch( LOWORD(wParam) ) {
                 
                case BTN_RunAsAdmin:  // �Թ���Ա������еİ�ť������¼�
                 {
                     // 1. ��ȡ������·��
                     TCHAR path[ MAX_PATH ] = { 0 };
                     DWORD dwSize = MAX_PATH;
                     QueryFullProcessImageName( GetCurrentProcess( ) , 0 , path , &dwSize );
 
                     // ���ش���
                     ShowWindow( hWnd , SW_HIDE );
 
                     // 2. �Թ���Ա�������
                     ShellExecute( hWnd , 
                                   L"runas" , /*ϵͳ�����˼�Թ�������һ������*/
                                   path , 
                                   NULL , NULL , SW_SHOW );
 
                     if( ERROR_SUCCESS == GetLastError( ) ) {
                         // ��������ɹ����˳�����.
                         ExitProcess( 0 );
                     }
                     else {
                         // �����������ʧ��, �򽫴�����ʾ����
                         ShowWindow( hWnd , SW_SHOW );
                     }
                 }
                 break;
             }
         }
         break;
 
         case WM_CLOSE:
             EndDialog( hWnd , 0 );
             break;
         default:return FALSE;
            break;
     }
 
     return TRUE;
 }
 


BOOL    IsAdmin( HANDLE hProcess )
{
    HANDLE hToken = NULL;
    OpenProcessToken( hProcess , TOKEN_QUERY , &hToken );

    TOKEN_ELEVATION_TYPE tokenType = TokenElevationTypeDefault ; // ���ڽ�����������

    DWORD dwRetSize = 0; // ���ڽ��պ��������Ϣ���ֽ���
    // 2. ��ѯ���������е�Ȩ������ֵ.( ���ֵ���¼��ǰ�������Ǻ�������( ϸ�ڽ�17_Ȩ�޹���_���ƵĻ�ȡ.cpp ) )
    GetTokenInformation( hToken ,
                         TokenElevationType ,// ��ȡ���Ƶĵ�ǰ�����ȼ�
                         &tokenType ,
                         sizeof( tokenType ) ,
                         &dwRetSize // ���軺�������ֽ���
                         );

    // �������Ƶ������������Ӧ����Ϣ
    if( TokenElevationTypeFull == tokenType ) {
        // 3. ���������TokenElevationTypeFull , ��ӵ���������ϵ�����,���Ը���������κ���Ȩ,���ص�0��ִ�д���.
        return TRUE;
    }
    // 4. �����������, ����Ҫ�Թ���Ա����������б�����. ���������Ե������ķ������ʣ�µ�����.
    else if( TokenElevationTypeDefault == tokenType ) {
       
        // Ĭ���û�������һ��û��Ȩ�޵ı�׼�û�
        // Ҳ������UAC���ر�.
        // ֱ�ӵ���ϵͳ�ĺ���IsUserAnAdmin ���ж��û��Ƿ��ǹ���Ա.
        return IsUserAnAdmin( );
    }
    else if( TokenElevationTypeLimited == tokenType ) {
       
        return FALSE;
    }

    return FALSE;
}

