
// MonitorDllWindowDlg.h : ͷ�ļ�
//

#pragma once


// CMonitorDllWindowDlg �Ի���
class CMonitorDllWindowDlg : public CDialogEx
{
// ����
public:
	CMonitorDllWindowDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_MONITORDLLWINDOW_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;
	CListCtrl m_hookList;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg LRESULT OnLogHook(WPARAM wParam,LPARAM lParam);	// ��Ϣ������
	afx_msg void OnBnClickedButtonLoad();
	afx_msg void OnBnClickedButtonUnload();
	afx_msg void OnBnClickedButtonMsga();
	afx_msg void OnBnClickedButtonMsgw();
};
