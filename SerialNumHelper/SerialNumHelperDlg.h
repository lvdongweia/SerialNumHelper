
// SerialNumHelperDlg.h : 头文件
//
#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "RKDeviceCtl.h"

typedef enum
{
	STATE_NONE = 0,
	STATE_PASS,
	STATE_FAIL
}TEST_STATE;

typedef enum
{
	EVENT_NONE   = 0,
	EVENT_IMPORT,
	EVENT_SWITCH,
	EVENT_REBOOT,
	EVENT_WRITE,
	EVENT_READ,
	EVENT_SNUPDATE,
}EXECUTE_EVENT;


// CSerialNumHelperDlg 对话框
class CSerialNumHelperDlg : public CDialogEx
{
// 构造
public:
	CSerialNumHelperDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CSerialNumHelperDlg();

// 对话框数据
	enum { IDD = IDD_SERIALNUMHELPER_DIALOG };

	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	CStatic mStaticDev;
	CStatic mStaticResult;
	CEdit mEditSN;
	CEdit mEditWIFI;
	CString mStrWifiRelTm;
	CEdit mEditBT;
	CString mStrBTRelTm;
	CButton mCheckSN;
	CButton mCheckWIFI;
	CButton mCheckBT;
	CButton mBtnImport;
	CButton mBtnSwitch;
	CButton mBtnReboot;
	CButton mBtnWrite;
	CButton mBtnRead;
	CListBox mMsgList;

	void HandleEvent();
	void SNEditUpdating();
	EXECUTE_EVENT mExcEvent;
	
private:
	CFont m_ComFont;
	CFont m_ResultFont;
	CFont m_DevFont;
	CFont m_SmallFont;
	TEST_STATE m_ExcResult;
	CBrush m_PassBrush;
	CBrush m_FailBrush;
	CBrush m_NoneBrush;

	CString m_ConfigPath;
	CString m_SNPath;
	BOOL m_bDevInserted;
	BOOL m_bIsSwitching;

	CWinThread *m_pSNEditUpdateThread;
	DWORD m_SNEditLastUpdate;

	CRKDeviceCtl m_RkDevCtl;

	void InitUI();
	void SetExcResult(TEST_STATE state);
	void SetDevStatus(ENUM_RKUSB_TYPE type);
	void EnableUIWindows(BOOL bEnable);
	BOOL IsDeviceInserted();
	BOOL IsInLoaderMode();
	void WriteSerialNums();
	void ReadSerialNums();
	BOOL SwitchToLoader();
	void ExitLoader();
	
	void SetSnData(BYTE *dst, int &len, CString data);
	void GetSnData(BYTE *dst, int len, CString &data);
	void SetWifiData(BYTE *dst, CString data);
	void GetWifiData(BYTE *dst, CString &data);
	void SetBtData(BYTE *dst, CString data);
	void GetBtData(BYTE *dst, CString &data);

	void LoadConfig();
	void SaveConfig();
	BOOL CorrectInputChar(MSG *pMsg);
	BOOL CorrectWifiInput(CString strOld, CString strNew);
	BOOL CorrectBtInput(CString strOld, CString strNew);
	BOOL IsValidSnInput(char c);
	BOOL IsValidWifiInput(char c);
	BOOL IsValidBtInput(char c);

	void AddProgressInfo(CString strInfo);
	
	void SelectImportSNFile();
	void AutoImportSNFile(CString strName);
	void ImportSNFile(CString path);
	void CreateConfig(CString path);
	void CheckConfig();
	CString GetFileEncode(CString path);

// 实现
protected:
	HICON m_hIcon;
	HACCEL m_hAccel;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnDeviceChange(UINT nEventType, DWORD_PTR dwData);

	afx_msg void OnClose();
	afx_msg void OnBnClickedImport();
	afx_msg void OnBnClickedSwitch();
	afx_msg void OnBnClickedReboot();
	afx_msg void OnBnClickedWrite();
	afx_msg void OnBnClickedRead();
	afx_msg void OnBnClickedCheckSn();
	afx_msg void OnBnClickedCheckWifi();
	afx_msg void OnBnClickedCheckBt();
	afx_msg void OnUpdateEditWifi();
	afx_msg void OnEnUpdateEditBt();
	afx_msg void OnEnChangeEditSn();

	DECLARE_MESSAGE_MAP()
};
