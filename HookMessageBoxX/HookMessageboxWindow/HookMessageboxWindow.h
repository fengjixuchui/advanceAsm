
// HookMessageboxWindow.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CHookMessageboxWindowApp:
// �йش����ʵ�֣������ HookMessageboxWindow.cpp
//

class CHookMessageboxWindowApp : public CWinApp
{
public:
	CHookMessageboxWindowApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CHookMessageboxWindowApp theApp;