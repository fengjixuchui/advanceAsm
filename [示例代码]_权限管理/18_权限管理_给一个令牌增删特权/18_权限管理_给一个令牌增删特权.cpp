// 18_Ȩ�޹���_��һ��������ɾ��Ȩ.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <windows.h>
#include <shlobj.h>


// ��������Ӷ�Ӧ��Ȩ��
// HANDLE hProcess : Ҫ���Ȩ�޵Ľ���
// const TCHAR* pszPrivilegeName : Ҫ��ӵ���Ȩ������
BOOL    AddPrivilege( HANDLE hProcess , const TCHAR* pszPrivilegeName );

// �ж��Ƿ��ǹ���ԱȨ��
BOOL    IsAdmin( HANDLE hProcess);

// �Թ���Ա�������ָ���Ŀ�ִ���ļ�
void    RunAsAdmin( const TCHAR* pszExeFile );


int _tmain( int argc , _TCHAR* argv[ ] )
{
    // ��������������ܻ���Ϊ��Ȩ���Ͷ�ִ��ʧ��
    HANDLE hProcess = NULL;
    hProcess = OpenProcess( PROCESS_ALL_ACCESS , FALSE , 12320 );

    if( hProcess == NULL ) {
        printf( "������:%x\n" , GetLastError( ) );
    }

    // ����ȨȨ��������SeDebug��Ȩ, ���ܹ����κν���.
    // ����ֻ���Թ���Ա�������,���Ҳ��������ƵĹ���Ա��ݲ����޸�,���,
    // ��Ҫ�жϵ�ǰ�Ƿ���й���Ա��ȫ��Ȩ��
    // �������, ����Ҫ�Թ���Ա������б�����
    
    // �жϵ�ǰ�����Ƿ��Թ���Ա�������
    if( !IsAdmin( GetCurrentProcess( ) ) ) {

        // �Թ���Ա������б�����
        //  1 ��ȡ�����̵��ļ�·��.
        TCHAR path[ MAX_PATH ] = { 0 }; // ��Ҫ��ʼ��
        DWORD dwPathSize = MAX_PATH;
        QueryFullProcessImageName( GetCurrentProcess( ) , 0 , 
								   path , 
								   &dwPathSize );

        // 2 ���ô������̵�API���б�����.
        ShellExecute( NULL ,            // ���ھ��,û������NULL
                      _T( "runas" ) ,   // �Թ���Ա������е���Ҫ����
                      path ,            // �������еĳ����·��(�����Ǳ�����)
                      NULL ,            // �����в���
                      NULL ,            // �½��̵Ĺ���Ŀ¼��·��
                      SW_SHOW           // ���������ʾ��־(��С��,���, ��ʾ,���ص�)
                      );

        // �˳�������
        ExitProcess( 0 ); 
    }



    // ��Ȩ��������SeDbugȨ��
    if( !AddPrivilege( GetCurrentProcess( ) , SE_DEBUG_NAME/*�ַ�����ʽ��Ȩ����*/ ) ) {
		printf( "����Ȩ��ʧ��\n" );
		system( "pause" );
        return 0;
    }

    // �򿪽��̾��
    hProcess = OpenProcess( PROCESS_ALL_ACCESS , FALSE , 12320 );
    if( hProcess == NULL ) {
        printf( "������:%x\n" , GetLastError( ) );
		system( "pause" );
        return 0;
    }
    

    printf( "�򿪳ɹ�\n" );
    system( "pause" );
    return 0;
}




BOOL    AddPrivilege( HANDLE hProcess , const TCHAR* pszPrivilegeName )
{
    // ���̵���Ȩʹ��LUIDֵ����ʾ, ���, ��Ҫ�Ȼ�ȡ�����Ȩ������Ӧ��LUIDֵ


    // 0. ��ȡ��Ȩ��Ӧ��LUIDֵ
    LUID privilegeLuid;
    if( !LookupPrivilegeValue( NULL , pszPrivilegeName , &privilegeLuid ) )
        return FALSE;


    // 1. ��ȡ����������
    HANDLE hToken = NULL;
    // ������ʱ, ��Ҫ����TOKEN_ADJUST_PRIVILEGES Ȩ��(���Ȩ�������޸�������Ȩ)
    if( !OpenProcessToken( GetCurrentProcess( ) , TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY , &hToken ) ) {
        printf( "������:%x\n" , GetLastError( ) );
        return 0;
    }

    // 2. ʹ��������Ȩ�޸ĺ�����SeDebug��LUID��Ȩֵ��ӵ�����������
    TOKEN_PRIVILEGES tokenPrivieges; // �µ���Ȩ

    // ʹ����Ȩ��LUID����ʼ���ṹ��.
    tokenPrivieges.PrivilegeCount = 1; // ��Ȩ����
    tokenPrivieges.Privileges[ 0 ].Luid = privilegeLuid; // ����ȨLUID���浽������
    tokenPrivieges.Privileges[ 0 ].Attributes = SE_PRIVILEGE_ENABLED;// ������ֵ��Ϊ����(�н���,�Ƴ�������״̬)



    // ���ú��������Ȩ
    return AdjustTokenPrivileges( hToken ,              // Ҫ�����Ȩ������
                                  FALSE ,               // TRUE���Ƴ���Ȩ, FALSE�������Ȩ
                                  &tokenPrivieges ,     // Ҫ��ӵ���Ȩ����
                                  sizeof( tokenPrivieges ) ,// ������Ȩ����Ĵ�С
                                  NULL ,                // �ɵ���Ȩ����
                                  NULL                  // �ɵ���Ȩ����ĳ���
                                  );
}


BOOL    IsAdmin( HANDLE hProcess )
{
    HANDLE hToken = NULL;
    OpenProcessToken( hProcess , TOKEN_QUERY , &hToken );

    TOKEN_ELEVATION_TYPE tokenType = TokenElevationTypeDefault ; // ���ڽ�����������

    DWORD dwRetSize = 0; // ���ڽ��պ��������Ϣ���ֽ���

    // 2. ��ѯ���������е�Ȩ������ֵ.( ���ֵ���¼��ǰ�������Ǻ�������( ϸ����17_Ȩ�޹���_���ƵĻ�ȡ.cpp ) )
    GetTokenInformation( hToken ,
                         TokenElevationType ,// ��ȡ���Ƶĵ�ǰ�����ȼ�
                         &tokenType ,
                         sizeof( tokenType ) ,
                         &dwRetSize // ���軺�������ֽ���
                         );


    // �������Ƶ������������Ӧ����Ϣ
    if( TokenElevationTypeFull == tokenType ) {
        // 3. ���������TokenElevationTypeFull , ��ӵ���������ϵ�����,���Ը���������κ���Ȩ
        printf( "����Ա�˻�,��ӵ��ȫ����Ȩ��,���Ը���������κ���Ȩ\n" );
        return TRUE; 
    }
    // 4. �����������, ����Ҫ�Թ���Ա����������б�����. ���������Ե������ķ������ʣ�µ�����.
    else if( TokenElevationTypeDefault == tokenType ) {
        printf( "Ĭ���û�, ������һ����ͨ�û�, �����ǹر�UACʱ��¼�Ĺ���Ա�û�\n" );

        // ����ϵͳ����IsUserAnAdmin, ��һ��ȷ������ͨ�û����ǹ���Ա�û�
        return IsUserAnAdmin();
    }
    else if( TokenElevationTypeLimited == tokenType ) {

        // �ж������Ƶ��û��ǹ���Ա
        // ����ǹ���Ա, ����������лᱣ���й���Ա��SID

        // 1. ��ȡϵͳ�ڼ�����Ա�û���SID
        SID adminSid;
        DWORD dwSize = sizeof( adminSid );
        CreateWellKnownSid( WinBuiltinAdministratorsSid , // ��ȡSID������,������ϵͳ�ڼ�����Ա
                            NULL , // ��NULL,��ȡ���ؼ�����Ĺ���Ա
                            &adminSid ,// ��������Ĺ���ԱSID
                            &dwSize // ����ṹ�Ĵ�С,Ҳ��Ϊ���
                            );

        // ��ȡ�����Ƶ���������(�����Ƶ����ƶ�����һ�����ӵ�����,�����Ƶ�������ʽ����������������. )
        TOKEN_LINKED_TOKEN linkToken;
        GetTokenInformation( hToken ,
                             TokenLinkedToken , // ��ȡ���ӵ����ƾ��
                             &linkToken ,
                             sizeof( linkToken ) ,
                             &dwSize
                             );

        // �����ӵ������в����Ƿ���й���Ա��SID
        BOOL    bIsContain = FALSE; // ���ڱ����Ƿ����.
        CheckTokenMembership( linkToken.LinkedToken , // ����������м��
                              &adminSid ,             // ����������Ƿ������SID
                              &bIsContain );           // ���TRUE�����,��֮������



        if( bIsContain ) {
            printf( "Ȩ�ޱ��˸�������ƹ���Ա�˻�, ����Ȩ�ޱ��ƴ���\n" );
        }


        return bIsContain; // �����Թ���ԱȨ������
    }

    return FALSE;
}

