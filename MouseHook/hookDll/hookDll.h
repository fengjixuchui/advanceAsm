// hookDll.h : hookDll DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// ChookDllApp
// �йش���ʵ�ֵ���Ϣ������� hookDll.cpp
//

class ChookDllApp : public CWinApp
{
public:
	ChookDllApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
