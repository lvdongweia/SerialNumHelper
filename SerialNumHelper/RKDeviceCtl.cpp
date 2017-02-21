#include "StdAfx.h"
#include "RKDeviceCtl.h"

#include "dll/RKUpgradeDll.h"

const UINT ROCKUSB_VID = 0x2207;
const UINT ROCKUSB_PID = 0x320A;
const UINT ROCKUSB_TMOUT = 20;
const UINT ROCKMSC_VID = 0x2207;//0x0BB4;
const UINT ROCKMSC_PID = 0x0010;//0x0C02;
const UINT ROCKMSC_TMOUT = 20;



CRKDeviceCtl::CRKDeviceCtl(void)
{
}


CRKDeviceCtl::~CRKDeviceCtl(void)
{
}


BOOL CRKDeviceCtl::IsRockUsbDevPlugged()
{
	return RK_IsRockUsbPlugged();
}

BOOL CRKDeviceCtl::Initialize()
{
	INIT_DEV_INFO dev_info;
	INIT_LOG_INFO log_info;
	INIT_CALLBACK_INFO callback_info;

	dev_info.bScan4FsUsb = TRUE;
	dev_info.emSupportDevice = RK32_DEVICE;
	dev_info.uiRockMscTimeout = ROCKMSC_TMOUT;
	dev_info.uiRockusbTimeout = ROCKUSB_TMOUT;
	dev_info.usRockMscPid = ROCKMSC_PID;
	dev_info.usRockMscVid = ROCKMSC_PID;
	dev_info.usRockusbPid = ROCKUSB_PID;
	dev_info.usRockusbVid = ROCKUSB_VID;

	log_info.bLogEnable = FALSE;
	log_info.lpszLogPathName = L"";

	callback_info.pProgressPromptProc = NULL;
	callback_info.pUpgradeStepPromptProc = NULL;

	return RK_Initialize(dev_info, log_info, callback_info);
}

BOOL CRKDeviceCtl::UnInitialize()
{
	return RK_Uninitialize();
}

BOOL CRKDeviceCtl::ScanDevice()
{
	UINT devCount;
	BOOL bExistMsc;
	BOOL bExistAdb;
	return RK_ScanDevice(devCount, bExistMsc, bExistAdb);
}

ENUM_RKUSB_TYPE CRKDeviceCtl::GetDevType()
{
	BOOL bSuccess;
	UINT devCount;
	BOOL bExistMsc;
	BOOL bExistAdb;

	bSuccess = RK_ScanDevice(devCount, bExistMsc, bExistAdb);
	if (!bSuccess || devCount == 0)
		return RKUSB_NONE;

	if (bExistAdb) 
	{
		return RKUSB_MASKROM;
	}
	else if (bExistMsc)
	{
		return RKUSB_MSC;
	}
	else
	{
		return RKUSB_LOADER;
	}
}

BOOL CRKDeviceCtl::SwtichToLoaderMode()
{
	/*ENUM_RKUSB_TYPE type = GetDevType();

	if (type == RKUSB_NONE)
	{
		return FALSE;
	}
	else if (type == RKUSB_MASKROM)
	{
		return RK_AdbToRockusb();
	}
	else if (type == RKUSB_MSC) 
	{
		return RK_SwitchToRockusb();
	}*/

	return RK_AdbToRockusb();

	return TRUE;
}

int CRKDeviceCtl::GetDevCounts()
{
	DWORD layerArray[5] = {0};
	DWORD usbTypeArray[5] = {0};
	DWORD deviceTypeArray[5] = {0};

	//ScanDevice();
	return RK_GetDeviceInfo(layerArray, usbTypeArray, deviceTypeArray, 5);
}

BOOL CRKDeviceCtl::RebootRockusbDev()
{
	return RK_ResetRockusb();
}

int CRKDeviceCtl::ReadSN(PBYTE pSN, int &nSNLen)
{
	BOOL bSuccess;

	if (pSN == NULL)
		return ERR_FAIL;

	if (nSNLen < 30)
		return ERR_BUF_SMALL;

	bSuccess = RK_ReadSN(pSN, nSNLen);
	return bSuccess ? ERR_NONE : ERR_FAIL;
}

int CRKDeviceCtl::WriteSN(PBYTE pSN, int nSNLen)
{
	BOOL bSuccess;

	if (pSN == NULL)
		return ERR_FAIL;

	if (nSNLen > 30)
		return ERR_BUF_LARGE;

	bSuccess = RK_WriteSN(pSN, nSNLen);
	return bSuccess ? ERR_NONE : ERR_FAIL;
}

int CRKDeviceCtl::WriteMAC(PBYTE pMac, int nMacLen/*=6*/)
{
	BOOL bSuccess;

	if (pMac == NULL)
		return ERR_FAIL;

	if (nMacLen != 6)
		return ERR_BUF_LENGTH;

	bSuccess = RK_WriteMAC(pMac, nMacLen);
	return bSuccess ? ERR_NONE : ERR_FAIL;
}

int CRKDeviceCtl::ReadMAC(PBYTE pMac, int &nMacLen)
{
	BOOL bSuccess;

	if (pMac == NULL)
		return ERR_FAIL;

	if (nMacLen != 6)
		return ERR_BUF_LENGTH;

	bSuccess = RK_ReadMAC(pMac, nMacLen);
	return bSuccess ? ERR_NONE : ERR_FAIL;
}

int CRKDeviceCtl::WriteWIFI(PBYTE pWifi, int nWifiLen/*=6*/)
{
	if (pWifi == NULL)
		return ERR_FAIL;

	// Wifi地址首字节必须为偶数
	BYTE first = *pWifi;
	if ((first % 2) != 0)
		return ERR_INVALID_ADDR;

	return WriteMAC(pWifi, nWifiLen);
}

int CRKDeviceCtl::ReadWIFI(PBYTE pWifi, int &nWifiLen)
{
	return ReadMAC(pWifi, nWifiLen);
}

int CRKDeviceCtl::WriteBT(PBYTE pBT, int nBTLen/*=6*/)
{
	BOOL bSuccess;

	if (pBT == NULL)
		return ERR_FAIL;

	if (nBTLen != 6)
		return ERR_BUF_LENGTH;

	bSuccess = RK_WriteBT(pBT, nBTLen);
	return bSuccess ? ERR_NONE : ERR_FAIL;
}

int CRKDeviceCtl::ReadBT(PBYTE pBT, int &nBTLen)
{
	BOOL bSuccess;

	if (pBT == NULL)
		return ERR_FAIL;

	if (nBTLen != 6)
		return ERR_BUF_LENGTH;

	bSuccess = RK_ReadBT(pBT, nBTLen);
	return bSuccess ? ERR_NONE : ERR_FAIL;
}

/* 自定义区域一共512字节
0-1: SN Size
2-31: SN Data
32-450:Reserved
451:IMEI Size (length:15)
452-466:IMEI Data
467:UID Size (length:30)
468-497:UID Data
498:BT Size (length:6)
499-504:BT Data
505:Mac Size (length:6)
506-511:Mac Data
*/
int CRKDeviceCtl::ReadAllInfo(CUSTOM_DATA &data)
{
	BOOL bSuccess;
	BYTE buf[512] = {0};

	bSuccess = RK_ReadCustomData(buf, sizeof(buf), 0);
	if (!bSuccess)
		return ERR_FAIL;

	ZeroMemory(&data, sizeof(CUSTOM_DATA));
	data.snLen = buf[0];
	memcpy(data.sn, buf+2, data.snLen);
	memcpy(data.bt, buf+499, 6);
	memcpy(data.wifi, buf+506, 6);

	return ERR_NONE;
}

int CRKDeviceCtl::WriteAllInfo(CUSTOM_DATA *data)
{
	BOOL bSuccess;

	if (data == NULL) 
		return ERR_FAIL;

	BYTE buf[512] = {0};
	bSuccess = RK_ReadCustomData(buf, sizeof(buf), 0);
	if (!bSuccess)
		return ERR_FAIL;

	if (data->bSn)
	{
		buf[0] = data->snLen;
		memcpy(buf+2, data->sn, data->snLen);
	}
	if (data->bBt)
	{
		buf[498] = 0x06;
		memcpy(buf+499, data->bt, 6);
	}
	if (data->bWifi)
	{
		buf[505] = 0x06;
		memcpy(buf+506, data->wifi, 6);
	}

	bSuccess = RK_WriteCustomData(buf, 0, 512);
	return bSuccess ? ERR_NONE : ERR_FAIL;
}


