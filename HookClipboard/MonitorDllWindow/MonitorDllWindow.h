
// MonitorDllWindow.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CMonitorDllWindowApp:
// �йش����ʵ�֣������ MonitorDllWindow.cpp
//

class CMonitorDllWindowApp : public CWinApp
{
public:
	CMonitorDllWindowApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMonitorDllWindowApp theApp;