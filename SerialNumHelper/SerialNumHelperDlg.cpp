
// SerialNumHelperDlg.cpp : 实现文件
//

#include "stdafx.h"
#include <Dbt.h>
#include <InitGuid.h>
#include "SerialNumHelper.h"
#include "SerialNumHelperDlg.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// USB Guid
DEFINE_GUID(GUID_DEVINTERFACE_USB_DEVICE, 0xA5DCBF10L, 0x6530, 0x11D2, 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED);
#define GUID_CLASS_USB_DEVICE GUID_DEVINTERFACE_USB_DEVICE

const CString CONFIG_INI = L"config.ini";
const CString INI_SECTION_PATH = L"SNPath";
const CString INI_SECTION_SETTING = L"Setting";
const CString INI_KEY_PATH = L"Path";
const CString INI_KEY_SN = L"SN";
const CString INI_KEY_WIFI = L"WIFI";
const CString INI_KEY_BT = L"BT";


void debug_log(LPTSTR formats, ...)
{
	va_list ap;
	va_start(ap, formats);

	SYSTEMTIME tm;
	ZeroMemory(&tm, sizeof(SYSTEMTIME));
	GetLocalTime(&tm);

#ifdef UNICODE
	WCHAR info[1056] = {0};
	WCHAR buf[1024] = {0};
	swprintf(info, L"%02d:%02d:%03d  ", tm.wMinute, tm.wSecond, tm.wMilliseconds);
	vswprintf_s(buf, 1023, formats, ap);
	wcscat_s(info, 1056, buf);
	wcscat_s(info, 1056, L"\n");
#else
	CHAR buf[1024] = {0};
	sprintf_s()
	vsprintf_s(buf, 1023, formats, ap);
	strcat_s(buf, 1024, "\n");
#endif
	va_end(ap);
	OutputDebugString(info);
}

void UnicodeToAnsi(WCHAR *_wch, CHAR *_ch)
{
	int nLen = WideCharToMultiByte(CP_ACP, 0, _wch, -1, NULL, 0, NULL, NULL); 
	//debug_log(L"Unicode len:%d", nLen);
	WideCharToMultiByte(CP_ACP, 0, _wch, -1, _ch, nLen, NULL, NULL);
}

void AnsiToUnicode(CHAR *_ch, WCHAR *_wch)
{
	int nLen = MultiByteToWideChar(CP_ACP, 0, _ch, -1, NULL, 0);
	//debug_log(L"Ansi len:%d", nLen);
	MultiByteToWideChar(CP_ACP, 0, _ch, -1, _wch, nLen);
}

BYTE D2H(CHAR c)
{
	if (c >= '0' && c <= '9') return (c - '0');
	if (c >= 'A' && c <= 'F') return (c - 'A' + 10);
	if (c >= 'a' && c <= 'f') return (c - 'a' + 10);
	
	return 0;
}

void Hex2Str(BYTE *src, int srcLen, CHAR *dst, int dstSize)
{
	int i;
	char szTmp[3] = {0};

	// CHAR *dst的大小必须至少为srcLen的两倍+1
	if (dstSize < 2*srcLen+1) return;

	for (i = 0; i < srcLen; i++)
	{
		sprintf_s(szTmp, 3, "%02x", *(src+i));
		memcpy(&dst[2*i], szTmp, 2);
	}

	dst[2*i] = '\0';
}

BOOL IsEven(CString str)
{
	long num = wcstol(str.GetBuffer(), NULL, 16);
	str.ReleaseBuffer();

	if (num % 2 == 0)
		return TRUE;
	else
		return FALSE;
}

CString FindKeyValue(char *value, const char *key)
{

	char *p, *end;

	if ((p = strstr(value, key)) == NULL)
		return L"";

	if ((end = strchr(p, '\n')) != NULL || (end = strstr(p, '\0')) != NULL)
	{
		p = end + 1;
		if ((end = strchr(p, '\n')) != NULL || (end = strchr(p, '\0')) != NULL)
		{
			int size = end - p;
			char *cstr = new char[size+1];
			memset(cstr, 0, size+1);
			memcpy_s(cstr, size+1, p, size);

			CStringA strVal;
			strVal.Format("%s", cstr);
			delete[] cstr;

			return CString(strVal);
		}
	}

	return L"";
}

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
//	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CSerialNumHelperDlg 对话框




CSerialNumHelperDlg::CSerialNumHelperDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSerialNumHelperDlg::IDD, pParent)
	, mStrWifiRelTm(_T("")), m_ConfigPath(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hAccel = ::LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR1));


	m_ComFont.CreatePointFont(120, L"微软雅黑");
	m_ResultFont.CreatePointFont(360, L"微软雅黑");
	m_SmallFont.CreatePointFont(90, L"微软雅黑");
	m_DevFont.CreateFontW(20, 0, 0, 0, FW_BOLD, 0, 1, 0, 0, 0, 0, 0, 0, L"宋体");
	m_PassBrush.CreateSolidBrush(RGB(0,255,0));
	m_FailBrush.CreateSolidBrush(RGB(255,0,0));
	m_NoneBrush.CreateSolidBrush(::GetSysColor(COLOR_3DFACE));
	mExcEvent = EVENT_NONE;
	m_ExcResult = STATE_NONE;
	m_bDevInserted = FALSE;
	m_bIsSwitching = FALSE;
	m_pSNEditUpdateThread = NULL;
}


CSerialNumHelperDlg::~CSerialNumHelperDlg()
{
	m_ComFont.DeleteObject();
	m_ResultFont.DeleteObject();
	m_DevFont.DeleteObject();
	m_SmallFont.DeleteObject();
	m_PassBrush.DeleteObject();
	m_FailBrush.DeleteObject();
	m_NoneBrush.DeleteObject();
	m_RkDevCtl.UnInitialize();
}


void CSerialNumHelperDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_DEV, mStaticDev);
	DDX_Control(pDX, IDC_EDIT_SN, mEditSN);
	DDX_Control(pDX, IDC_EDIT_WIFI, mEditWIFI);
	DDX_Control(pDX, IDC_EDIT_BT, mEditBT);
	DDX_Control(pDX, IDC_BUTTON_IMPORT, mBtnImport);
	DDX_Control(pDX, IDC_BUTTON_SWITCH, mBtnSwitch);
	DDX_Control(pDX, IDC_BUTTON_WRITE, mBtnWrite);
	DDX_Control(pDX, IDC_BUTTON_READ, mBtnRead);
	DDX_Control(pDX, IDC_STATIC_RESULT, mStaticResult);
	DDX_Control(pDX, IDC_CHECK_SN, mCheckSN);
	DDX_Control(pDX, IDC_CHECK_WIFI, mCheckWIFI);
	DDX_Control(pDX, IDC_CHECK_BT, mCheckBT);
	DDX_Control(pDX, IDC_BUTTON_REBOOT, mBtnReboot);
	DDX_Text(pDX, IDC_EDIT_WIFI, mStrWifiRelTm);
	DDX_Text(pDX, IDC_EDIT_BT, mStrBTRelTm);
	DDX_Control(pDX, IDC_LIST_MSG, mMsgList);
}

BEGIN_MESSAGE_MAP(CSerialNumHelperDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CTLCOLOR()
	ON_WM_DEVICECHANGE()
	ON_BN_CLICKED(IDC_BUTTON_IMPORT, &CSerialNumHelperDlg::OnBnClickedImport)
	ON_BN_CLICKED(IDC_BUTTON_SWITCH, &CSerialNumHelperDlg::OnBnClickedSwitch)
	ON_BN_CLICKED(IDC_BUTTON_WRITE, &CSerialNumHelperDlg::OnBnClickedWrite)
	ON_BN_CLICKED(IDC_BUTTON_READ, &CSerialNumHelperDlg::OnBnClickedRead)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_CHECK_SN, &CSerialNumHelperDlg::OnBnClickedCheckSn)
	ON_BN_CLICKED(IDC_CHECK_WIFI, &CSerialNumHelperDlg::OnBnClickedCheckWifi)
	ON_BN_CLICKED(IDC_CHECK_BT, &CSerialNumHelperDlg::OnBnClickedCheckBt)
	ON_BN_CLICKED(IDC_BUTTON_REBOOT, &CSerialNumHelperDlg::OnBnClickedReboot)
	ON_EN_UPDATE(IDC_EDIT_WIFI, &CSerialNumHelperDlg::OnUpdateEditWifi)
	ON_EN_UPDATE(IDC_EDIT_BT, &CSerialNumHelperDlg::OnEnUpdateEditBt)
	ON_EN_CHANGE(IDC_EDIT_SN, &CSerialNumHelperDlg::OnEnChangeEditSn)
END_MESSAGE_MAP()


// CSerialNumHelperDlg 消息处理程序

BOOL CSerialNumHelperDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	InitUI();

	// 注册USB设备插拔事件
	DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
	ZeroMemory(&NotificationFilter, sizeof(NotificationFilter));
	NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
	NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
	NotificationFilter.dbcc_classguid = GUID_CLASS_USB_DEVICE;
	RegisterDeviceNotification(m_hWnd, &NotificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE);

	// 初始化设备
	m_RkDevCtl.Initialize();
	ENUM_RKUSB_TYPE type = m_RkDevCtl.GetDevType();
	if (type != RKUSB_NONE)
	{
		m_bDevInserted = TRUE;
	}
	SetDevStatus(type);

	// 加载配置
	LoadConfig();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}


void CSerialNumHelperDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	SaveConfig();

	CDialogEx::OnClose();
}


void CSerialNumHelperDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CSerialNumHelperDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CSerialNumHelperDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CSerialNumHelperDlg::InitUI()
{

	CWnd *pWnd = GetWindow(GW_CHILD);
	WCHAR buf[512];

	while (pWnd)
	{
		memset(buf, 0, sizeof(buf));
		::GetClassNameW(pWnd->GetSafeHwnd(), buf, 512);

		if (pWnd->GetDlgCtrlID() == IDC_STATIC_RESULT)
		{
			pWnd = pWnd->GetNextWindow(GW_HWNDNEXT);
			continue;
		}
		else if (pWnd->GetDlgCtrlID() == IDC_STATIC_DEV)
		{
			pWnd->SetFont(&m_DevFont);
			pWnd = pWnd->GetNextWindow(GW_HWNDNEXT);
			continue;
		}else if (pWnd->GetDlgCtrlID() == IDC_STATIC_WARN)
		{
			pWnd->SetFont(&m_SmallFont);
			pWnd = pWnd->GetNextWindow(GW_HWNDNEXT);
			continue;
		}

		CString str = buf;

		/*if (str == L"Static")
		{
		pWnd->SetFont(&m_ComFont);
		} 
		else if (str == L"Edit")
		{
		pWnd->SetFont(&m_ComFont);
		}
		else if (str == L"Button")
		{
		pWnd->SetFont(&m_ComFont);
		}*/

		pWnd->SetFont(&m_ComFont);
		pWnd = pWnd->GetNextWindow(GW_HWNDNEXT);
	}
	
	mStaticResult.SetFont(&m_ResultFont);
	mStaticDev.SetWindowTextW(L"没有检测到设备");
}

BOOL CSerialNumHelperDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_CHAR) 
	{
		if (!CorrectInputChar(pMsg)) return TRUE;
	}
	if (::TranslateAccelerator(m_hWnd, m_hAccel, pMsg))
	{
		return TRUE;
	}
	/*if (WM_KEYDOWN == pMsg->message && VK_RETURN == pMsg->wParam)
	{
		return TRUE;
	}*/
	else if (WM_KEYDOWN == pMsg->message && VK_ESCAPE == pMsg->wParam)
	{
		return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

HBRUSH CSerialNumHelperDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性
	if (pWnd->GetDlgCtrlID() == IDC_STATIC_RESULT)
	{
		pDC->SetBkMode(TRANSPARENT);

		if (m_ExcResult == STATE_PASS)
			return m_PassBrush;
		else if (m_ExcResult == STATE_FAIL)
			return m_FailBrush;
		else
			return m_NoneBrush;
	}
	else if (pWnd->GetDlgCtrlID() == IDC_STATIC_WARN)
	{
		pDC->SetTextColor(RGB(255,0,0));
	}

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}

void CSerialNumHelperDlg::SetExcResult(TEST_STATE state)
{
	m_ExcResult = state;
	if (m_ExcResult == STATE_PASS)
		mStaticResult.SetWindowTextW(L"PASS");
	else if (m_ExcResult == STATE_FAIL)
		mStaticResult.SetWindowTextW(L"FAIL");
	else
		mStaticResult.SetWindowTextW(L"");
	mStaticResult.Invalidate();
}

BOOL CSerialNumHelperDlg::OnDeviceChange(UINT nEventType, DWORD_PTR dwData)
{
	ENUM_RKUSB_TYPE dev_type = RKUSB_NONE;
	
	//debug_log(L"USB Dev Change:%x", nEventType);
	if (nEventType == DBT_DEVICEARRIVAL && !m_bDevInserted)
	{
		// A device has been inserted and is now available. 
		//debug_log(L"OnDeviceChange: DBT_DEVICEARRIVAL");

		dev_type = m_RkDevCtl.GetDevType();
		if (dev_type != RKUSB_NONE)
		{
			m_bDevInserted = TRUE;
		}
		SetDevStatus(dev_type);
	}
	else if (nEventType == DBT_DEVICEREMOVECOMPLETE && m_bDevInserted)
	{
		// Device has been removed
		//debug_log(L"OnDeviceChange: DBT_DEVICEREMOVECOMPLETE");

		if (m_bIsSwitching)
		{
			dev_type = RKUSB_NONE;
			m_bDevInserted = FALSE;
		}
		else
		{
			dev_type = m_RkDevCtl.GetDevType();
			if (dev_type == RKUSB_NONE)
			{
				m_bDevInserted = FALSE;
			}
		}

		SetDevStatus(dev_type);
	}

	return TRUE;
}

void CSerialNumHelperDlg::SetDevStatus(ENUM_RKUSB_TYPE type)
{
	CString strDev;

	switch (type) 
	{
	case RKUSB_NONE:
		strDev = L"没有检测到设备";
		break;
	case RKUSB_MASKROM:
		strDev = L"发现一台ADB设备";
		break;
	case RKUSB_MSC:
		strDev = L"发现一台MSC设备";
		break;
	case RKUSB_LOADER:
		strDev = L"发现一台LOADER设备";
		break;

	default:
		break;
	}

	mStaticDev.SetWindowTextW(strDev);
}


static UINT ExecuteThread(LPVOID pParam)
{
	CSerialNumHelperDlg *dlg = (CSerialNumHelperDlg *)pParam;
	if (dlg != NULL)
	{
		if (dlg->mExcEvent == EVENT_SNUPDATE)
		{
			dlg->SNEditUpdating();
		}
		else
		{
			dlg->HandleEvent();
		}

	}
	return 0;
}


void CSerialNumHelperDlg::OnBnClickedImport()
{
	// TODO: 在此添加控件通知处理程序代码
	mExcEvent = EVENT_IMPORT;
	AfxBeginThread(ExecuteThread, this);
}


void CSerialNumHelperDlg::OnBnClickedSwitch()
{
	// TODO: 在此添加控件通知处理程序代码
	mExcEvent = EVENT_SWITCH;
	AfxBeginThread(ExecuteThread, this);
}

void CSerialNumHelperDlg::OnBnClickedReboot()
{
	// TODO: 在此添加控件通知处理程序代码
	mExcEvent = EVENT_REBOOT;
	AfxBeginThread(ExecuteThread, this);
}


void CSerialNumHelperDlg::OnBnClickedWrite()
{
	// TODO: 在此添加控件通知处理程序代码
	mExcEvent = EVENT_WRITE;
	AfxBeginThread(ExecuteThread, this);
}


void CSerialNumHelperDlg::OnBnClickedRead()
{
	// TODO: 在此添加控件通知处理程序代码
	mExcEvent = EVENT_READ;
	AfxBeginThread(ExecuteThread, this);
}

void CSerialNumHelperDlg::EnableUIWindows(BOOL bEnable)
{
	mEditSN.EnableWindow(bEnable & mCheckSN.GetCheck());
	mEditWIFI.EnableWindow(bEnable & mCheckWIFI.GetCheck());
	mEditBT.EnableWindow(bEnable & mCheckBT.GetCheck());

	mBtnImport.EnableWindow(bEnable);
	mBtnSwitch.EnableWindow(bEnable);
	mBtnReboot.EnableWindow(bEnable);
	mBtnWrite.EnableWindow(bEnable);
	mBtnRead.EnableWindow(bEnable);

	mCheckSN.EnableWindow(bEnable);
	mCheckWIFI.EnableWindow(bEnable);
	mCheckBT.EnableWindow(bEnable);
}

void CSerialNumHelperDlg::HandleEvent()
{
	EnableUIWindows(FALSE);

	// clear last result
	SetExcResult(STATE_NONE);
	mMsgList.ResetContent();

	switch (mExcEvent)
	{
	case EVENT_IMPORT:
		SelectImportSNFile();
		break;

	case EVENT_SWITCH:
		if (IsDeviceInserted())
		{
			if (SwitchToLoader())
			{
				AfxMessageBox(L"模式切换成功", MB_OK);
			}
			else
			{
				AfxMessageBox(L"模式切换失败", MB_OK);
			}
		}
		break;

	case EVENT_REBOOT:
		if (IsDeviceInserted())
		{
			if (IsInLoaderMode())
				ExitLoader();
			else
				AfxMessageBox(L"设备必须在<Loader>模式下执行重启操作", MB_OK | MB_ICONSTOP);
		}
		break;

	case EVENT_WRITE:
		if (IsDeviceInserted())
		{
			WriteSerialNums();
		}
		break;

	case EVENT_READ:
		if (IsDeviceInserted())
		{
			if (IsInLoaderMode())
				ReadSerialNums();
			else
				AfxMessageBox(L"设备必须在<Loader>模式下执行读取操作", MB_OK | MB_ICONSTOP);
		}
		break;

	default:
		break;
	}

	EnableUIWindows(TRUE);
	SendMessage(WM_SETFOCUS, (WPARAM)GetDlgItem(IDC_EDIT_SN));
	mEditSN.SetSel(0, -1);
}

void CSerialNumHelperDlg::SelectImportSNFile()
{
	CString strPath, filter;
	filter = L"文本文件(*.txt)|*.txt||";
	CFileDialog openDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, filter, NULL);
	if (!m_SNPath.IsEmpty()) openDlg.m_ofn.lpstrInitialDir = m_SNPath;

	if (openDlg.DoModal() == IDOK)
	{

		strPath = openDlg.GetPathName();
		m_SNPath = strPath.Left(strPath.ReverseFind(L'\\') + 1);
		
		ImportSNFile(strPath);
	}
}


void CSerialNumHelperDlg::AutoImportSNFile(CString strName)
{
	CString path = m_SNPath + strName + L".txt";

	if (strName.IsEmpty() || !PathFileExistsW(path))
		return;

	ImportSNFile(path);
}


void CSerialNumHelperDlg::ImportSNFile(CString path)
{
	//debug_log(L"Import file path:%s", path);
	if (GetFileEncode(path) != L"ANSI")
	{
		AfxMessageBox(L"文件非ANSI编码格式，无法读取", MB_OK | MB_ICONSTOP);
		return;
	}
	
	CStdioFile file;
	if (!file.Open(path, CFile::modeRead|CFile::shareDenyNone))
		return;

	file.SeekToBegin();

	ULONGLONG cur, tol, size;
	size = file.GetLength();
	//debug_log(L"File size:%d", size);

	char *cTxt = new char[size];
	memset(cTxt, 0, size);

	tol = 0;
	while ((cur = file.Read(cTxt+tol, size-tol)) > 0)
	{
		tol += cur;
	}
	//cTxt[tol] = '\0';
	file.Close();

	// Get data
	CString strSn = FindKeyValue(cTxt, "[SN]");
	if (strSn.IsEmpty())
	{
		mCheckSN.SetCheck(BST_UNCHECKED);
		mEditSN.SetWindowTextW(L"");
		mEditSN.EnableWindow(FALSE);
	}
	else
	{
		mCheckSN.SetCheck(BST_CHECKED);
		mEditSN.SetWindowTextW(strSn);
		mEditSN.EnableWindow(TRUE);
	}

	CString strWifi = FindKeyValue(cTxt, "[WIFI]");
	if (strWifi.IsEmpty())
	{
		mCheckWIFI.SetCheck(BST_UNCHECKED);
		mEditWIFI.SetWindowTextW(L"");
		mEditWIFI.EnableWindow(FALSE);
	}
	else
	{
		mCheckWIFI.SetCheck(BST_CHECKED);
		mEditWIFI.SetWindowTextW(strWifi);
		mEditWIFI.EnableWindow(TRUE);
	}

	CString strBt = FindKeyValue(cTxt, "[BT]");
	if (strBt.IsEmpty())
	{
		mCheckBT.SetCheck(BST_UNCHECKED);
		mEditBT.SetWindowTextW(L"");
		mEditBT.EnableWindow(FALSE);
	}
	else
	{
		mCheckBT.SetCheck(BST_CHECKED);
		mEditBT.SetWindowTextW(strBt);
		mEditBT.EnableWindow(TRUE);
	}
	delete[] cTxt;
}

CString CSerialNumHelperDlg::GetFileEncode(CString path)
{	
	CFile file;
	BYTE head[3] = {0};

	if (file.Open(path, CFile::modeRead))
	{
		file.Read(head, 3);
		file.Close();
		if (head[0] == 0xEF && head[1] == 0xBB && head[2] == 0xBF)
		{
			//debug_log(L"%s", L"UTF-8编码格式文件");
			return L"UTF-8";
		}
		else if (head[0] == 0xFF && head[1] == 0xFE)
		{
			//debug_log(L"%s", L"Unicode编码格式文件");
			return L"Unicode";
		}
		else if (head[0] == 0xFE && head[1] == 0xFF)
		{
			//debug_log(L"%s", L"Unicode big endian编码格式文件");
			return L"Unicode Big Endian";
		}
		else
		{
			//debug_log(L"%s", L"ASCII编码格式文件");
			return L"ANSI";
		}
	}

	return L"UnKnow";
}

void CSerialNumHelperDlg::WriteSerialNums()
{
	CString strSn, strWifi, strBt;
	BOOL bCheckSn, bCheckWifi, bCheckBt;
	CString strErr;
	int ret;

	// get check
	bCheckSn = (BOOL)mCheckSN.GetCheck();
	bCheckWifi = (BOOL)mCheckWIFI.GetCheck();
	bCheckBt = (BOOL)mCheckBT.GetCheck();

	mEditSN.GetWindowTextW(strSn);
	mEditWIFI.GetWindowTextW(strWifi);
	mEditBT.GetWindowTextW(strBt);

	if (bCheckSn && (strSn.GetLength() <= 0 || strSn.GetLength() > 30))
	{
		strErr = L"无效的SN数据\n(只能包含字母、数字，最多为30个字符)";
		AfxMessageBox(strErr, MB_OK|MB_ICONSTOP);
		return;
	}

	if (bCheckWifi && (strWifi.GetLength() != 17 || !IsEven(strWifi.Left(2))))
	{
		strErr = L"无效的Wifi数据\n(16进制数据，例如:A0-B1-C2-D3-E4-F5，且第一个值必须是偶数)";
		AfxMessageBox(strErr, MB_OK|MB_ICONSTOP);
		return;
	}
	if (bCheckBt && strBt.GetLength() != 17)
	{
		strErr = L"无效的BT数据\n(16进制数据，例如:A1-B2-C3-D4-E5-F6)";
		AfxMessageBox(strErr, MB_OK|MB_ICONSTOP);
		return;
	}
	
	if (!IsInLoaderMode())
	{
		AddProgressInfo(L"正在切换至Loader模式...");
		if (!SwitchToLoader())
		{
			AddProgressInfo(L"模式切换失败");
			goto failed;
		}
		AddProgressInfo(L"模式切换成功");
	}

	CUSTOM_DATA data;
	ZeroMemory(&data, sizeof(CUSTOM_DATA));

	if (bCheckSn)
	{
		data.bSn = TRUE;
		SetSnData(data.sn, data.snLen, strSn);
	}
	if (bCheckWifi)
	{
		data.bWifi = TRUE;
		SetWifiData(data.wifi, strWifi);
	}
	if (bCheckBt)
	{
		data.bBt = TRUE;
		SetBtData(data.bt, strBt);
	}

	AddProgressInfo(L"正在写入数据...");
	ret = m_RkDevCtl.WriteAllInfo(&data);
	if (ret != ERR_NONE)
		goto failed;

	AddProgressInfo(L"写入数据成功");
	SetExcResult(STATE_PASS);
	return;

failed:
	SetExcResult(STATE_FAIL);
}

void CSerialNumHelperDlg::ReadSerialNums()
{
	BOOL bSuccess;
	CString strSn, strWifi, strBt;
	BOOL bCheckSn, bCheckWifi, bCheckBt;

	// clear edit
	mEditSN.SetWindowTextW(L"");
	mEditWIFI.SetWindowTextW(L"");
	mEditBT.SetWindowTextW(L"");

	// get check
	bCheckSn = (BOOL)mCheckSN.GetCheck();
	bCheckWifi = (BOOL)mCheckWIFI.GetCheck();
	bCheckBt = (BOOL)mCheckBT.GetCheck();

	CUSTOM_DATA data;
	ZeroMemory(&data, sizeof(CUSTOM_DATA));

	bSuccess = m_RkDevCtl.ReadAllInfo(data);
	if (!bSuccess)
		goto failed;

	if (bCheckSn)
	{
		GetSnData(data.sn, data.snLen, strSn);
		mEditSN.SetWindowTextW(strSn);
	}
	if (bCheckWifi)
	{
		GetWifiData(data.wifi, strWifi);
		mEditWIFI.SetWindowTextW(strWifi);
	}
	if (bCheckBt)
	{
		GetBtData(data.bt, strBt);
		mEditBT.SetWindowTextW(strBt);
	}

	SetExcResult(STATE_PASS);
	return;

failed:
	SetExcResult(STATE_FAIL);
}

BOOL CSerialNumHelperDlg::SwitchToLoader()
{
	BOOL bRet ;

	m_bIsSwitching = TRUE;

	bRet = m_RkDevCtl.SwtichToLoaderMode();

	m_bIsSwitching = FALSE;

	return bRet;	
}

void CSerialNumHelperDlg::ExitLoader()
{
	BOOL bRet = m_RkDevCtl.RebootRockusbDev();
	if (!bRet)
		AfxMessageBox(L"执行失败", MB_OK|MB_ICONSTOP);
}


void CSerialNumHelperDlg::CheckConfig()
{
	WCHAR wcPath[MAX_PATH] = {0};
	GetModuleFileNameW(AfxGetApp()->m_hInstance, wcPath, sizeof(wcPath));
	m_ConfigPath = wcPath;

	int pos = m_ConfigPath.ReverseFind('\\');
	m_ConfigPath = m_ConfigPath.Left(pos+1);
	m_ConfigPath.Append(CONFIG_INI);

	if (!PathFileExistsW(m_ConfigPath))
	{
		CreateConfig(m_ConfigPath);
	}
}

void CSerialNumHelperDlg::CreateConfig(CString path)
{
	HRSRC hrFile = FindResource(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_CONFIG1), L"CONFIG");
	if (hrFile == NULL)
		return;

	HGLOBAL hgFile = LoadResource(AfxGetResourceHandle(), hrFile);
	if (hgFile == NULL)
		return;

	LPVOID filgBuf = LockResource(hgFile);
	HANDLE hfFile = CreateFile(path, GENERIC_WRITE, 0,
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD nFileLen = SizeofResource(AfxGetResourceHandle(), hrFile);
	DWORD nWriteLen;
	WriteFile(hfFile, filgBuf, nFileLen, &nWriteLen, NULL);
	CloseHandle(hfFile);
	FreeResource(hgFile);
}

void CSerialNumHelperDlg::LoadConfig()
{
	CheckConfig();
	WCHAR checkSn[2], checkWifi[2], checkBt[2];

	GetPrivateProfileStringW(INI_SECTION_PATH, INI_KEY_PATH, L"",
		m_SNPath.GetBuffer(MAX_PATH), MAX_PATH, m_ConfigPath);
	GetPrivateProfileStringW(INI_SECTION_SETTING, INI_KEY_SN, L"N",
		checkSn, 2, m_ConfigPath);
	GetPrivateProfileStringW(INI_SECTION_SETTING, INI_KEY_WIFI, L"N",
		checkWifi, 2, m_ConfigPath);
	GetPrivateProfileStringW(INI_SECTION_SETTING, INI_KEY_BT, L"N",
		checkBt, 2, m_ConfigPath);
	m_SNPath.ReleaseBuffer();

	if (wcscmp(checkSn, L"N") == 0)
	{
		mCheckSN.SetCheck(BST_UNCHECKED);
		mEditSN.EnableWindow(FALSE);
	}
	else 
	{
		mCheckSN.SetCheck(BST_CHECKED);
	}

	if (wcscmp(checkWifi, L"N") == 0)
	{
		mCheckWIFI.SetCheck(BST_UNCHECKED);
		mEditWIFI.EnableWindow(FALSE);
	}
	else 
	{
		mCheckWIFI.SetCheck(BST_CHECKED);
	}

	if (wcscmp(checkBt, L"N") == 0)
	{
		mCheckBT.SetCheck(BST_UNCHECKED);
		mEditBT.EnableWindow(FALSE);
	}
	else 
	{
		mCheckBT.SetCheck(BST_CHECKED);
	}
}

void CSerialNumHelperDlg::SaveConfig()
{
	BOOL bCheckSN, bCheckWifi, bCheckBt;

	bCheckSN = mCheckSN.GetCheck();
	bCheckWifi = mCheckWIFI.GetCheck();
	bCheckBt = mCheckBT.GetCheck();

	WritePrivateProfileStringW(INI_SECTION_PATH, INI_KEY_PATH, m_SNPath, m_ConfigPath);
	WritePrivateProfileStringW(INI_SECTION_SETTING, INI_KEY_SN, bCheckSN ? L"Y" : L"N", m_ConfigPath);
	WritePrivateProfileStringW(INI_SECTION_SETTING, INI_KEY_WIFI, bCheckWifi ? L"Y" : L"N", m_ConfigPath);
	WritePrivateProfileStringW(INI_SECTION_SETTING, INI_KEY_BT, bCheckBt ? L"Y" : L"N", m_ConfigPath);
}

void CSerialNumHelperDlg::SetSnData(BYTE *dst, int &len, CString data)
{
	char cSn[31] = {0};

	UnicodeToAnsi(data.GetBuffer(), cSn);
	data.ReleaseBuffer();

	len = strlen(cSn);
	memcpy_s(dst, 30, cSn, len);
}

void CSerialNumHelperDlg::GetSnData(BYTE *dst, int len, CString &data)
{
	WCHAR wcSn[31] = {0};
	char cSn[31] = {0};

	strncpy_s(cSn, 31, (char*)dst, len);

	AnsiToUnicode(cSn, wcSn);
	data.Format(L"%s", wcSn);
}

void CSerialNumHelperDlg::SetWifiData(BYTE *dst, CString data)
{
	//A0-A1-A2-A3-A4-A5
	WCHAR wcWifi[13] = {0};
	char cWifi[13] = {0};

	data.Remove('-');
	wcsncpy_s(wcWifi, 13, data, data.GetLength());

	UnicodeToAnsi(wcWifi, cWifi);
	for (int i = 0; i < 6; i++)
	{
		*(dst+i) = D2H(cWifi[2*i])*16 + D2H(cWifi[2*i+1]);
	}
}

void CSerialNumHelperDlg::GetWifiData(BYTE *dst, CString &data)
{
	WCHAR wcWifi[13] = {0};
	char cWifi[13] = {0};

	Hex2Str(dst, 6, cWifi, sizeof(cWifi));
	AnsiToUnicode(cWifi, wcWifi);

	data.Format(L"%s", wcWifi);
	for (int i = 10; i > 0; i-=2)
	{
		data.Insert(i, L"-");
	}
}

void CSerialNumHelperDlg::SetBtData(BYTE *dst, CString data)
{
	SetWifiData(dst, data);
}

void CSerialNumHelperDlg::GetBtData(BYTE *dst, CString &data)
{
	GetWifiData(dst, data);
}

void CSerialNumHelperDlg::OnBnClickedCheckSn()
{
	// TODO: 在此添加控件通知处理程序代码
	if (mCheckSN.GetCheck() == BST_CHECKED)
	{
		mEditSN.EnableWindow(TRUE);
	}
	else
	{
		mEditSN.EnableWindow(FALSE);
	}
}


void CSerialNumHelperDlg::OnBnClickedCheckWifi()
{
	// TODO: 在此添加控件通知处理程序代码
	if (mCheckWIFI.GetCheck() == BST_CHECKED)
	{
		mEditWIFI.EnableWindow(TRUE);
	}
	else
	{
		mEditWIFI.EnableWindow(FALSE);
	}
}

void CSerialNumHelperDlg::OnBnClickedCheckBt()
{
	// TODO: 在此添加控件通知处理程序代码
	if (mCheckBT.GetCheck() == BST_CHECKED)
	{
		mEditBT.EnableWindow(TRUE);
	}
	else
	{
		mEditBT.EnableWindow(FALSE);
	}
}

BOOL CSerialNumHelperDlg::IsDeviceInserted()
{
	if (!m_bDevInserted)
	{
		AfxMessageBox(L"没有检测到设备", MB_OK|MB_ICONSTOP);
		return FALSE;
	}

	return TRUE;
}

BOOL CSerialNumHelperDlg::IsInLoaderMode()
{
	ENUM_RKUSB_TYPE type = m_RkDevCtl.GetDevType();
	if (type != RKUSB_LOADER)
		return FALSE;
	else
		return TRUE;
}

BOOL CSerialNumHelperDlg::CorrectInputChar(MSG *pMsg)
{
	HWND hWnd = pMsg->hwnd;
	char inCh = (char)pMsg->wParam;
	
	//debug_log(L"Input Char:%d", inCh);
	// ctrl+c, ctrl+v, back
	if (inCh == 3 || inCh == 8 || inCh == 22)
		return TRUE;

	if (hWnd == mEditSN.GetSafeHwnd())
	{
		return IsValidSnInput(inCh);
	}
	else if (hWnd == mEditWIFI.GetSafeHwnd())
	{
		return IsValidWifiInput(inCh);
	}
	else if (hWnd == mEditBT.GetSafeHwnd())
	{
		return IsValidBtInput(inCh);
	}

	return TRUE;
}

BOOL CSerialNumHelperDlg::IsValidSnInput(char c)
{
	if (IsCharAlphaA(c) || isdigit(c))
		return TRUE;
	else
		return FALSE;
}

BOOL CSerialNumHelperDlg::IsValidWifiInput(char c)
{
	if (c == '-')
		return TRUE;
	else if (isdigit(c))
		return TRUE;
	else if ('a' <= tolower(c) && tolower(c) <= 'f')
		return TRUE;

	return FALSE;
}

BOOL CSerialNumHelperDlg::CorrectWifiInput(CString strOld, CString strNew)
{
	// TODO 



	return TRUE;
}

BOOL CSerialNumHelperDlg::IsValidBtInput(char c)
{
	return IsValidWifiInput(c);
}

BOOL CSerialNumHelperDlg::CorrectBtInput(CString strOld, CString strNew)
{
	return CorrectWifiInput(strOld, strNew);
}

void CSerialNumHelperDlg::OnUpdateEditWifi()
{
	// TODO:  在此添加控件通知处理程序代码
	CString strWifi;
	mEditWIFI.GetWindowTextW(strWifi);
	CorrectWifiInput(mStrWifiRelTm, strWifi);
}

void CSerialNumHelperDlg::OnEnUpdateEditBt()
{
	// TODO:  在此添加控件通知处理程序代码
	CString strBt;
	mEditWIFI.GetWindowTextW(strBt);
	CorrectBtInput(mStrBTRelTm, strBt);
}

void CSerialNumHelperDlg::OnEnChangeEditSn()
{
	// TODO:  在此添加控件通知处理程序代码
	//debug_log(L"%s", L"SN Edit Changed");
	m_SNEditLastUpdate = GetTickCount();
	if (m_pSNEditUpdateThread == NULL)
	{	
		mExcEvent = EVENT_SNUPDATE;
		//debug_log(L"%s", L"Create SN Edit Update Thread");

		m_pSNEditUpdateThread = AfxBeginThread(ExecuteThread, this);
	}
}

void CSerialNumHelperDlg::SNEditUpdating()
{
	const DWORD TMSP = 600;
	CString strTxt;
	DWORD diff;

	do 
	{
		Sleep(200);
		diff = GetTickCount() - m_SNEditLastUpdate;
		
	} while (diff < TMSP);


	mEditSN.GetWindowTextW(strTxt);
	AutoImportSNFile(strTxt);
	m_pSNEditUpdateThread = NULL;

	//debug_log(L"%s", L"----Exit SN Edit Update Thread----");
}

void CSerialNumHelperDlg::AddProgressInfo(CString strInfo)
{
	CString strTxt = L"Msg:";

	strTxt += strInfo;
	mMsgList.AddString(strTxt);

	int count = mMsgList.GetCount();
	if (count > 0)
		mMsgList.SetTopIndex(count - 1);
}