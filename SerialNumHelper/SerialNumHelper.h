
// SerialNumHelper.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CSerialNumHelperApp:
// �йش����ʵ�֣������ SerialNumHelper.cpp
//

class CSerialNumHelperApp : public CWinApp
{
public:
	CSerialNumHelperApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CSerialNumHelperApp theApp;