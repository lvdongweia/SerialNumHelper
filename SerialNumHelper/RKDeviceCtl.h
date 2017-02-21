
#ifndef _RK_DEVICE_CTL
#define _RK_DEVICE_CTL

typedef enum
{
	RKNONE_DEVICE=0,
	RK27_DEVICE=0x10,
	RKCAYMAN_DEVICE,
	RK28_DEVICE=0x20,
	RK281X_DEVICE,
	RKNANO_DEVICE=0x30,
	RKCROWN_DEVICE=0x40,
	RK29_DEVICE=0x50,
	RK292X_DEVICE,
	RK30_DEVICE=0x60,
	RK30B_DEVICE,
	RK31_DEVICE=0x70,
	RK32_DEVICE=0x80
}ENUM_RKDEVICE_TYPE;

typedef enum
{
	RKUSB_NONE=0x0,
	RKUSB_MASKROM=0x01,   // adb
	RKUSB_LOADER=0x02,    // loader
	RKUSB_MSC=0x04        // msc
}ENUM_RKUSB_TYPE;


typedef enum
{
	TESTDEVICE_PROGRESS,			//�豸���Խ���
	DOWNLOADIMAGE_PROGRESS,			//Image�ļ����ؽ���
	CHECKIMAGE_PROGRESS,			//Image��Ч�Լ�����
	TAGBADBLOCK_PROGRESS,			//Flash�����ǽ���
	TESTBLOCK_PROGRESS,				//Flash�������
	ERASEFLASH_PROGRESS				//Flash��������
}ENUM_PROGRESS_PROMPT;

typedef enum
{
	CALL_FIRST,						//��һ�ε���
	CALL_MIDDLE,					//�м����
	CALL_LAST						//���һ�ε���
}ENUM_CALL_STEP;


typedef enum
{
	ERR_NONE			= 1,
	ERR_FAIL			= 0,
	ERR_BUF_LENGTH      = -1,
	ERR_BUF_SMALL		= -2,
	ERR_BUF_LARGE		= -3,
	ERR_INVALID_ADDR	= -4,

	ERR_UNKNOW			= -10,
}ENUM_ERROR_CODES;


typedef struct  
{
	int snLen;
	BOOL bSn;
	BYTE sn[30];
	BOOL bWifi;
	BYTE wifi[6];
	BOOL bBt;
	BYTE bt[6];
}CUSTOM_DATA;

class CRKDeviceCtl
{
public:
	CRKDeviceCtl(void);
	~CRKDeviceCtl(void);


	BOOL IsRockUsbDevPlugged();
	BOOL Initialize();
	BOOL UnInitialize();
	BOOL ScanDevice();
	ENUM_RKUSB_TYPE GetDevType();
	BOOL SwtichToLoaderMode();
	int GetDevCounts();
	BOOL RebootRockusbDev();
	int WriteSN(PBYTE pSN, int nSNLen);
	int ReadSN(PBYTE pSN, int &nSNLen);
	
	//Wifi��ַд�뵽mac��ַ������
	int WriteWIFI(PBYTE pWifi, int nWifiLen=6);
	int ReadWIFI(PBYTE pWifi, int &nWifiLen);
	int WriteBT(PBYTE pBT, int nBTLen=6);
	int ReadBT(PBYTE pBT, int &nBTLen);

	int ReadAllInfo(CUSTOM_DATA &data);
	int WriteAllInfo(CUSTOM_DATA *data);

private:
	
	int WriteMAC(PBYTE pMac, int nMacLen=6); 
	int ReadMAC(PBYTE pMac, int &nMacLen);

};

#endif /* _RK_DEVICE_CTL */