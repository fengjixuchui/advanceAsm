// 006-02-�ڴ����_��ӳ�����.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <windows.h>

int main()
{
	// ���ļ�ӳ�����
	HANDLE hMap = 
		OpenFileMapping(GENERIC_READ | GENERIC_WRITE, 
						FALSE,
						L"Global\\15pb_wenjianyingshe");

	// ���ļ�ӳ�����ӳ�䵽���̵������ַ�ռ���
	LPBYTE p    = (LPBYTE)MapViewOfFile(hMap,
									 FILE_MAP_READ | FILE_MAP_WRITE,
									 0,
									 0,
									 4096);

	*(DWORD*)p  = 0xFFFFFFFF;
	
	UnmapViewOfFile(p);
	CloseHandle(hMap);
    return 0;
}

