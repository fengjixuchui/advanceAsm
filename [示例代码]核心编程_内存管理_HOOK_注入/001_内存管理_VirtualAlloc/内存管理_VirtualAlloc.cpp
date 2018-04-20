// �ڴ����_VirtualAlloc.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <windows.h>

int _tmain(int argc, _TCHAR* argv[])
{
	// �ڴ������
	//VirtualAlloc( );
	//HeapCreate(	 );
	//CreateFileMapping( );
	//MapViewOfFile( );
	//
	//malloc( );
	//new int;






	LPVOID pBuff = NULL;


	// Ԥ���ڴ�
	// Ԥ��֮��, �ڴ�״̬�ͱ��Ԥ��״̬(reserve), ����û�к������ڴ���й���
	// ��˲���������״̬���ڴ��н��ж�д����.
	// Ԥ��֮��, ��Ҫ�����ύ���ܶ��ڴ���ж�д.
	// Ҫ�ͷ�Ԥ�����ڴ�, ��Ҫʹ��VirtualFree,��ʹ��MEM_RELEASE��־
	// �������Ĵ�С����һ���ڴ��������(64Kb),����������ȡ����64Kb
	pBuff = VirtualAlloc( NULL ,		  /*������ָ����ַ��Ԥ���ڴ�.*/
						  1024*64*3  ,    /*Ԥ��3���������ȵ�λ���ڴ�*/
						  MEM_RESERVE ,   /*Ԥ���ڴ�ı�־,����*/
						  PAGE_NOACCESS /*�ڴ��ҳ����,��Ԥ��ʱ�޷������ڴ��ҳ����*/
						  );

	if( pBuff == NULL ) {
		printf( "Ԥ���ڴ�ʧ��\n" );
	}

	// ����д��ո�Ԥ�����ڴ�,
	__try {
		*(DWORD*)pBuff = 0;
	}
	__except( EXCEPTION_EXECUTE_HANDLER ) {
		printf( "Ԥ�����ڴ���뾭���ύ����ʹ��\n" );
	}

	// ��Ԥ�����ڴ��ύ�������ڴ�.
	// �ύ֮��, �ڴ�״̬�ʹ�Ԥ��״̬(reserve)����ύ״̬(commit)
	// �ύ֮��, Ԥ�����ڴ���ܹ��͹��������ڴ�, ���ܹ�������״̬�ڴ�
	// ���ж�д����(�������ڴ�֧�ֶ�/дȨ�޵Ļ�).
	// �����Ҫȡ���ύ, ����ʹ��VirtualFree����������MEM_DECOMMIT
	// ��ȡ���ύ.
	// Ӧ��ע�����, �ύ֮ʱ, �ύ���׵�ַ���������ڴ��ҳ���Ƚ��ж���
	// ��, �ڴ�Ĵ�С��ӦҲ���ڴ��ҳ���ȵı���. ����׵�ַ�����ڴ��ҳ
	// ���ȵı���, �����ڲ�������ȡ�����ڴ��ҳ���ȵı���, 
	// ����ڴ��ַ����һ���ڴ��ҳ, �����ڲ����Զ�����ȡ�����ڴ��ҳ����
	// �ı���.
	// �ڴ��ҳ����һ����4096Kb
	pBuff = VirtualAlloc( pBuff , /* Ҫ�������ڴ���й����������ַ. */
						  4096 ,  /* ��Ҫ�������ֽ��� */
						  MEM_COMMIT , /* �ύ�ĵ������ڴ�ı�־,����Ϊ�ύ */
						  PAGE_READWRITE  /*�����ڴ��ҳ����:�ɶ���д*/
						  );

	// ����д��ո�����������ڴ�
	*(DWORD*)pBuff  = 0;

	pBuff = VirtualAlloc((LPVOID)((DWORD)pBuff+4096), /* Ҫ�������ڴ���й����������ַ. */
						 4096,  /* ��Ҫ�������ֽ��� */
						 MEM_COMMIT, /* �ύ�ĵ������ڴ�ı�־,����Ϊ�ύ */
						 PAGE_READWRITE  /*�����ڴ��ҳ����:�ɶ���д*/
						);

	*(DWORD*)pBuff = 0;

	// ���������ύͬʱ����.
	LPVOID pBuff2;
	pBuff2 = VirtualAlloc(NULL,
						  64 * 1024,
						  MEM_RESERVE | MEM_COMMIT,
						  PAGE_READWRITE
						  );


	// ȡ���ύ�������ڴ�������ڴ�.
	// ȡ���ύ��, �����ٴ�ʹ��VirtualAlloc�ٴ��ύ
	VirtualFree( pBuff ,  /*��ʼ��ַ*/
				 4096 ,   /*ȡ���ύ���ֽ���*/
				 MEM_DECOMMIT  /*ȡ���ύ�ı�־*/
				 );


	// �ͷ�Ԥ�����ڴ�
	// �ͷ�ʱ, �ͷŵĵ�ַ�����ǵ���VirtualAllocԤ���ڴ������ص��ڴ��׵�ַ
	// ��С������0
	// �ͷ�֮��, �ڴ�״̬�ͱ�����Ϊ����״̬(free)
	VirtualFree( pBuff , 
				 0 , 
				 MEM_RELEASE );

	// ��ѯһ����ַ���ڴ�״̬
	MEMORY_BASIC_INFORMATION mbi = { 0 };
	/*
	typedef struct _MEMORY_BASIC_INFORMATION {
		PVOID BaseAddress;		// Ҫ��ѯ�ĵ�ַ
		PVOID AllocationBase;  //��ַ�������ڴ����׵�ַ
		DWORD AllocationProtect;// ������ڴ��ҳ����
		SIZE_T RegionSize;     // ��������Ĵ�С
		DWORD State;		  // �ڴ��״��(����,����,�ύ)
		DWORD Protect;		  // �ڴ������ڴ��ҳ����
		DWORD Type;			 // ӳ�䷽ʽ(private,image,mapped)
	} MEMORY_BASIC_INFORMATION
	*/
	HMODULE hMod = GetModuleHandle(NULL);
	VirtualQuery((LPVOID)((DWORD)hMod + 500) ,/*��Ҫ��ѯ�ĵ�ַ*/
				 &mbi, /*���ղ�ѯ����Ľṹ��*/
				 sizeof(mbi)/*�ṹ����ֽ���*/
				 );

	VirtualQuery((LPVOID)0x401000,
				 &mbi, /*���ղ�ѯ����Ľṹ��*/
				 sizeof(mbi)/*�ṹ����ֽ���*/);

	if (mbi.State == MEM_FREE)// MEM_FREE,MEM_COMMIT,MEM_RESERVE
	{
		VirtualAlloc((LPVOID)0x401000, 64 * 1024, MEM_RESERVE | MEM_COMMIT, 0);
	}
	

	return 0;
}

