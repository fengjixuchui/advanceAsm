// 006-01-�ڴ����_�ļ�ӳ��.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <windows.h>

int main()
{
	HANDLE hMapping;
	// ����һ���ļ�ӳ�����,���Բ����κ��ļ����й���
	// ��Global�ռ��д��������������Ҫ�õ�����ԱȨ��
	hMapping = CreateFileMapping(INVALID_HANDLE_VALUE,/*�ļ����*/
								 NULL,
								 PAGE_READWRITE,
								 0,
								 4096,
								 L"Global\\15pb_wenjianyingshe");

	// ���ļ�ӳ�����ӳ�䵽���̵������ַ�ռ���
	LPBYTE pBuff =
		(LPBYTE)MapViewOfFile(hMapping,
							  FILE_MAP_READ | FILE_MAP_WRITE,
							  0, /*ӳ���ƫ�Ƹ�32λ*/
							  0,/*ӳ���ƫ�Ƶ�32λ*/
							  4096/*ӳ����ֽ���*/);

	UnmapViewOfFile(pBuff);
	CloseHandle(hMapping);

    return 0;
}

