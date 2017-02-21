
// SerialNumHelper.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "SerialNumHelper.h"
#include "SerialNumHelperDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSerialNumHelperApp

BEGIN_MESSAGE_MAP(CSerialNumHelperApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CSerialNumHelperApp ����

CSerialNumHelperApp::CSerialNumHelperApp()
{
	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CSerialNumHelperApp ����

CSerialNumHelperApp theApp;


// CSerialNumHelperApp ��ʼ��

BOOL CSerialNumHelperApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	// ����Ψһʵ��
	HANDLE hMutex = ::CreateMutex(NULL, FALSE, L"SerialNumHelper.exe");
	if (ERROR_ALREADY_EXISTS == GetLastError())
	{
		HWND hWnd;

		// �����Ѵ��ڵĳ���
		hWnd = FindWindow(NULL, L"���к���д");
		if (NULL != hWnd)
		{
			// �жϵ�ǰ�����Ƿ���С��
			if (::IsIconic(hWnd))
			{
				// ������ʾ������Ϣ
				::PostMessage(hWnd, WM_SYSCOMMAND, SC_RESTORE, NULL);
			}

			SetForegroundWindow(hWnd); // �����
			BringWindowToTop(hWnd);    // �������ö���
		}

		// �رջ���
		CloseHandle(hMutex);
		return FALSE;
	}


	CWinApp::InitInstance();

	AfxEnableControlContainer();


	// ���� shell ���������Է��Ի������
	// �κ� shell ����ͼ�ؼ��� shell �б���ͼ�ؼ���
	CShellManager *pShellManager = new CShellManager;

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	CSerialNumHelperDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}

	// ɾ�����洴���� shell ��������
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}

