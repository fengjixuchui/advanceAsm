// 17_Ȩ�޹���_���ƵĻ�ȡ.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <windows.h>
#include <shlobj.h>


int _tmain(int argc, _TCHAR* argv[])
{
    // ��ȡ����������
    HANDLE hToken = NULL;
    if( !OpenProcessToken( GetCurrentProcess( ) , TOKEN_QUERY , &hToken ) ) {
        printf( "������:%x\n" , GetLastError( ) );
        return 0;
    }


    // ���ڽ�����������
    TOKEN_ELEVATION_TYPE tokenType = TokenElevationTypeDefault ;


    DWORD dwRetSize = 0; // ���ڽ��պ��������Ϣ���ֽ���

    // ��ȡ������Ϣ
    GetTokenInformation( hToken ,
                         TokenElevationType ,// ��ȡ���Ƶĵ�ǰ�����ȼ�
                         &tokenType ,
                         sizeof( tokenType ) ,
                         &dwRetSize // ���軺�������ֽ���
                         );
    // �ж��Ƿ���óɹ�
    if( ERROR_SUCCESS != GetLastError( ) ) {
        printf( "������:%x\n" , GetLastError( ) );
        return 0;
    }

    // �������Ƶ������������Ӧ����Ϣ

    switch( tokenType ) {

        case TokenElevationTypeDefault:
            printf( "Ĭ���û�, ������һ����ͨ�û�, �����ǹر�UACʱ��¼�Ĺ���Ա�û�\n" );

            // ����ϵͳ����IsUserAnAdmin, ��һ��ȷ������ͨ�û����ǹ���Ա�û�
            return IsUserAnAdmin( );
            break;

        case TokenElevationTypeFull:
            printf( "����Ա�˻�,��ӵ��ȫ����Ȩ��\n" );
            break;

        case TokenElevationTypeLimited:
        {
            // �ж������Ƶ��û��ǹ���Ա
            // ����ǹ���Ա, ����������лᱣ���й���Ա��SID


            // 1. ��ȡϵͳ�ڼ�����Ա�û���SID
            byte adminSID[ SECURITY_MAX_SID_SIZE ];
            DWORD dwSize = SECURITY_MAX_SID_SIZE;
            CreateWellKnownSid( WinBuiltinAdministratorsSid , // ��ȡSID������,������ϵͳ�ڼ�����Ա
                                NULL , // ��NULL,��ȡ���ؼ�����Ĺ���Ա
                                adminSID ,// ��������Ĺ���ԱSID
                                &dwSize // ����ṹ�Ĵ�С,Ҳ��Ϊ���
                                );

            // ��ȡ�����Ƶ���������(�����Ƶ����ƶ�����һ�����ӵ�����,�����Ƶ�������������������������. )
            TOKEN_LINKED_TOKEN linkToken;
            GetTokenInformation( hToken ,
                                 TokenLinkedToken ,
                                 &linkToken ,
                                 sizeof( linkToken ) ,
                                 &dwSize
                                 );

            // �����ӵ������в����Ƿ���й���Ա��SID
            BOOL    bIsMember = FALSE; // ���ڱ����Ƿ����.
            CheckTokenMembership( linkToken.LinkedToken , 
                                  &adminSID ,
                                  &bIsMember );

            
            if( bIsMember ) {

                printf( "Ȩ�ޱ��˸�������ƹ���Ա�˻�, ����Ȩ�ޱ��ƴ���\n" );
            }
        }
        break;
    }

    system( "pause" );
	return 0;
}

