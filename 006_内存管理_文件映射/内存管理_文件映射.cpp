// �ڴ����_�ļ�ӳ��.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <Windows.h>


int _tmain(int argc, _TCHAR* argv[])
{
	// �ļ�ӳ�䲽��:
	// 1. ���ļ�(�ں˶���)
	// 2. �����ļ�ӳ�����(�ں˶���)
	// 3. ���ļ�ӳ�����ӳ�䵽�����ڴ�.
	HANDLE hFile = 0;
	// 1. ���ļ�.
	hFile = CreateFile( L"�ڴ����_�ļ�ӳ��.cpp" ,
						GENERIC_ALL ,
						FILE_SHARE_READ ,
						NULL ,
						OPEN_EXISTING ,
						FILE_ATTRIBUTE_NORMAL ,
						NULL
						);
	if( hFile == INVALID_HANDLE_VALUE ) {
		printf( "�ļ�������\n" );
		return 0;
	}

	DWORD   dwHigSize = 0;
	DWORD	dwLowSize = GetFileSize( hFile , &dwHigSize );
	// 2. �����ļ�ӳ�����
	HANDLE hMapping = CreateFileMapping( hFile ,		/* ��ӳ����ļ����� */
										 NULL ,				/* ��ȫ������ */
										 PAGE_READWRITE,/* ӳ�䵽�ڴ����ڴ��ҳ���� */
										 dwHigSize ,		/* ӳ��Ĵ�С�ĸ�32λ */
										 dwLowSize ,		/* ӳ���С�ĵ�32λ */
										 NULL	/* �ļ�ӳ������ȫ������ */
										 );
	if( hMapping == NULL ) {
		printf( "�����ļ�ӳ�����ʧ��\n" );
		CloseHandle( hFile );
		return 0;
	}

	// 3. ���ļ�ӳ�����ӳ�䵽�ڴ�.
	LPVOID	pBuff = NULL;
	pBuff = MapViewOfFile( hMapping ,	 /* �ļ�ӳ����� */
						   FILE_MAP_ALL_ACCESS,	/* Ȩ�� */
						   0 ,			 /* ӳ�䵽�ڴ���ļ�ƫ��(��32λ) */
						   0 ,			 /* ӳ�䵽�ڴ���ļ�ƫ��(��32λ) */
						   50			 /* ӳ�䵽�ڴ���ֽ��� */
						   );
	if( pBuff == NULL ) {
		printf( "���ļ�����ӳ�䵽�ڴ�ʱʧ��\n" );
		// �ر��ļ�ӳ��
		CloseHandle( hMapping );
		CloseHandle( hFile );
	}
	


	// �޸��ڴ�, ͬʱҲ��Ӱ���ļ�
	strcpy( (char*)pBuff , "Hello World\r\n" );
	
	FlushViewOfFile(pBuff, 20);
	// ȡ��ӳ��
	UnmapViewOfFile( pBuff );

	// �ر��ļ�ӳ��
	CloseHandle( hMapping );

	// �ر��ļ�
	CloseHandle( hFile );

	return 0;
}

