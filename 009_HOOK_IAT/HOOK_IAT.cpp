// HOOK_IAT.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <windows.h>

bool	hookIat( const char* pszDllName , const char* pszFunction , LPVOID pNewFunction );


// �����MessageBox
DWORD WINAPI MyMessageBox( HWND hWnd , TCHAR* pText , TCHAR* pTitle , DWORD type ) {

	// ��ԭIAT
	hookIat( "User32.dll" , 
			 "MessageBoxW" , 
			 GetProcAddress(GetModuleHandleA("User32.dll"),"MessageBoxW" )
			 );

	// ����ԭ�溯��
	MessageBox( 0 , L"�ڵ����MessageBox�е����˿�" , L"��ʾ" , 0 );


	// HOOK IAT
	hookIat( "User32.dll" , "MessageBoxW" , &MyMessageBox );

	return 0;
}



int _tmain(int argc, _TCHAR* argv[])
{
	MessageBox( 0 , L"����API" , L"��ʾ" , 0 );
	// HOOK IAT
	hookIat( "User32.dll" , "MessageBoxW" , &MyMessageBox );

	MessageBox( 0 , L"����API" , L"��ʾ" , 0 );
	MessageBox( 0 , L"����API" , L"��ʾ" , 0 );
	return 0;
}


bool hookIat( const char* pszDllName ,
			  const char* pszFunction ,
			  LPVOID pNewFunction ) {

	// PE�ļ���,���е�API�ĵ�ַ�����浽�˵������.

	// �������һ��APIʱ, �Ȼ�ӵ�����еõ�API
	// �ĵ�ַ, �ٵ��������ַ.
	// �����������е�API��ַ�滻��, ��ô����
	// APIʱ, �ͻ���ñ��滻�ĵ�ַ.

	// HOOK IAT�Ĳ���:
	// 1. ����PE�ļ�,�ҵ������
	// 2. �ҵ�������ж�Ӧ��ģ��
	// 3. �ҵ���Ӧģ��Ķ�Ӧ����.
	// 4. �޸ĺ�����ַ.

	// ������������ű�, һ���ǵ������Ʊ�, һ���ǵ���
	// ��ַ��, �����ű��Ԫ��һһ��Ӧ��.
	// �������Ʊ��д�ŵ��Ǻ�����
	// �����ַ���д�ŵ��Ǻ�����ַ.

	HANDLE hProc = GetCurrentProcess( );
	
	PIMAGE_DOS_HEADER			pDosHeader; // Dosͷ
	PIMAGE_NT_HEADERS			pNtHeader;	// Ntͷ
	PIMAGE_IMPORT_DESCRIPTOR	pImpTable;	// �����
	PIMAGE_THUNK_DATA			pInt;		// ������еĵ������Ʊ�
	PIMAGE_THUNK_DATA			pIat;		// ������еĵ����ַ��
	DWORD						dwSize;
	DWORD						hModule;
	char*						pFunctionName;
	DWORD						dwOldProtect;

	hModule = ( DWORD)GetModuleHandle( NULL );

	// ��ȡdosͷ
	pDosHeader = (PIMAGE_DOS_HEADER)hModule;

	// ��ȡNtͷ
	pNtHeader = (PIMAGE_NT_HEADERS)( hModule + pDosHeader->e_lfanew );


	// ��ȡ�����
	pImpTable = ( PIMAGE_IMPORT_DESCRIPTOR )
		(hModule + pNtHeader->OptionalHeader.DataDirectory[1].VirtualAddress);

	// ���������
	while( pImpTable->FirstThunk != 0 && pImpTable->OriginalFirstThunk != 0 ) {


		// �ж��Ƿ��ҵ��˶�Ӧ��ģ����
		if( _stricmp( (char*)(pImpTable->Name+hModule) , pszDllName ) != 0 ) {
			++pImpTable;
			continue;
		}
		
		// �������Ʊ�,�ҵ�������
		pInt = (PIMAGE_THUNK_DATA)( pImpTable->OriginalFirstThunk + hModule );
		pIat = (PIMAGE_THUNK_DATA)( pImpTable->FirstThunk + hModule );

		while( pInt->u1.AddressOfData != 0 ) {

			// �ж��������Ƶ��뻹������Ҫ����
			if( pInt->u1.Ordinal & 0x80000000 == 1 ) {
				// ����ŵ���

				// �ж��Ƿ��ҵ��˶�Ӧ�ĺ������
				if( pInt->u1.Ordinal == ( (DWORD)pszFunction ) & 0xFFFF ) {
					// �ҵ�֮��,�����Ӻ����ĵ�ַд�뵽iat
					VirtualProtect( &pIat->u1.Function ,
									4 ,
									PAGE_READWRITE ,
									&dwOldProtect
									);

					pIat->u1.Function = (DWORD)pNewFunction;

					VirtualProtect( &pIat->u1.Function ,
									4 ,
									dwOldProtect ,
									&dwOldProtect
									);
					return true;
				}
			}
			else {
				// �����Ƶ���
				pFunctionName = (char*)( pInt->u1.Function + hModule + 2);

				// �ж��Ƿ��ҵ��˶�Ӧ�ĺ�����
				if( strcmp( pszFunction , pFunctionName ) == 0 ) {

					VirtualProtect( &pIat->u1.Function ,
									4 ,
									PAGE_READWRITE ,
									&dwOldProtect
									);

					// �ҵ�֮��,�����Ӻ����ĵ�ַд�뵽iat
					pIat->u1.Function = (DWORD)pNewFunction;

					VirtualProtect( &pIat->u1.Function ,
									4 ,
									dwOldProtect ,
									&dwOldProtect
									);

					return true;
				}
			}
			
			++pIat;
			++pInt;
		}


		++pImpTable;
	}

	return false;
	
}

