// MonitorDll.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "MonitorDll.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/************************************************************************/
/*          �����Ķ����뺯������                                        */

// ��������
#pragma data_seg("Share")

HWND g_hwnd = NULL;			//	�����ھ��������HOOK��ʱ����
HINSTANCE hInstance = NULL;	//	��DLL��ʵ�����
HHOOK hhook = NULL;			//	��깳�Ӿ��

#pragma data_seg()
#pragma comment(linker,"/section:Share,rws")

//������������

#define WM_HOOKMSG WM_USER + 106	// �Լ��������Ϣ
HANDLE hProcess = NULL;		//	��ǰ���̾��
BOOL bIsInjected = FALSE;	//	��ֻ֤ע��һ��


typedef HANDLE ( WINAPI *TypeSetClipboardData )(_In_ UINT uFormat,_In_opt_ HANDLE hMem);//User32.dll
typedef HANDLE ( WINAPI *TypeGetClipboardData )(_In_ UINT uFormat);
typedef HRESULT (WINAPI *TypeOleSetClipboard)(_In_ LPDATAOBJECT pDataObj);		//Ole32.dll
typedef HRESULT (WINAPI *TypeOleGetClipboard)(_Out_ LPDATAOBJECT *ppDataObje); 

TypeSetClipboardData oldSetClipboardData = NULL;	// ����ԭ������ַ
TypeGetClipboardData oldGetClipboardData = NULL;
TypeOleSetClipboard  oldOleSetClipboard = NULL;
TypeOleGetClipboard  oldOleGetClipboard = NULL;

FARPROC pfOldSetClipboardData = NULL;	// ָ��ԭ������ַ��Զָ��
FARPROC pfOldGetClipboardData = NULL;
FARPROC pfOldOleSetClipboard = NULL;
FARPROC pfOldOleGetClipboard = NULL;

HANDLE WINAPI MySetClipboardData(_In_ UINT uFormat,_In_opt_ HANDLE hMem);	// �Լ��ĺ�������
HANDLE WINAPI MyGetClipboardData(_In_ UINT uFormat);
HRESULT WINAPI MyOleSetClipboard(_In_ LPDATAOBJECT pDataObj);
HRESULT WINAPI MyOleGetClipboard(_Out_ LPDATAOBJECT *ppDataObje);



#define CODE_LENGTH	5			//	���ָ���
BYTE oldCodeSet[CODE_LENGTH];	//	ԭ�������
BYTE newCodeSet[CODE_LENGTH];	//	�滻�������ָ��

BYTE oldCodeGet[CODE_LENGTH];
BYTE newCodeGet[CODE_LENGTH];

BYTE oldCodeSetOle[CODE_LENGTH];
BYTE newCodeSetOle[CODE_LENGTH];

BYTE oldCodeGetOle[CODE_LENGTH];
BYTE newCodeGetOle[CODE_LENGTH];


BOOL WINAPI HookLoad(HWND hwnd);	// ����dll hook ����
VOID WINAPI HookUnload();
VOID Inject();
VOID HookOn();
VOID HookOff();
BOOL AdjustPrivileges();	// ����Ȩ��
LRESULT CALLBACK MouseProc(		// ��깳���ӹ��̵���
	int nCode,	// hook code
	WPARAM wParam,// message identifier
	LPARAM lParam // mouse coordinates
	);
BOOL WriteMemory(LPVOID lpAddress,BYTE* pcode,size_t length); //������Ϊ length �� pcode д���ַ lpAddress �Ľ����ڴ���


/************************************************************************/


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

// CMonitorDllApp

BEGIN_MESSAGE_MAP(CMonitorDllApp, CWinApp)
END_MESSAGE_MAP()


// CMonitorDllApp ����

CMonitorDllApp::CMonitorDllApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CMonitorDllApp ����

CMonitorDllApp theApp;


// CMonitorDllApp ��ʼ��

BOOL CMonitorDllApp::InitInstance()
{
	CWinApp::InitInstance();

	hInstance = AfxGetInstanceHandle();

	AdjustPrivileges();
	DWORD dwPid = ::GetCurrentProcessId();
	hProcess = ::OpenProcess(PROCESS_ALL_ACCESS,0,dwPid);

	if (hProcess == NULL)
	{
		CString str;
		str.Format(_T("OpenProcess fail!!, error code = [%d]"),GetLastError());
		AfxMessageBox(str);
		return FALSE;
	}

	Inject();
	return TRUE;
}

//	�˳�ʱ��һ��Ҫ�ǵûָ�ԭ������ַ������

int CMonitorDllApp::ExitInstance()
{
	HookOff();	//Ҫ�ǵûָ�ԭ������ַ
	
	return CWinApp::ExitInstance();
}

/************************************************************************/
/*                      DLL��ز�������                                */

/*
	��깳���ӹ��̣�Ŀ���Ǽ��ر�dll��ʹ�����ĳ���.
	��깳�ӵ����ã��������ĳ���򴰿���ʱ���ͻ�����������dll
*/
LRESULT CALLBACK MouseProc(		
	int nCode,	// hook code
	WPARAM wParam,// message identifier
	LPARAM lParam // mouse coordinates
	)
{
	/*	if (nCode == HC_ACTION)
	{
		// ���������ڴ��ھ������������
		::SendMessage(g_hWnd,WM_HOOKMSG,wParam,(LPARAM)(((PMOUSEHOOKSTRUCT)lParam)->hwnd));
	}*/

	return CallNextHookEx(hhook,nCode,wParam,lParam);
}

/* 
	��װ����
*/
BOOL WINAPI HookLoad(HWND hwnd)
{
	BOOL ret = FALSE;

	g_hwnd = hwnd;
	hhook = ::SetWindowsHookEx(WH_MOUSE,MouseProc,hInstance,0);
	if (hhook == NULL)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

/*
	ж�ع���
	ע��ж�ع���֮ǰ��һ��Ҫ�ǵûָ�ԭ������ַ������
*/
VOID WINAPI HookUnload()
{
	HookOff();	// �ָ�ԭ������ַ
	if (hhook != NULL)
	{
		UnhookWindowsHookEx(hhook);
	}
	if (hInstance != NULL)
	{
		FreeLibrary(hInstance);
	}
}

/*
	ע�뺯����
	��Ҫ���ԭ������ַ�ı��棬���浽 oldCode_[]�У�
	����ڵ�ַ�ļ��㣬���浽newCode_[]�У��� jmp xxxx ָ�
	����ڵ�ַ = �º�����ַ - ԭ������ַ - ָ���
	���һ��Ҫ�ǵ�HookOn����
*/
VOID Inject()
{
	if (bIsInjected == TRUE)	
	{
		return;
	}
	bIsInjected = TRUE;// ��ֻ֤ע��һ��

	// ����User32.dll �� Ole32.dll

	HMODULE hmodleUser32,hmodleOle32;
	hmodleUser32 = ::LoadLibrary(_T("User32.dll"));
	if ( NULL == hmodleUser32)
	{
		TRACE("Inject :: ���� User32.dll ʧ��");
		return;
	}
	hmodleOle32 = ::LoadLibrary(_T("Ole32.dll"));
	if ( NULL == hmodleOle32)
	{
		AfxMessageBox(_T("Inject :: ����Ole32.dll ʧ��"));
		return;
	}

	//	��ȡԭ������ַ
	oldGetClipboardData = (TypeGetClipboardData)::GetProcAddress(hmodleUser32,"GetClipboardData");
	pfOldGetClipboardData = (FARPROC)oldGetClipboardData;
	if (NULL == pfOldGetClipboardData)
	{
		AfxMessageBox(_T("Inject :: ��ȡ���� GetClipboardData ʧ��"));
		return;
	}
	oldSetClipboardData = (TypeSetClipboardData)::GetProcAddress(hmodleUser32,"SetClipboardData");
	pfOldSetClipboardData = (FARPROC) oldSetClipboardData;
	if (NULL == pfOldGetClipboardData)
	{
		AfxMessageBox(_T("Inject :: ��ȡ���� SetClipboardData ʧ��"));
		return;
	}
	oldOleGetClipboard = (TypeOleGetClipboard)::GetProcAddress(hmodleOle32,"OleGetClipboard");
	pfOldOleGetClipboard = (FARPROC)oldOleGetClipboard;
	if (NULL == pfOldGetClipboardData)
	{
		AfxMessageBox(_T("Inject :: ��ȡ���� OleGetClipboard ʧ��"));
		return;
	}
	oldOleSetClipboard = (TypeOleSetClipboard)::GetProcAddress(hmodleOle32,"OleSetClipboard");
	pfOldOleSetClipboard = (FARPROC) oldOleSetClipboard;
	if (NULL == pfOldGetClipboardData)
	{
		AfxMessageBox(_T("Inject :: ��ȡ���� OleSetClipboard ʧ��"));
		return;
	}


	//
	//	����ԭ�������
	//
	_asm
	{
		lea edi, oldCodeGet
		mov esi,pfOldGetClipboardData
		cld
		mov ecx,CODE_LENGTH
		rep movsb
	}

	_asm
	{
		lea edi,oldCodeSet
		mov esi,pfOldSetClipboardData
		cld
		mov ecx,CODE_LENGTH
		rep movsb
	}

	_asm
	{
		lea edi,oldCodeGetOle
		mov esi,pfOldOleGetClipboard
		cld
		mov ecx,CODE_LENGTH
		rep movsb
	}

	_asm
	{
		lea edi,oldCodeSetOle
		mov esi,pfOldOleSetClipboard
		cld
		mov ecx,CODE_LENGTH
		rep movsb
	}

	//
	//	��ȡ����� : ����Ե�ַ = �º�����ַ - ԭ������ַ - ָ���
	//
	newCodeGet[0] = 0xe9;	//	jmp ָ��, 1 BYTE
	_asm
	{
		lea eax,MyGetClipboardData
		mov ebx,pfOldGetClipboardData
		sub eax,ebx
		sub eax,CODE_LENGTH
		mov dword ptr [newCodeGet+1],eax	// 4 BYTE
	}

	newCodeSet[0] = 0xe9;
	_asm
	{
		lea eax,MySetClipboardData
		mov ebx,pfOldSetClipboardData
		sub eax,ebx
		sub eax,CODE_LENGTH
		mov dword ptr [newCodeSet+1],eax
	}

	newCodeGetOle[0] = 0xe9;
	_asm
	{
		lea eax,MyOleGetClipboard
		mov ebx,pfOldOleGetClipboard
		sub eax,ebx
		sub eax,CODE_LENGTH
		mov dword ptr [newCodeGetOle+1],eax
	}

	newCodeSetOle[0] = 0xe9;
	_asm
	{
		lea eax,MyOleSetClipboard
		mov ebx,pfOldOleSetClipboard
		sub eax,ebx
		sub eax,CODE_LENGTH
		mov dword ptr [newCodeSetOle+1],eax
	}

	HookOn();	//�����ϣ���ʼHOOK
}
/*
	������Ϊ length �� pcode д���ַ lpAddress �Ľ����ڴ���
*/
BOOL WriteMemory(LPVOID lpAddress,BYTE* pcode,size_t length)
{
	ASSERT(hProcess != NULL);

	DWORD dwtemp,dwOldProtect,dwRet,dwWrited;

	dwRet = VirtualProtectEx(hProcess,lpAddress,length,PAGE_READWRITE,&dwOldProtect);
	CString logInfo;
	if ( 0 == dwRet)
	{
		logInfo.Format(_T("WriteMemory :: Call VirtualProtectEx fail, eror code = [%d]\n\n"),GetLastError());
		AfxMessageBox(logInfo);
		return FALSE;
	}
	dwRet = WriteProcessMemory(hProcess,lpAddress,pcode,length,&dwWrited);
	if ( 0 == dwRet || 0 == dwWrited)
	{
		logInfo.Format(_T("WriteMemory :: Call WriteProcessMomory fail, error code = [%d]\n\n"),GetLastError());
		AfxMessageBox(logInfo);
		return FALSE;
	}
	dwRet = VirtualProtectEx(hProcess,lpAddress,length,dwOldProtect,&dwtemp);
	if ( 0 == dwRet )
	{
		logInfo.Format(_T("WriteMemory :: Recover Protect fail, error code = [%d]\n\n"),GetLastError());
		AfxMessageBox(logInfo);
		return FALSE;
	}
	return TRUE;
}


/*
	��ʼHOOK��
	������Inject ��ʼ���õ���ڵ�ַ����д������ڴ��С�
	����� �º������ newCode_[]��д���ڴ��С�
	����һ������ԭ���������õ�ʱ�򣬾ͻ���ת�������º�����λ�á�
	
	ע: ���ﴦ��ĺ������ǵ�ǰ��Ҫ�滻�����к���������ֻ��Inject()�����е��ã�
	�����г�ʼ����ʱ���õ��ú�����
*/
VOID HookOn()
{
	
	BOOL ret;
	ret = WriteMemory(pfOldGetClipboardData,newCodeGet,CODE_LENGTH);
	if (FALSE == ret)
	{
		TRACE("HookOn ::��Fail to write pfOldGetClipboardData \n\n");
	}
	ret = WriteMemory(pfOldSetClipboardData,newCodeSet,CODE_LENGTH);
	if (FALSE == ret)
	{
		TRACE("HookOn :: Fail to wirte pfOldSetClipboardData \n\n");
	}
	ret = WriteMemory(pfOldOleGetClipboard,newCodeGetOle,CODE_LENGTH);
	if (FALSE == ret)
	{
		TRACE("HookOn :: Fail to wirte pfOldOleGetClipboard \n\n");
	}
	ret = WriteMemory(pfOldOleSetClipboard,newCodeSetOle,CODE_LENGTH);
	if (FALSE == ret)
	{
		TRACE("HookOn :: Fail to write pfOldOleSetClipboard \n\n");
	}
	
}

/*
	ֹͣHOOK��
	�ָ�ԭ������ַ��

	ע�����ﴦ����������滻�ĺ���������һ�������ֻ����ж��HOOK�����е���
*/
VOID HookOff()
{
	
	ASSERT(hProcess != NULL);

	
	BOOL ret;
	ret = WriteMemory(pfOldGetClipboardData,oldCodeGet,CODE_LENGTH);
	if (FALSE == ret)
	{
		TRACE("HookOff :: fail to recover pfOldGetClipboardData oldCode \n\n");
	}
	ret = WriteMemory(pfOldSetClipboardData,oldCodeSet,CODE_LENGTH);
	if ( FALSE == ret)
	{
		TRACE("Hookoff :: fail to recover pfOldSetClipboardData oldCode \n\n");
	}
	ret = WriteMemory(pfOldOleGetClipboard,oldCodeGetOle,CODE_LENGTH);
	if (FALSE == ret)
	{
		TRACE("HookOff :: fail to recover pfOldOleGetClipboard \n\n");
	}
	ret = WriteMemory(pfOldOleSetClipboard,oldCodeSetOle,CODE_LENGTH);
	if (FALSE == ret)
	{
		TRACE("HookOff :: fail to recover pfOldOleSetClipboard \n\n");
	}
}
/*
	����Ȩ��
*/
BOOL AdjustPrivileges()
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tp;
	TOKEN_PRIVILEGES oldtp;
	DWORD dwSize=sizeof(TOKEN_PRIVILEGES);
	LUID luid;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
		if (GetLastError()==ERROR_CALL_NOT_IMPLEMENTED) return true;
		else return false;
	}
	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid)) {
		CloseHandle(hToken);
		return false;
	}
	ZeroMemory(&tp, sizeof(tp));
	tp.PrivilegeCount=1;
	tp.Privileges[0].Luid=luid;
	tp.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;
	/* Adjust Token Privileges */
	if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), &oldtp, &dwSize)) {
		CloseHandle(hToken);
		return false;
	}
	// close handles
	CloseHandle(hToken);
	return true;
}

/************************************************************************/




/************************************************************************/
/*                            �滻����ʵ��                              */

CString getCurProcessName()
{
	TCHAR* procName = new TCHAR[MAX_PATH];
	GetModuleFileName(NULL,procName,MAX_PATH);  // �������ȡ�ý���������ô�Ϳ����ж��Ƿ��ǻ��ܽ���������
	CString info;
	info.Format(_T("%s"),procName);
	return info;
}

HANDLE WINAPI MyGetClipboardData(_In_ UINT uFormat)
{
	HANDLE handle = NULL;

	WriteMemory(pfOldGetClipboardData,oldCodeGet,CODE_LENGTH);//�ָ�ԭ������ַ

	CString processName = getCurProcessName();
	CString info;
	info.Format(_T("HOOK  GetClipboardData �� ^_^����ǰ���̣�[%s]"),processName);// ����ԭ����
	AfxMessageBox(info);

	handle = oldGetClipboardData(uFormat);
	WriteMemory(pfOldGetClipboardData,newCodeGet,CODE_LENGTH);//�滻ԭ����������HOOK
	return handle;
}

HANDLE WINAPI MySetClipboardData(_In_ UINT uFormat,_In_opt_ HANDLE hMem)
{
	HANDLE handle = NULL;

	WriteMemory(pfOldSetClipboardData,oldCodeSet,CODE_LENGTH);
	CString processName = getCurProcessName();
	CString info;
	info.Format(_T("HOOK  SetClipboardData �� ^_^����ǰ���̣�[%s]"),processName);
	AfxMessageBox(info);

	handle = oldSetClipboardData(uFormat,hMem);

	WriteMemory(pfOldSetClipboardData,newCodeSet,CODE_LENGTH);
	return handle;
}

HRESULT WINAPI MyOleGetClipboard(_Out_ LPDATAOBJECT *ppDataObje)
{
	HRESULT hresult = S_FALSE;

	HookOff();

	CString processName = getCurProcessName();
	CString info;
	info.Format(_T("HOOK  OleGetClipboard �� ^_^����ǰ���̣�[%s]"),processName);
	AfxMessageBox(info);

	hresult = OleGetClipboard(ppDataObje);

	HookOn();

	return hresult;
}

HRESULT WINAPI MyOleSetClipboard(_In_ LPDATAOBJECT pDataObj)
{
	HRESULT hresult = S_FALSE;

	HookOff();

	CString processName = getCurProcessName();
	CString info;
	info.Format(_T("HOOK  OleSetClipboard �� ^_^����ǰ���̣�[%s]"),processName);
	AfxMessageBox(info);

	hresult = OleSetClipboard(pDataObj);

	HookOn();

	return hresult;
}

/************************************************************************/


