
// MonitorDllWindowDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MonitorDllWindow.h"
#include "MonitorDllWindowDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define  WM_HOOK_LOG WM_USER + 106	// �Լ��������Ϣ����


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


// CMonitorDllWindowDlg �Ի���




CMonitorDllWindowDlg::CMonitorDllWindowDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMonitorDllWindowDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMonitorDllWindowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_LIST_LOG,m_hookList);
}

BEGIN_MESSAGE_MAP(CMonitorDllWindowDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_LOAD, &CMonitorDllWindowDlg::OnBnClickedButtonLoad)
	ON_BN_CLICKED(IDC_BUTTON_UNLOAD, &CMonitorDllWindowDlg::OnBnClickedButtonUnload)

	ON_MESSAGE(WM_HOOK_LOG,&CMonitorDllWindowDlg::OnLogHook)	// ��Ϣӳ��

	ON_BN_CLICKED(IDC_BUTTON_MSGA, &CMonitorDllWindowDlg::OnBnClickedButtonMsga)
	ON_BN_CLICKED(IDC_BUTTON_MSGW, &CMonitorDllWindowDlg::OnBnClickedButtonMsgw)
END_MESSAGE_MAP()


// CMonitorDllWindowDlg ��Ϣ�������

BOOL CMonitorDllWindowDlg::OnInitDialog()
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

	// ��ʼ�� log list
	m_hookList.InsertColumn(0,_T("HOOK��Ϣ"));
	m_hookList.SetColumnWidth(0,250);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CMonitorDllWindowDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMonitorDllWindowDlg::OnPaint()
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
HCURSOR CMonitorDllWindowDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


/*
	��Ϣ������
*/

LRESULT CMonitorDllWindowDlg::OnLogHook(WPARAM wParam,LPARAM lParam)
{

	return 1;
}

HINSTANCE g_hinsDll = NULL;

/*
	����HOOK
*/
void CMonitorDllWindowDlg::OnBnClickedButtonLoad()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	g_hinsDll = LoadLibrary(_T("MonitorDll.dll"));
	CString logInfo;
	if ( NULL == g_hinsDll )
	{
		logInfo.Format(_T("���� MonitorDll.dll ʧ�ܣ�������룺%d"),GetLastError());
		m_hookList.InsertItem(m_hookList.GetItemCount(),logInfo);
		m_hookList.EnsureVisible(m_hookList.GetItemCount()-1,FALSE);
		return;
	}

	typedef BOOL (WINAPI* HookLoad)(HWND hwnd);
	HookLoad hookLoad = NULL;
	hookLoad = (HookLoad)::GetProcAddress(g_hinsDll,"HookLoad");
	if ( NULL == hookLoad)
	{
		m_hookList.InsertItem(m_hookList.GetItemCount(),_T("��ȡ���� HookLoad ʧ��"));
		m_hookList.EnsureVisible(m_hookList.GetItemCount()-1,FALSE);
		return;
	}

	BOOL ret =FALSE;
	ret = hookLoad(m_hWnd);
	if ( FALSE == ret )
	{
		m_hookList.InsertItem(m_hookList.GetItemCount(),_T("����HOOKʧ��"));
		m_hookList.EnsureVisible(m_hookList.GetItemCount()-1,FALSE);
	}
	else
	{
		m_hookList.InsertItem(m_hookList.GetItemCount(),_T("����HOOK�ɹ�"));
		m_hookList.EnsureVisible(m_hookList.GetItemCount()-1,FALSE);
	}

}

/*
	ж��HOOK
*/
void CMonitorDllWindowDlg::OnBnClickedButtonUnload()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (g_hinsDll != NULL)
	{
		g_hinsDll = LoadLibrary(_T("MonitorDll.dll"));
		if(NULL == g_hinsDll)
		{
			m_hookList.InsertItem(m_hookList.GetItemCount(),_T("���� MonitorDll.dll ʧ��"));
			m_hookList.EnsureVisible(m_hookList.GetItemCount()-1,FALSE);
			return;
		}
	}

	typedef VOID (WINAPI* HookUnload)();
	HookUnload hookUnload = NULL;
	hookUnload = (HookUnload) ::GetProcAddress(g_hinsDll,"HookUnload");
	if (NULL == hookUnload)
	{
		m_hookList.InsertItem(m_hookList.GetItemCount(),_T("��ȡ���� HookUnload ʧ��"));
		m_hookList.EnsureVisible(m_hookList.GetItemCount()-1,FALSE);
		return;
	}

	hookUnload();
	FreeLibrary(g_hinsDll);

	m_hookList.InsertItem(m_hookList.GetItemCount(),_T("ж��HOOK�ɹ�"));
	m_hookList.EnsureVisible(m_hookList.GetItemCount()-1,FALSE);
}


void CMonitorDllWindowDlg::OnBnClickedButtonMsga()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	MessageBoxA(m_hWnd,"����������MessageBoxA...","����",0);
}


void CMonitorDllWindowDlg::OnBnClickedButtonMsgw()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	MessageBoxW(_T("����������MessageBoxW..."),_T("�Ǻ�"),0);
}
