// �ڴ����_���������ڴ�.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <Windows.h>
#include <list>
using std::list;

enum MEMORYSTATE {
	e_stat_free = MEM_FREE ,
	e_stat_reserve = MEM_RESERVE ,
	e_stat_commit = MEM_COMMIT
};

enum MEMORYTYPE {
	e_type_image = MEM_IMAGE ,
	e_type_mapped = MEM_MAPPED ,
	e_type_private = MEM_PRIVATE ,
};

typedef struct VMINFO {
	DWORD		address;
	DWORD		size;
	MEMORYSTATE state;
}VMINFO;




void queryVirtualMemoryStatue( HANDLE hProcess , list<VMINFO>* memoryStatue ) {

	MEMORY_BASIC_INFORMATION	mbi			= { 0 };
	VMINFO						statue		= { 0 };
	DWORD						dwAddress	= 0;
	DWORD						dwSize		= 0;
	BOOL						bRet		= FALSE;
	while( 1 ) {

		bRet = VirtualQueryEx( hProcess , 
							   (LPCVOID)dwAddress , 
							   &mbi , 
							   sizeof( MEMORY_BASIC_INFORMATION ) );
		if( bRet == FALSE )
			break;


		statue.address	= dwAddress;
		statue.state	= (MEMORYSTATE)mbi.State;
		dwSize			= mbi.RegionSize;
		
		// ����ڴ�״̬, 
		// �ڴ�״̬�������������ڴ���û�к�����洢�����й���.
		// ���Ƿ�Ԥ��.
		// free   : ����,û��Ԥ��,û�к�����洢������
		// reserve: ����,��Ԥ��,û�к�����洢������
		// commit : �ύ,�Ѿ�������洢������
		switch( statue.state ) {
			case e_stat_free:
				printf( "0x%08X : Free\n" , statue.address );
				break;
			case e_stat_reserve:
				printf( "0x%08X : reserve\n" , statue.address  );
				break;
			case e_stat_commit:
				printf( "0x%08X : commit\n" , statue.address );
				break;
		}

		// ����ڴ��ַ�Ѿ��ύ�������ڴ�,������ύ����ÿһ���ڴ��.
		if( statue.state == e_stat_commit ) {

			dwSize						= 0;
			LPVOID	dwAllocationBase	= mbi.AllocationBase;
			DWORD	dwBlockAddress		= (DWORD)dwAddress;
			while( 1 ) {

				bRet = VirtualQueryEx( hProcess ,
									   (LPCVOID)dwBlockAddress ,
									   &mbi ,
									   sizeof( MEMORY_BASIC_INFORMATION ) );
				if( bRet == FALSE ) {
					break;
				}


				// �жϱ����������ڴ���Ƿ���ͬһ��.(�����ǵķ�����׵�ַ�Ƿ����.)
				// �������,������ѭ��.
				if( mbi.AllocationBase != dwAllocationBase )
					break;

				printf( "\t0x%08X " , dwBlockAddress );

				// ����ڴ�����
				// �ڴ����ͱ�ʾ�����ڴ����Ժ��ַ�ʽ������洢�����й���
				// image  : �Ǵ�Ӱ���ļ���ӳ�����
				// mapped : �ڴ�ӳ��
				// private: ˽���ڴ�,���������޷�����.
				switch( mbi.Type ) {
					case e_type_image:
						printf( " ����: image   " );
						break;
					case e_type_mapped:
						printf( " ����: mapped  " );
						break;
					case e_type_private:
						printf( " ����: private " );
						break;
					default:
						break;
				}

				// ����ڴ��ҳ����
				// �ڴ��ҳ�������ڱ�ʾ�ڴ��ҳ�ܹ����к��ַ���,���,д,ִ��,дʱ����.
				if (mbi.Protect == 0)
					printf("---");
				else if (mbi.Protect & PAGE_EXECUTE)
					printf("E--");
				else if (mbi.Protect & PAGE_EXECUTE_READ)
					printf("ER-");
				else if (mbi.Protect & PAGE_EXECUTE_READWRITE)
					printf("ERW");
				else if (mbi.Protect & PAGE_READONLY)
					printf("-R-");
				else if (mbi.Protect & PAGE_READWRITE)
					printf("-RW");
				else if (mbi.Protect & PAGE_WRITECOPY)
					printf("WCOPY");
				else if (mbi.Protect & PAGE_EXECUTE_WRITECOPY)
					printf("EWCOPY");

				// ����ڴ��Ĵ�С.
				printf( " ��С:0x%X\n", mbi.RegionSize );

				// ��������һ���ڴ��
				dwBlockAddress +=  mbi.RegionSize;

				// �ۼ��ڴ��Ĵ�С
				dwSize += mbi.RegionSize;
			}
		}

		statue.size = dwSize;
		memoryStatue->push_back( statue );

		// ������һ�������ڴ�.
		dwAddress += dwSize;
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	list<VMINFO> vmList;

	queryVirtualMemoryStatue( GetCurrentProcess( ) , &vmList );

	return 0;
}

