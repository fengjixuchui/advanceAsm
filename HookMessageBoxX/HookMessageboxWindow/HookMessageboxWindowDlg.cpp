
// HookMessageboxWindowDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "HookMessageboxWindow.h"
#include "HookMessageboxWindowDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CHookMessageboxWindowDlg �Ի���




CHookMessageboxWindowDlg::CHookMessageboxWindowDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CHookMessageboxWindowDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHookMessageboxWindowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_STATIC_STATUS,m_status);
}

BEGIN_MESSAGE_MAP(CHookMessageboxWindowDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_START, &CHookMessageboxWindowDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CHookMessageboxWindowDlg::OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_CALL, &CHookMessageboxWindowDlg::OnBnClickedButtonCall)
END_MESSAGE_MAP()


// CHookMessageboxWindowDlg ��Ϣ�������

BOOL CHookMessageboxWindowDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CHookMessageboxWindowDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CHookMessageboxWindowDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CHookMessageboxWindowDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
//	����API����
#define CODE_LENGTH 5				//	���ָ���
typedef int (WINAPI *TypeMessageBoxW)(HWND hwnd,LPCWSTR lpText,LPCWSTR lpCaption,UINT uType);
TypeMessageBoxW OdlMsgBoxW = NULL;	// ָ����ԭ��ָ��
FARPROC pfOldMsgBoxW;				// ָ����Զָ��
BYTE OldCode[CODE_LENGTH];			// ԭϵͳAPI���
BYTE NewCode[CODE_LENGTH];			// �Լ�ʵ�ֵ�API����ڣ���jmp xxxx����xxxxΪ��API��ڵ�ַ

HANDLE hProcess = NULL;				// ��������̾��
HINSTANCE hInst = NULL;				// API���ڵ�dll�ļ����

VOID HookOn();			//	��ʼHOOK
VOID HookOff();			//	ֹͣHOOK
VOID GetApiEntrancy();	//	��ȡAPI��ڵ�ַ
bool AdjustPrivileges();//	���Ȩ��

//
// �Լ�����ģ������滻��ӦAPI�ģ��ٵ�API
//
int WINAPI MyMessageBoxW(HWND hwnd,LPCWSTR lpText,LPCWSTR lpCation,UINT uType)
{
	TRACE(lpText);
	
	/*
		����ԭ����֮ǰ����ֹͣHOOK��Ҳ���ǻָ�ԭϵͳAPI��������ڣ�
		�����޷����õ�ԭAPI���������Ǽ��������Լ���API���������
		ѭ����������ɶ�ջ�����������
	*/
	HookOff();	

	/*
		����ԭ����MessageBoxW��ӡ���ǵ���Ϣ��
	*/
	int ret = MessageBoxW(hwnd,_T("��������HOOK������"),lpCation,uType);

	/*
		������ԭϵͳAPI�󣬼ǵûָ�HOOK��Ҳ��������HOOK����ԭAPI������ڻ��������Լ�����ĺ�����ڣ�
		������һ�ε���MessageBoxW��ʱ����޷�ת�������Լ������API�����У�Ҳ���޷�ʵ��HOOK��
	*/
	HookOff();

	return ret;
}


/*
	����HOOK����ԭAPI����ڵ�ַ���������Լ����庯������ڵ�ַ
*/
VOID HookOn()
{
	//
	//	ȷ����������̾��hProcess��ΪNULL
	//
	ASSERT(hProcess!=NULL);

	DWORD dwTemp;
	DWORD dwOldProtect;
	SIZE_T writedByte;

	//
	// �޸�API��ڵ�ǰ5���ֽڣ�jmp xxxx
	//
	VirtualProtectEx(hProcess,pfOldMsgBoxW,CODE_LENGTH,PAGE_READWRITE,&dwOldProtect);
	WriteProcessMemory(hProcess,pfOldMsgBoxW,NewCode,CODE_LENGTH,&writedByte);
	if (writedByte == 0)
	{
		AfxMessageBox(_T("�滻ԭAPI��ַʧ��"));
	}
	VirtualProtectEx(hProcess,pfOldMsgBoxW,CODE_LENGTH,dwOldProtect,&dwTemp);
}

/*
	����HOOK������ڻ���ԭ����API��ڵ�ַ
*/
VOID HookOff()
{
	ASSERT(hProcess != NULL);

	DWORD dwTemp;
	DWORD dwOldProtect;

	SIZE_T wirtedByte;

	//	�ظ�ԭAPI��ַ
	VirtualProtectEx(hProcess,pfOldMsgBoxW,CODE_LENGTH,PAGE_READWRITE,&dwOldProtect);
	WriteProcessMemory(hProcess,pfOldMsgBoxW,OldCode,CODE_LENGTH,&wirtedByte);
	if (wirtedByte == 0)
	{
		AfxMessageBox(_T("�ظ�ԭAPI��ַʧ��"));
	}
	VirtualProtectEx(hProcess,pfOldMsgBoxW,CODE_LENGTH,dwOldProtect,&dwTemp);
}

/*
	����ԭAPI����API�ĵ�ַ
*/
VOID GetApiEntrancy()
{
	//
	// ����ԭ��API��ַ
	//
	HMODULE hmod = LoadLibrary(_T("User32.dll"));
	if ( NULL == hmod)
	{
		AfxMessageBox(_T("����User32.dllʧ��"));
		return;
	}
	OdlMsgBoxW = (TypeMessageBoxW)::GetProcAddress(hmod,"MessageBoxW");
	pfOldMsgBoxW = (FARPROC)OdlMsgBoxW;
	if ( pfOldMsgBoxW == NULL)
	{
		AfxMessageBox(_T("��ȡԭAPI��ڵ�ַ����"));
		return;
	}

	//
	//	��ԭAPI�����5���ֽڴ��뱣�浽OdeCode[]��
	//
	_asm
	{
			lea edi,OldCode			// ȡ����OldCode[]��ַ����ŵ�edi��
			mov esi,pfOldMsgBoxW	// ��ȡԭAPI��ڵ�ַ������esi��
			cld						// ���÷���
			movsd					// �ƶ�dword ��4 Byte
			movsb					// �ƶ� 1 Byte
	}

	//
	// �µ�API��ڱ��浽NewCode[]�У���jmp xxxx��xxxxΪ��API��ַ����ָ���ܳ���Ϊ5���ֽ�
	//
	NewCode[0] = 0xe9; //	0xe9�൱��jmpָ��

	_asm
	{
		lea eax,MyMessageBoxW
		mov ebx,pfOldMsgBoxW
		sub eax,ebx
		sub eax,CODE_LENGTH
		mov dword ptr[NewCode+1],eax
	}

	//
	// �����ϣ���ʼHOOK����ʹ��NewCode[]�滻ԭAPI���
	//
	HookOn();
}

bool AdjustPrivileges() {
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

//
//	���� HookMessageBoxW
//
void CHookMessageboxWindowDlg::OnBnClickedButtonStart()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	AdjustPrivileges();		//	����Ȩ�ޣ���Ϊ���� OpenProcess() ��Ҫ���ʵ�Ȩ��
	DWORD dwPid = ::GetCurrentProcessId();
	hProcess = OpenProcess(PROCESS_ALL_ACCESS,0,dwPid);
	if (hProcess == NULL)
	{
		CString logInfo;
		logInfo.Format(_T("��ȡ���̾��ʧ�ܣ���,���� id = 0x%x ,������� = 0x%x"),dwPid,GetLastError());
		AfxMessageBox(logInfo);
		return;
	}

	
	GetApiEntrancy();	//	��ȡ�¾�API��ڣ�����ʼHOOK

	m_status.SetWindowText(_T("Hook������"));
}

//
//	��ֹ HookMessageBoxW
//
void CHookMessageboxWindowDlg::OnBnClickedButtonStop()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	HookOff();
	m_status.SetWindowText(_T("Hook��ֹͣ"));
}

//
//	���� HookMessageBoxW
//
void CHookMessageboxWindowDlg::OnBnClickedButtonCall()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	::MessageBoxW(m_hWnd,_T("����������MessageBoxW"),_T("Hello"),0);
}
