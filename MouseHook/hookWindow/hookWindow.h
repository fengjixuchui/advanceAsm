
// hookWindow.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// ChookWindowApp:
// �йش����ʵ�֣������ hookWindow.cpp
//

class ChookWindowApp : public CWinApp
{
public:
	ChookWindowApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern ChookWindowApp theApp;