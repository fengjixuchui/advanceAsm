// HOOK_����HOOK.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <windows.h>

BYTE	g_jmpShellCode[5] = { "\xe9" };
BYTE	g_oldOpcode[ 5 ] = { 0 };

#include "hookFunction.h"


// �����MessageBox
DWORD WINAPI MyMessageBox( HWND hWnd , TCHAR* pText , TCHAR* pTitle , DWORD type ) 
{

	DWORD dwOldProtect = 0;
	DWORD dwWrite = 0;
	VirtualProtectEx( GetCurrentProcess( ) ,
					  &MessageBox ,
					  4 ,
					  PAGE_EXECUTE_READWRITE ,
					  &dwOldProtect
					  );


	// �ָ�����ԭ��������
	WriteProcessMemory( GetCurrentProcess( ) ,
						&MessageBox ,
						g_oldOpcode ,
						sizeof( g_jmpShellCode ) ,
						&dwWrite
						);

	// ����ԭ�溯��
	MessageBox( 0 , L"�ڵ����MessageBox�е����˿�" , L"��ʾ" , 0 );



	// ������ԭ�溯��֮��,�ٴ�HOOK�������
	WriteProcessMemory( GetCurrentProcess( ) ,
						&MessageBox ,
						g_jmpShellCode ,
						sizeof( g_jmpShellCode ) ,
						&dwWrite
						);

	//	6. �ָ��ڴ��ҳ������
	VirtualProtectEx( GetCurrentProcess( ) ,
					  &MessageBox ,
					  4 ,
					  dwOldProtect ,
					  &dwOldProtect
					  );

	return 0;
}

// �����MessageBox
DWORD WINAPI MyMessageBox2( HWND hWnd , TCHAR* pText , TCHAR* pTitle , DWORD type ) {
	//MessageBox( hWnd , pText , pTitle , type );
	MessageBox( hWnd , L"--------------", pTitle , type );
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{

	// ����HOOK��ʾ��:
	// δ�����صĺ���
	// MessageBox:
	//		+--------------+
	// ==>  | mov edi,edi  |
	//		| push ebp     |
	//		| mov ebp,esp  |
	//		|              |
	// 		|  XXXXXXXXX   |
	//		+--------------+
	// 
	// �����صĺ���	
	//										function
	//	MessageBox:				  +--->>-------------------+			
	//		+--------------+	  |	   |	push ebp	   |
	// ==>  | jmp function>>------+    |   mov ebp,esp     |
	//		| push ebp     |		   |                   |
	//		| mov ebp,esp  |		   |     XXXXXX        |
	//		|              |		   |     XXXXXX        |
	// 		|  XXXXXXXXX   |		   |     XXXXXX        |
	//		+--------------+		   |     XXXXXX        |
	//								   | jmp  MessageBox+5 |
	//								   +-------------------+
	//
	// ��νHOOK, ��������, HOOK API������API���������뱻ִ��ǰ������
	// ��ת����һ���ط�ִ�д���, ִ�к�����ת��ȥ,����ת.
	// HOOK�ĸ���ȼ�, ��һЩϸ����Ҫע��
	// ����:
	// 1. ԭ������û����ת��XXX��ַ�Ĵ����,jmp XXX������ͨ���޸��ڴ�
	//	  �����ȥ��, ���޸��ڴ�ʱ,��Ҫע�����,һ���ܹ���ִ�е��ڴ��ҳ
	//    ��û�п�д������,���,���޸��ڴ�ǰ,��Ҫ�޸��ڴ��ҳ����.
	// 2. jmp ָ����Ҫ�õ�һ����תƫ��, ����һ�����Եĵ�ַ. ��jmp XXX��
	//    opcodeд�뵽�ڴ�ʱ, ����֪����ô����������תƫ��. һ����ת
	//    ƫ����Ҫ�õ����¹�ʽ������:
	//		��תƫ�� = Ŀ���ַ - ��ǰ��תָ�����ڵ�ַ - ��תָ������峤��
	//	  ��:  
	//		ָ��Ҫ��ת�� 011AB61F
	//		jmp 011AB61F ������תָ�����ڵ�ַΪ 011AB618
	//		jmp 011AB61F ����ָ������峤��Ϊ2, ��Ϊ����opcode��EB 05,ֻ��2���ֽ�
	//		��������ƫ��Ϊ: 011AB61F - 011AB61F - 2  => 5
	//		   
	//	+-<011AB618 | EB 05 | JMP 011AB61F 
	//	|  011AB61A | 31 C0 | XOR EAX , EAX 
	//	|  011AB61C | 31 DB | XOR EBX , EBX 
	//	|  011AB61E | 40    | INC EAX 
	//	+> 011AB61F | 43    | INC EBX 


	// ����HOOK����:
	//	1. ����Ҫ�޸ĵ��ڴ�ķ�ҳ����Ϊ��д
	//  2. ׼����ת��shellcode
	//  3. ������תƫ��,��������õ���תƫ��д�뵽shellcode��
	//  4. ��������ʼ��ַ����opcode����,�ֽ�����shellcode�ȳ�.
	//	5. ��opcodeд�뵽ҪHOOK�ĺ���
	//	6. �ָ��ڴ��ҳ������

	// 1. ����Ҫ�޸ĵ��ڴ�ķ�ҳ����Ϊ��д
	DWORD dwOldProtect = 0;
	VirtualProtectEx( GetCurrentProcess( ) ,
					  &MessageBox ,
					  4 ,
					  PAGE_EXECUTE_READWRITE ,
					  &dwOldProtect
					  );

	// 2. ׼����ת��shellcode
	//	BYTE	g_jmpShellCode[5] = { "\xe9" };


	// 3. ������תƫ��,��������õ���תƫ��д�뵽shellcode��
	DWORD dwJmpOffset = (DWORD)&MyMessageBox - (DWORD)&MessageBoxW - 5;
	*(DWORD*)( g_jmpShellCode + 1 ) = dwJmpOffset;



	// 4. ��������ʼ��ַ����opcode����,�ֽ�����shellcode�ȳ�.
	//    BYTE	g_oldOpcode[ 5 ] = { 0 };
	DWORD	dwRead = 0;
	ReadProcessMemory( GetCurrentProcess( ) ,
					   &MessageBox ,
					   g_oldOpcode ,
					   sizeof( g_jmpShellCode ) ,
					   &dwRead
					   );


	//	5. ��opcodeд�뵽ҪHOOK�ĺ���
	WriteProcessMemory( GetCurrentProcess( ) ,
						&MessageBox ,
						g_jmpShellCode ,
						sizeof( g_jmpShellCode ) ,
						&dwRead
						);

	//	6. �ָ��ڴ��ҳ������
	VirtualProtectEx( GetCurrentProcess( ) ,
					  &MessageBox ,
					  4 ,
					  dwOldProtect ,
					  &dwOldProtect
					  );


	// ���������ú�,���ܵ����Ӻ�����ִ�д���
	MessageBox( NULL , L"��������MessageBox" , L"��ʾ" , 0 );
	return 0;
}



