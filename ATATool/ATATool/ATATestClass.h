#pragma once
#include "header\ATFunc.h"
#include "header\ATResult.h"
#include "header\ICommChannel.h"
#include "header\PhoneCommand.h"
#include "header\CommModule.h"
#include "header\ATResult.h"
#include "header\ATA_DLL.h"
#define ATTIMEOUT 1700
#define MAX_ITEMNUM 64
#define ATA_DLL_STOP_FLAG       0x9876


typedef struct
{
	unsigned char   szBTAddr[13];
	unsigned char   szWIFIAddr[13];
	char  szSwVer[256];
	char  szChipID[256];
}SWVERSION;

typedef struct
{
	S_ATADLL_COMPORT	com_port_info;
	int					bActive;
} S_COMPORT_ACTIVE_INFO;

typedef struct
{
	int     loop_times;
	int     fail_times[E_TEST_ITEM_COUNT];
} S_TESTITEM_LOOP_TEST_INFO;

const CString strCalFlag[] =
{
	_T("PCS AGC"),
	_T("PCS APC"),
	_T("DCS AGC"),
	_T("DCS APC"),
	_T("EGSM AGC"),
	_T("EGSM APC"),
	_T("GSM850 AGC"),
	_T("GSM850 APC"),
	_T("AFC"),
	_T("ADC"),
	_T("FINAL PCS"),
	_T("FINAL DCS"),
	_T("FINAL EGSM"),
	_T("FINAL GSM850"),
	_T("TD AFC"),
	_T("TD APC"),
	_T("TD AGC"),
	_T("FINAL TD"),
	_T("ANTENNA"),
	_T("CALI EDGE"),
	_T("FINAL EDGE"),
	_T("MMI CIT"),
	_T("WCDMA BANDI"),
	_T("WCDMA BANDII"),
	_T("WCDMA BANDV"),
	_T("WCDMA BANDVIII"),
	_T("WCDMA BANDIV"),
	_T("WCDMA FINAL TEST"),
	_T("LTE AFC TDD Calibration Flag"),
	_T("LTE AGC TDD Calibration Flag"),
	_T("LTE APC TDD Calibration Flag"),
	_T("LTE AFC FDD Calibration Flag"),
	_T("LTE AGC FDD Calibration Flag"),
	_T("LTE APC FDD Calibration Flag"),
	_T("LTE Final Test Flag"),
	_T("LTE Antenna Test Flag"),
	_T("LTE Final Test FDD Flag"),
	_T("LTE Antenna Test FDD Flag")
};

#define TEST_FLAG_W 	  (WCDMA_TESTFLAG_CALI_BANDI \
						|WCDMA_TESTFLAG_CALI_BANDII   \
						|WCDMA_TESTFLAG_CALI_BANDV   \
						|WCDMA_TESTFLAG_CALI_BANDVIII  )

#define TEST_FLAG_LTE 	 (CALI_FlAG_LTE_AGC_TDD  \
							|CALI_FLAG_LTE_APC_TDD    \
							|CALI_FlAG_LTE_AGC_FDD     \
							| CALI_FLAG_LTE_APC_FDD    )

#define TEST_FLAG_GSM  	(TEST_FLAG_CALI_PCS_AGC    \
						|TEST_FLAG_CALI_PCS_APC      \
						| TEST_FLAG_CALI_DCS_AGC      \
						|TEST_FLAG_CALI_DCS_APC  \
						|TEST_FLAG_CALI_EGSM_AGC  \
						|TEST_FLAG_CALI_EGSM_APC   \
						| TEST_FLAG_CALI_GSM850_AGC  \
						|TEST_FLAG_CALI_GSM850_APC    \
						| TEST_FLAG_CALI_AFC       )

class ATATestClass
{
public:
	~ATATestClass();
	ATATestClass();

public:
	S_ATADLL_COMMON_CFG_T	*CommonCFG;  //测试项的相关设置，如wifi名字，IMEI号等
	PVOID  g_hDiagPhone;
	int PelePort;
protected:
	int	ATAHandle;
	S_ATADLL_TEST_ITEM_T *itemcfg = new S_ATADLL_TEST_ITEM_T;
	S_ATADLL_TEST_ITEM_T	*TestItem;  //测试项
	S_ATADLL_TEST_CNF	TestCNF;  //测试结果	
	int PMPort = 163;
	CommModule *PMCom;
	E_ATDLL_TEST_ITEM_COUNT m_TestItem;
	S_ATADLL_TESTCASE_T  testcases_bg[MAX_ITEMNUM];
	S_ATADLL_TESTCASE_T  testcases_fg[MAX_ITEMNUM];
	int BGItem, FGItem;

	/*串口Begin*/
public:
	long InitComm(const char *portname, int flowctrl, int baud, int *stopFlag);
	long InitComm_Ex(const char *portname, const S_ATADLL_COMMON_CFG_T *comm_cfg);
	long InitComm_Ex_Ex(const char *portname, const S_ATADLL_COMMON_CFG_T *comm_cfg);
	void DeInitComm(long handle);
	void WriteComm(long handle, char* ATcommand);
	int preloader_com_port;
	/*串口End*/

	/*连接机子begin*/
public:
	bool ConnectTarget(int initState);
	void BreakupConnectTarget();
	bool ConnectPM();
	long GetPMCommModule();
	void InitConfig(int PortNum);
	HANDLE 	m_hEnterModeSuccess;
	HANDLE  m_hExitThread;
	/*连接机子end*/

	/*setup begin*/
protected:
	S_ATADLL_TEST_ITEM_T	ata_test_items;
	S_ATADLL_COMMON_CFG_T	ata_common_cfg;
	S_ATADLL_COMMON_CFG_T	ata_common_cfg1;
	S_ATADLL_COMMON_CFG_T	ata_common_cfg2;
	S_ATADLL_COMMON_CFG_T	ata_common_cfg3;
	S_ATADLL_COMMON_CFG_T	ata_common_cfg4;
	S_ATADLL_TESTITEM_SPEC	ata_test_spec;
	S_COMPORT_ACTIVE_INFO	comport_setting[4];
	S_TESTITEM_LOOP_TEST_INFO loop_test_info;
	/*setup end*/

	/*测试项begin*/
public:
	int RunIMEITest(CString & IMEI);
	int RunSWVersionTest(SWVERSION & SWVerVal);
	int RunGPSTest(int StartNum, int StartSignal, char(&Val)[256], unsigned long TimeOut);
	int RunWifiTest(CString ConnName, int & Rssi, CString & WifiName, unsigned long TimeOut1 = 15000, unsigned long TimeOut = 10000);
	int RunLedTest(bool & IsPass, int Port);
	int RunChargerTest(bool & IsPass);
	int RunRTCTest(bool & IsPass, int & SOSStatus);
	int RunLCMTest(bool & IsPass, int Port);
	int RunKeyPadTest(bool & IsPass, int Port);
	int RunSIMTest(bool & IsPass, int SIMNum = 4);
	int RunCorrectFlagTest(bool & IsPass);
	BOOL GetCalFlagMaskByIndexWCDMA(int nIndex);
	DWORD GetCalFlag(int nIndex);
	BOOL CheckOneCalFlag(DWORD dwValue, DWORD dwFlag, int nIndex);
	BOOL GetCalFlagMaskByIndexLTE(int nIndex);
	BOOL GetCalFlagMaskByIndex(int nIndex);
	int RunAudioTest(int Type, bool & IsPass, int Port, float & SnrVal);
	int TestAudio(char index, bool & IsSucceed, float & SNRVal, int Port);
	float SNR_Wave_Verification(double Freq, double SampleRate, int SampleNum, double * pReadWaveData);
	void dft(double * input, int N, double * output);
	double SNREstimation(double SampleRate, double Freq, int SampleNum, double * pSinwav);
	int RunTouchPanelTest(bool & IsPass);
	/*测试项end*/
};

