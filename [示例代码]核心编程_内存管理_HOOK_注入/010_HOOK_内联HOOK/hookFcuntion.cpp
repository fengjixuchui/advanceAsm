#include "stdafx.h"
#include "hookFunction.h"
#include <windows.h>
#include <map>
using std::map;

#pragma pack(1) // ���ýṹ���������Ϊ1�ֽڶ���
struct JMPCODE{
	BYTE	call;		// call $0
	DWORD	callOffset; // call ��ƫ��
};

#pragma pack(1)
struct FUNCTION 
{
	LPVOID		desFucntion; // Ŀ�꺯���ĵ�ַ(HOOK���ĺ���)
	LPVOID		srcFcuntion; // ԭ������ַ
	JMPCODE		backupData;	 // ���ݳ����ĺ�������
};

struct STACK {
	DWORD		entryAddress;
	DWORD		dwOldProtect ;
	DWORD		dwWrite ;
	FUNCTION*	pObj;
	void( _cdecl *fn )( );
	JMPCODE		jmpCode;
};


map<LPVOID , FUNCTION> g_hookMap;
typedef map<LPVOID , FUNCTION>::iterator MapItr;




FUNCTION* getFunctionInfo( LPVOID address ) {

	static FUNCTION obj;
	auto itr = g_hookMap.find( address );
	if( itr == g_hookMap.end( ) )
		return NULL;
	obj = itr->second;
	return &obj;
}





STACK g_Stack;

// �м亯��
void __declspec(naked) bridge_32( ) 
{
	_asm
	{
		mov eax , [ esp ];		// �õ�����ס�ĺ����ĵ�ַ
		add esp , 4;			// ƽ���ջ

		pushad;					// ����Ĵ���
		lea esi , g_Stack;		
		sub  eax , 5;			// call�ķ��ص�ַ-5 => ����ס�������׵�ַ
		mov [esi], eax;			// �����׵�ַ
	}

	// ���ݺ����׵�ַ�ҵ�������Ϣ.
	g_Stack.pObj = getFunctionInfo( (LPVOID)g_Stack.entryAddress );
	if( g_Stack.pObj == NULL )
		goto _EXIT;
	
	// �޸��ڴ��ҳ����
	VirtualProtectEx( GetCurrentProcess( ) ,
					  (LPVOID)g_Stack.entryAddress ,
					  sizeof(JMPCODE) ,
					  PAGE_EXECUTE_READWRITE ,
					  &g_Stack.dwOldProtect
					  );

	// ���浱ǰ��shellcode
	ReadProcessMemory( GetCurrentProcess( ) ,
					   (LPVOID)g_Stack.entryAddress ,
					   &g_Stack.jmpCode ,
					   sizeof( JMPCODE ) ,
					   &g_Stack.dwWrite
					   );



	// �ָ�����ԭ��������
	WriteProcessMemory( GetCurrentProcess( ) ,
						(LPVOID)g_Stack.entryAddress ,
						&g_Stack.pObj->backupData ,
						sizeof( JMPCODE ) ,
						&g_Stack.dwWrite
						);

	// ����ԭ�溯��
	g_Stack.fn = ( void( _cdecl * )( ) )g_Stack.pObj->desFucntion;
	
	_asm
	{
		;// ����ǰ, �Ƚ���ջ�ָ�,�����Ӻ����޷����ʵ�ԭ�����Ĳ���.
		popad;					// �ָ���ջ
		mov eax , g_Stack.fn;	// ��ȡ������ַ
		pop g_Stack.fn;			// �������ص�ַ
		call eax;				// ���ú���

		;// 
		push g_Stack.fn;		// �ѷ��ص�ַ�Ż�ȥ
		pushad					// ����Ĵ���
	}
	


	// ������ԭ�溯��֮��,�ٴ�HOOK�������
	WriteProcessMemory( GetCurrentProcess( ) ,
						(LPVOID)g_Stack.entryAddress ,
						&g_Stack.jmpCode ,
						sizeof( JMPCODE ) ,
						&g_Stack.dwWrite
						);

	//	6. �ָ��ڴ��ҳ������
	VirtualProtectEx( GetCurrentProcess( ) ,
					  (LPVOID)g_Stack.entryAddress ,
					  sizeof( JMPCODE ) ,
					  g_Stack.dwOldProtect ,
					  &g_Stack.dwOldProtect
					  );


_EXIT:
	_asm
	{
		popad;  //�ָ��Ĵ���
		ret;
	}
}


bool	hookFunction( void* targetFunction , void* pNewFcuntionAddress ) 
{

	/**
	* HOOK����
	* ����Ҫ��סס�ĸ�����, �����shellcode��ֻ��call��bridge_32����.
	* ��bridge_32������,���������ֶ����ҵ�����ס�����ĸ�����, Ȼ�����
	* ��֮��Ӧ�Ĺ��Ӻ���.
	* 
	* 1. ����shellcode
	*	 call bridge_32
	* 2. ������ס�ĺ���,�͹��Ӻ���, �Լ�����ס������ԭʼ�����ֽڱ���
	* 3. �޸ı���ס�������ڴ��ҳ����
	* 4. ���汻��ס�����Ĵ����ֽ�
	* 5. ��shellcodeд�뵽����ס�ĺ�����.
	* 6. �ָ���ҳ����
	* 7. ������ס�ĺ����͹��Ӻ�������Ϣ��������.
	*/

	// shellcode
	JMPCODE		shellcode = { 0xE8 , 0 } ;
	// ����call��ƫ��
	shellcode.callOffset= (DWORD)&bridge_32 - (DWORD)targetFunction - 5 ;

	// ������Ϣ�ṹ��
	FUNCTION	functionInfo = { 0 };
	functionInfo.desFucntion = pNewFcuntionAddress; // ���Ӻ���
	functionInfo.srcFcuntion = targetFunction;		// ����ס�ĺ���
	

	DWORD dwWrite = 0;
	DWORD dwOldProtect = 0;

	// �޸��ڴ��ҳ����
	VirtualProtectEx( GetCurrentProcess( ) ,
					  (LPVOID)targetFunction ,
					  sizeof( JMPCODE ) ,
					  PAGE_EXECUTE_READWRITE ,
					  &dwOldProtect
					  );

	// ���汻��ס�ĺ�����ԭʼ�����ֽ�
	ReadProcessMemory( GetCurrentProcess( ) ,
					   (LPVOID)targetFunction ,
					   &functionInfo.backupData ,
					   sizeof( JMPCODE ) ,
					   &dwWrite
					   );


	// ��shellcodeд�뵽����ס�ĺ�����
	WriteProcessMemory( GetCurrentProcess( ) ,
						(LPVOID)targetFunction ,
						&shellcode ,
						sizeof( JMPCODE ) ,
						&dwWrite
						);

	//	6. �ָ��ڴ��ҳ������
	VirtualProtectEx( GetCurrentProcess( ) ,
					  (LPVOID)targetFunction,
					  sizeof( JMPCODE ) ,
					  dwOldProtect ,
					  &dwOldProtect
					  );

	// ����Ϣ����.
	return g_hookMap.insert( 
		std::pair<LPVOID , FUNCTION>( targetFunction , functionInfo ) ).second;
}



// ж�ع���
bool	unhookFunction( void* targetFunction )
{
	// ���ݱ���ס�ĺ����ĵ�ַ�ҵ���Ϣ
	MapItr itr = g_hookMap.find( targetFunction );

	// ���û���ҵ�, ˵��û��HOOK
	if( itr == g_hookMap.end( ) )
		return false;

	
	DWORD dwOldProtect = 0;
	DWORD dwWrite = 0;
	// �޸��ڴ��ҳ����
	VirtualProtectEx( GetCurrentProcess( ) ,
					  targetFunction ,
					  sizeof( JMPCODE ) ,
					  PAGE_EXECUTE_READWRITE ,
					  &dwOldProtect
					  );

	// ��ԭ����������ԭʼ�����ֽ�
	WriteProcessMemory( GetCurrentProcess( ) ,
						targetFunction,
						&itr->second.backupData ,
						sizeof( JMPCODE ) ,
						&dwWrite
						);

	// �ָ��ڴ��ҳ����
	VirtualProtectEx( GetCurrentProcess( ) ,
					  targetFunction ,
					  sizeof( JMPCODE ) ,
					  dwOldProtect ,
					  &dwOldProtect
					  );

	// ɾ����Ϣ.
	g_hookMap.erase( itr );
	return true;
}



