
// hookWindowDlg.h : ͷ�ļ�
//

#pragma once


// ChookWindowDlg �Ի���
class ChookWindowDlg : public CDialogEx
{
// ����
public:
	ChookWindowDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_HOOKWINDOW_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg LRESULT OnMouseMsg(WPARAM wParam,LPARAM lParam);	// ���HOOL DLL ������Ϣ������
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonHook();
	afx_msg void OnBnClickedCheckMove();
	afx_msg void OnBnClickedCheckWheel();

	CEdit m_ediInfo;
	CListCtrl m_List;
	bool m_bIsShowMouseMoveMsg;
	bool m_bIsSHowMouseWheelMsg;
};
