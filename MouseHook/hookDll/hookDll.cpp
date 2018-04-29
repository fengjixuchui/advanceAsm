// hookDll.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "hookDll.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_HOOKMSG WM_USER + 106	// �Զ�����Ϣ

//
// �������Σ������̹߳���
//
#pragma data_seg("SHARED")
static HHOOK hhkMouse = NULL;			// ��깳�Ӿ��
static HINSTANCE g_hInstance = NULL;	// ��DLL��ʵ�����
static HWND g_hWnd = NULL;				// ����DLL�������ھ��
#pragma data_seg()
#pragma comment(linker,"/section:SHARED,rws")

//
//TODO: ����� DLL ����� MFC DLL �Ƕ�̬���ӵģ�
//		��Ӵ� DLL �������κε���
//		MFC �ĺ������뽫 AFX_MANAGE_STATE ����ӵ�
//		�ú�������ǰ�档
//
//		����:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// �˴�Ϊ��ͨ������
//		}
//
//		�˺������κ� MFC ����
//		������ÿ��������ʮ����Ҫ������ζ��
//		��������Ϊ�����еĵ�һ�����
//		���֣������������ж������������
//		������Ϊ���ǵĹ��캯���������� MFC
//		DLL ���á�
//
//		�й�������ϸ��Ϣ��
//		����� MFC ����˵�� 33 �� 58��
//

// ChookDllApp

BEGIN_MESSAGE_MAP(ChookDllApp, CWinApp)
END_MESSAGE_MAP()


// ChookDllApp ����

ChookDllApp::ChookDllApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� ChookDllApp ����

ChookDllApp theApp;


// ChookDllApp ��ʼ��

BOOL ChookDllApp::InitInstance()
{
	CWinApp::InitInstance();

	//
	// ��ȡ�����dll���
	//
	g_hInstance = ::AfxGetInstanceHandle();

	return TRUE;
}

//
// ����ͼ�����Ӻ���
//
LRESULT CALLBACK LowLevelMouseProc(int nCode,WPARAM wParam,LPARAM lParam)
{
	// �������Ϣʱ�����䷢��������
	if ( g_hWnd != NULL && nCode == HC_ACTION)
	{
		::SendMessage(g_hWnd,WM_HOOKMSG,wParam,lParam);
	}

	return CallNextHookEx(hhkMouse,nCode,wParam,lParam);

}
//
// ��װ�ͼ�����Ӻ������Ӷ��ػ�ϵͳ���е������Ϣ
//
BOOL WINAPI StartHookMouse(HWND hwnd)
{
	g_hWnd = hwnd;
	hhkMouse = SetWindowsHookEx(WH_MOUSE_LL,LowLevelMouseProc,g_hInstance,0);
	if ( NULL == hhkMouse)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}

}

//
// ж�صͼ���깳��
//
VOID WINAPI StopHookMouse()
{
	if (hhkMouse != NULL)
	{
		::UnhookWindowsHookEx(hhkMouse);
	}
}