// �ڴ����_��.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <Windows.h>

class Tank {
	
private:

	static HANDLE	m_hHeap;				// �ѵľ��
	static int		m_nHeapReferenceCount;  // �ڶ��ϴ�������ĸ���

private:
	int m_nX;
	int m_nY;
	int m_nDirection;
	int m_nId;

public:

	// ����new�����
	void* operator new( size_t nSize ){

		// ������
		if( m_nHeapReferenceCount == 0 ) {
			// ������.
			m_hHeap = HeapCreate( 0 , 0 , 0 );
		}

		// ���Ӷ��еĶ������.
		++m_nHeapReferenceCount;

		// �ڶ��з���һ���ڴ����������.
		return HeapAlloc( m_hHeap , 0 , sizeof( Tank ) );
	}

	// ����delete�����
	void operator delete( void* pObj ) {

		if( pObj == NULL )
			return ;

		// �ڶ����ͷŶ���Ŀռ�.
		HeapFree( m_hHeap , 0 , pObj );

		// ����һ�����ж������.
		--m_nHeapReferenceCount;

		// �������û�ж�����, ���ٶ�.
		if( m_nHeapReferenceCount <= 0 ) {

			// ���ٶ�.
			HeapDestroy( m_hHeap );
			m_hHeap = NULL;
		}
	}
};


HANDLE	Tank::m_hHeap;				  // �ѵľ��
int		Tank::m_nHeapReferenceCount;  // �ڶ��ϴ�������ĸ���


int _tmain(int argc, _TCHAR* argv[])
{
	 //HeapCreate HeapAlloc(new), HeapFree(delete);
	//HANDLE hHeap = HeapCreate(0, 0, 0);
	//int* pArray = (int*)HeapAlloc(hHeap,
	//							  0, 
	//							  10 * sizeof(int)
	//							);
	HANDLE	hHeap = NULL;
	Tank*	pTank = NULL;


	// ����һ����
	hHeap = HeapCreate( 0 , /* �����ѵı�־, ��ʹ���κ������־ */
						0 , /* �ѵĳ�ʼ��С, �����0,��Ĭ��Ϊһ���ڴ��ҳ�Ĵ�С */
						0   /* �ѵ�����С, �����0,��ѵĴ�С�����Զ�����.*/
						);
	if( hHeap == NULL ) {
		printf( "������ʧ��\n" );
		return 0;
	}

	// �Ӷ��з����ڴ�, ����ͬʹ��new �� mallocһ��.
	pTank = (Tank*)HeapAlloc( hHeap ,			  /*�ѵľ��*/
							  HEAP_ZERO_MEMORY , /*����ʱ���ڴ��ʼ��Ϊ0*/
							  sizeof( Tank )     /*����Ĵ�С*/
							  );

	if( pTank == NULL ) {
		printf( "�ڶ��������ڴ�ʧ��\n" );
	}

	HeapFree( hHeap , 0 , pTank );


	// ʹ�����ص�new �� delete
	Tank* pTank1= new Tank( );
	delete pTank1;
	return 0;
}

