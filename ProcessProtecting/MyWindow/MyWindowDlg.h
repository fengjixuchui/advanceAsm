
// MyWindowDlg.h : ͷ�ļ�
//

#pragma once


// CMyWindowDlg �Ի���
class CMyWindowDlg : public CDialogEx
{
// ����
public:
	CMyWindowDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_MYWINDOW_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;


	HINSTANCE m_hinstHookDll;	//	MonitorDll��ʵ�����
	void HookLoad();			//	����HOOK			
	void HookUnload();			//	ж��HOOK

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	afx_msg void OnClose();		//	�رճ����ʱ��ж��DLL !!!!!

	DECLARE_MESSAGE_MAP()
};
