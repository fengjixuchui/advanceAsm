
// hookWindowDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "hookWindow.h"
#include "hookWindowDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_MOUSEMSG WM_USER + 106

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


// ChookWindowDlg �Ի���




ChookWindowDlg::ChookWindowDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(ChookWindowDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void ChookWindowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_LIST,m_List);
}

BEGIN_MESSAGE_MAP(ChookWindowDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_START, &ChookWindowDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_HOOK, &ChookWindowDlg::OnBnClickedButtonHook)
	ON_BN_CLICKED(IDC_CHECK_MOVE, &ChookWindowDlg::OnBnClickedCheckMove)
	ON_BN_CLICKED(IDC_CHECK_WHEEL, &ChookWindowDlg::OnBnClickedCheckWheel)

	ON_MESSAGE(WM_MOUSEMSG,&ChookWindowDlg::OnMouseMsg) //��Ϣӳ��

END_MESSAGE_MAP()


// ChookWindowDlg ��Ϣ�������

BOOL ChookWindowDlg::OnInitDialog()
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
	m_List.InsertColumn(0,_T("wParam"));
	m_List.InsertColumn(1,_T("lParam"));
	m_List.InsertColumn(2,_T("�����Ϣ"));
	m_List.InsertColumn(3,_T("�������"));
	m_List.InsertColumn(4,_T("���ڴ���"));
	m_List.SetColumnWidth(0,100);
	m_List.SetColumnWidth(1,100);
	m_List.SetColumnWidth(2,150);
	m_List.SetColumnWidth(3,100);
	m_List.SetColumnWidth(4,250);
	m_List.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	// �õ���ѡ��״̬��Ϣ
	CButton* pbton = (CButton*)GetDlgItem(IDC_CHECK_MOVE);
	pbton->SetCheck(TRUE);
	pbton = (CButton*)GetDlgItem(IDC_CHECK_WHEEL);
	pbton->SetCheck(TRUE);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void ChookWindowDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void ChookWindowDlg::OnPaint()
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
HCURSOR ChookWindowDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




HINSTANCE g_hInstanceDll = NULL;
//
//	������깳��
//
void ChookWindowDlg::OnBnClickedButtonStart()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	g_hInstanceDll = LoadLibrary(_T("hookDll.dll"));
	if (NULL == g_hInstanceDll)
	{
		AfxMessageBox(_T("����hookDll.dllʧ��"));
		return;
	}
	typedef BOOL (CALLBACK *StartHookMouse)(HWND hwnd);
	StartHookMouse startHook;
	startHook = (StartHookMouse) ::GetProcAddress(g_hInstanceDll,"StartHookMouse");
	if ( NULL == startHook )
	{
		AfxMessageBox(_T("��ȡ StartHookMouse ����ʧ��"));
		return;
	}

	if (startHook(this->m_hWnd))
	{
		m_List.InsertItem(m_List.GetItemCount(),_T("0"));
		m_List.SetItemText(m_List.GetItemCount()-1,1,_T("0"));
		m_List.SetItemText(m_List.GetItemCount()-1,2,_T("������깳�ӳɹ�"));
	}
	else
	{
		m_List.InsertItem(m_List.GetItemCount(),_T("0"));
		m_List.SetItemText(m_List.GetItemCount()-1,1,_T("0"));
		m_List.SetItemText(m_List.GetItemCount()-1,2,_T("������깳��ʧ��"));
	}
	
}

//
// ֹͣ��깳��HOOK
//
void ChookWindowDlg::OnBnClickedButtonHook()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	typedef VOID (CALLBACK *StopHookMouse)();
	StopHookMouse stopHook;
	g_hInstanceDll = LoadLibrary(_T("hookDll.dll"));
	if ( g_hInstanceDll == NULL)
	{
		AfxMessageBox(_T("����DLLʧ��"));
		return;
	}

	stopHook = (StopHookMouse) ::GetProcAddress(g_hInstanceDll,"StopHookMouse");
	if (stopHook == NULL)
	{
		m_List.InsertItem(m_List.GetItemCount(),_T("0"));
		m_List.SetItemText(m_List.GetItemCount()-1,1,_T("0"));
		m_List.SetItemText(m_List.GetItemCount()-1,2,_T("��ȡ���� StopHookMouse ʧ��"));
		return;
	}
	else
	{
		stopHook();
		m_List.InsertItem(m_List.GetItemCount(),_T("0"));
		m_List.SetItemText(m_List.GetItemCount()-1,1,_T("0"));
		m_List.SetItemText(m_List.GetItemCount()-1,2,_T("ֹͣHOOKMOUSE�ɹ�"));
	}

	if (g_hInstanceDll != NULL)
	{
		::FreeLibrary(g_hInstanceDll);
	}

	// ȷ��list control ���һ�пɼ�
	m_List.EnsureVisible(m_List.GetItemCount()-1,FALSE);

}

// ��Ϣ��Ӧ����
LRESULT ChookWindowDlg::OnMouseMsg(WPARAM wParam,LPARAM lParam)
{
	static CString str;
	static PMSLLHOOKSTRUCT mouseHookStruct;

	// ������
	str.Format(_T("0x%X"),wParam);
	m_List.InsertItem(m_List.GetItemCount(),str);
	str.Format(_T("0x%X"),lParam);
	m_List.SetItemText(m_List.GetItemCount()-1,1,str);

	if (wParam == WM_LBUTTONDBLCLK)	// ����������
	{
		str = _T("WM_LBUTTONDBLCLK");
		m_List.SetItemText(m_List.GetItemCount()-1,2,str);
	}
	else if (wParam == WM_LBUTTONUP) // ����������
	{
		str = _T("WM_LOBUTTONUP");
		m_List.SetItemText(m_List.GetItemCount()-1,2,str);
	}
	else if ( wParam == WM_RBUTTONDBLCLK) // ����Ҽ�����
	{
		str = _T("WM_RBUTTONDELCLK");
		m_List.SetItemText(m_List.GetItemCount()-1,2,str);
	}
	else if ( wParam == WM_RBUTTONUP) // ����Ҽ�����
	{
		str = _T("WM_RBUTTONUP");
		m_List.SetItemText(m_List.GetItemCount()-1,2,str);
	}
	else if ( wParam == WM_MOUSEMOVE) // ����ƶ�
	{
		if ( !m_bIsShowMouseMoveMsg && m_List.GetItemCount()>0 )
		{
			m_List.DeleteItem(m_List.GetItemCount()-1);
			return 1;
		}
		
		str = _T("WM_MOUSEMOVE");
		m_List.SetItemText(m_List.GetItemCount()-1,2,str);
	}
	else if ( wParam == WM_MOUSEWHEEL) // ������
	{
		if ( !m_bIsSHowMouseWheelMsg && m_List.GetItemCount()>0)
		{
			m_List.DeleteItem(m_List.GetItemCount()-1);
			return 1;
		}
		str = _T("WM_MOUSEWHEEL");
		m_List.SetItemText(m_List.GetItemCount()-1,2,str);
	}

	// ���λ��
	mouseHookStruct = (PMSLLHOOKSTRUCT)lParam;
	str.Format(_T("%d,%d"),mouseHookStruct->pt.x,mouseHookStruct->pt.y);
	m_List.SetItemText(m_List.GetItemCount()-1,3,str);

	// ���ڴ���
	HWND hwnd = ::WindowFromPoint(mouseHookStruct->pt);
	TCHAR title[MAX_PATH];
	::GetWindowText(hwnd,title,MAX_PATH);
	m_List.SetItemText(m_List.GetItemCount()-1,4,title);

	m_List.EnsureVisible(m_List.GetItemCount()-1,FALSE);

	return 1;
}

//	����ʾ����ƶ���Ϣ
void ChookWindowDlg::OnBnClickedCheckMove()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	if (BST_CHECKED == IsDlgButtonChecked(IDC_CHECK_MOVE))
	{
		m_bIsShowMouseMoveMsg = FALSE;
	}
	else
	{
		m_bIsShowMouseMoveMsg = TRUE;
	}
}

//	����ʾ����������Ϣ
void ChookWindowDlg::OnBnClickedCheckWheel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	if (BST_CHECKED == IsDlgButtonChecked(IDC_CHECK_WHEEL))
	{
		m_bIsSHowMouseWheelMsg = FALSE;
	}
	else
	{
		m_bIsSHowMouseWheelMsg = TRUE;
	}
}
