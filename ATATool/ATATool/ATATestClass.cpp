#include "stdafx.h"
#include "ATATestClass.h"


#pragma comment (lib,".\\lib\\ATDLL.lib")  //导入串口库
#pragma comment (lib,".\\lib\\PhoneCommand.lib")
#pragma comment (lib,".\\lib\\Release.lib")

ATATestClass::ATATestClass()
{
	//ATADLL_GetAvailbleHandle(&ATAHandle);
	g_hDiagPhone = SP_CreatePhone(NULL);
	m_hEnterModeSuccess = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hExitThread = CreateEvent(NULL, TRUE, FALSE, NULL);
	TestItem = new S_ATADLL_TEST_ITEM_T;
	ATADLL_GetAvailbleHandle(&ATAHandle);
}


ATATestClass::~ATATestClass()
{
	ATADLL_ReleaseHandle(ATAHandle);
}

long ATATestClass::InitComm(const char * portname, int flowctrl, int baud, int * stopFlag)
{
	return ATInitCommPort(portname, flowctrl, baud, stopFlag);
}

long ATATestClass::InitComm_Ex(const char * portname, const S_ATADLL_COMMON_CFG_T * comm_cfg)
{
	return ATInitCommPort_Ex(portname, comm_cfg);
}

long ATATestClass::InitComm_Ex_Ex(const char * portname, const S_ATADLL_COMMON_CFG_T * comm_cfg)
{
	return ATInitCommPort_Ex_Ex(portname, comm_cfg);
}

void ATATestClass::DeInitComm(long handle)
{
	ATDeInitCommPort(handle);
}

void ATATestClass::WriteComm(long handle, char * ATcommand)
{
	ATWriteCommand(handle, ATcommand);
}

//连接机子,需在线程中调用，否则WaitForMultipleObjects会导致程序不响应消息
bool ATATestClass::ConnectTarget(int initState)
{
	long lCommMod = 0;
	CommModule *cm = NULL;

	DWORD dwRet = 0;
	DWORD startTempTime, endTempTime;
	float tempTime;
	CHANNEL_ATTRIBUTE   ca;
	ca.ChannelType = CHANNEL_TYPE_USBMON;
	SP_EndPhoneTest(g_hDiagPhone);                       //先结束当前连接

	if (!_SPOK(SP_BeginPhoneTest(g_hDiagPhone, &ca)))    //开始一个新的连接
	{
		return false;
	}
	ResetEvent(m_hExitThread);
	ResetEvent(m_hEnterModeSuccess);

	RM_MODE_ENUM eMode = RM_AUTOTEST_MODE;

	SP_EnterModeProcess(g_hDiagPhone, true, preloader_com_port, eMode, m_hEnterModeSuccess, 0);   //指定测试模式

	HANDLE hHandle[] = { m_hExitThread,m_hEnterModeSuccess };

	dwRet = WaitForMultipleObjects(2, hHandle, FALSE, INFINITE);  //66:ERROR_BAD_DEV_TYPE      等待两个时间触发，只要某一个触发，就会向下执行

	if (WAIT_TIMEOUT == dwRet)
	{
		SP_StopModeProcess(g_hDiagPhone);   //指定测试模式超时的时候，停止
		return false;
	}
	else if (0 == dwRet - WAIT_OBJECT_0)  //用户停止
	{
		SP_StopModeProcess(g_hDiagPhone);
		ResetEvent(m_hExitThread);
		return false;
	}
	else if (1 == dwRet - WAIT_OBJECT_0)
	{
		dwRet = SP_GetUsbPort(g_hDiagPhone);                     //指定模式成功，获取usb端口
		ResetEvent(m_hEnterModeSuccess);
		if (0)
		{
			char bySend[6];
			DEVICE_AUTOTEST_ID_E eDeviceCmd = DEVICE_AUTOTEST_WIFI;
			Sleep(3000);//wait bbat
			eDeviceCmd = DEVICE_AUTOTEST_KEYPAD;
			bySend[0] = 1; //按键检测open。
			dwRet = SP_AutoBBTest(g_hDiagPhone, eDeviceCmd, (const char*)bySend, 1, NULL, 0);
			if (0 != dwRet)
			{
				AfxMessageBox(_T("自动测试通路检查1Fail !!\n"));
				return false;
			}
			else
			{
				eDeviceCmd = DEVICE_AUTOTEST_BT;
				bySend[0] = 2; //按键检测open。
				dwRet = SP_AutoBBTest(g_hDiagPhone, eDeviceCmd, (const char*)bySend, 1, NULL, 0);
				if (0 == dwRet) //这一步必须为fail，才证明autotest起作用了
				{
					AfxMessageBox(_T("自动测试通路检查2Fail !!\n"));
				}
				else
				{
					AfxMessageBox(_T("自动测试通路检查OK\n"));
				}
			}
		}
	}
	return true;
}

//中断连接
void ATATestClass::BreakupConnectTarget()
{
	SetEvent(m_hExitThread);
	ResetEvent(m_hExitThread);
}

//连接pm，发送start指令
bool ATATestClass::ConnectPM()
{
	long lpmCommMod = 0;
	CommModule *pmcm = NULL;

	lpmCommMod = GetPMCommModule();   //初始化串口
	if (lpmCommMod <= 0)
	{
		return false;
	}
	pmcm = (CommModule *)lpmCommMod;
	pmcm->SetCallBack(NULL);
	PMCom = pmcm;
	ATResult atret;
	string atstr = "TestStart\r\n";
	PMCom->SetNeedWithCRLR(true);
	PMCom->WriteCommand(atstr.c_str(), atstr.length(), ATTIMEOUT, false, atret);
	PMCom->SetNeedWithCRLR(false);
	return true;
}

//初始化串口
long ATATestClass::GetPMCommModule()
{
	int com_port = 163;
	if (com_port <= 0) com_port = 1000;

	CString cstr;
	cstr.Format("\\\\.\\COM%d", com_port);
	return ATInitCommPort_Ex_Ex((LPCTSTR)cstr, CommonCFG);
}

//初始化设置
void ATATestClass::InitConfig(int PortNum)
{
	PelePort = PortNum;

	memset(&ata_test_items, 0x0, sizeof(S_ATADLL_TEST_ITEM_T));
	memset(&ata_common_cfg, 0x0, sizeof(S_ATADLL_COMMON_CFG_T));
	memset(&ata_common_cfg1, 0x0, sizeof(S_ATADLL_COMMON_CFG_T));
	memset(&ata_common_cfg2, 0x0, sizeof(S_ATADLL_COMMON_CFG_T));
	memset(&ata_common_cfg3, 0x0, sizeof(S_ATADLL_COMMON_CFG_T));
	memset(&ata_common_cfg4, 0x0, sizeof(S_ATADLL_COMMON_CFG_T));
	memset(&ata_test_spec, 0x0, sizeof(S_ATADLL_TESTITEM_SPEC));
	memset(comport_setting, 0x0, sizeof(S_COMPORT_ACTIVE_INFO) * 4);

	memset(&loop_test_info, 0x0, sizeof(S_TESTITEM_LOOP_TEST_INFO));
	loop_test_info.loop_times = 1;

	char SetupDir[512];
	GetCurrentDirectory(512, SetupDir);
	sprintf_s(SetupDir, "%s\\Setup.ini", SetupDir);
	CFile file;
	if (!file.Open(SetupDir, CFile::modeReadWrite))
		return;
	file.SeekToBegin();

	int bufSize = 0;
	char buffer[10240];

	file.Read(&bufSize, sizeof(int));
	file.Read(buffer, bufSize);
	buffer[bufSize] = 0;
	memcpy(&ata_test_items, buffer, bufSize);

	file.Read(&bufSize, sizeof(int));
	file.Read(buffer, bufSize);
	buffer[bufSize] = 0;
	memcpy(&ata_common_cfg, buffer, bufSize);

	file.Read(&bufSize, sizeof(int));
	file.Read(buffer, bufSize);
	buffer[bufSize] = 0;
	memcpy(&ata_test_spec, buffer, bufSize);

	file.Read(&bufSize, sizeof(int));
	file.Read(buffer, bufSize);
	buffer[bufSize] = 0;
	memcpy(&comport_setting, buffer, bufSize);

	file.Close();

	//set timeout
	for (E_ATDLL_TEST_ITEM_COUNT index = E_LCD; index < E_TEST_ITEM_COUNT; index = (E_ATDLL_TEST_ITEM_COUNT)((int)index + 1))
	{
#ifdef __ATA30__
		ata_common_cfg.timeout[index] = 120;
#else
		ata_common_cfg.timeout[index] = 60;
		if (index == E_BT)
			ata_common_cfg.timeout[index] = 90;
		if (index == E_GPS)
			ata_common_cfg.timeout[index] = 90;
		if (index == E_KEYPAD || index == E_TOUCHSCR || index == E_CMMB)
			ata_common_cfg.timeout[index] = 20;
		if (index == E_CHARGING)
			ata_common_cfg.timeout[index] = 60;
		if (index == E_RSSI)
			ata_common_cfg.timeout[index] = 120;
		if (index == E_CAMERA)
			ata_common_cfg.timeout[index] = 60;
		if (index == E_SIM)
			ata_common_cfg.timeout[index] = 40;
		if (index == E_HDMI)
			ata_common_cfg.timeout[index] = 60;
#endif
	}

	if (comport_setting[0].bActive)
	{
		memcpy(&ata_common_cfg1, &ata_common_cfg, sizeof(S_ATADLL_COMMON_CFG_T));
		memcpy(&ata_common_cfg1.com_port_info, &comport_setting[0].com_port_info, sizeof(S_ATADLL_COMPORT));
		sprintf_s(ata_common_cfg1.MainCameraFilePath, "MainCamera1.jpg");
		sprintf_s(ata_common_cfg1.AHDCameraFilePath, "AHDCamera1.jpg");
	}

	CommonCFG = &ata_common_cfg1;
	CommonCFG->com_port_info.preloader_com_port = PortNum;
	CommonCFG->com_port_info.usb_enable = true;
	CommonCFG->stop_flag = new int;
	TestItem = &ata_test_items;
	//*(CommonCFG->stop_flag) = ATA_DLL_STOP_FLAG;
	ATADLL_GetTestCase_BG(ATAHandle, TestItem, testcases_bg, &BGItem);
	ATADLL_GetTestCase_FG(ATAHandle, TestItem, testcases_fg, &FGItem);
}


//测试项
int ATATestClass::RunIMEITest(CString &IMEI)
{
	int ret = 0;
	FASTNVACCESS_T nv_data;
	DWORD dwRwMask = 0;
	ZeroMemory((void *)&nv_data, sizeof(nv_data));
	unsigned long ulRecv = 0;
	char szSend[512] = { 0 }, szRecv[512] = { 0 };
	int iOK;
	string strReturn;
	dwRwMask = (DWORD)(FNAMASK_WRITE_IMEI1);
	nv_data.dwOperMask = dwRwMask;
	ret = SP_FastReadNvInfo(g_hDiagPhone, &nv_data);
	if (ret != 0)
	{
		IMEI = "";
		return ret;
	}
	else
	{
		CString IMEI1(nv_data.szImei1);
		IMEI = IMEI1;
		return ret;
	}
}

int ATATestClass::RunSWVersionTest(SWVERSION &SWVerVal) //bt，version
{
	int nStatus = 1;
	char szSwVer[256] = { 0 };
	char szChipID[256] = { 0 };
	ATResult atret;
	char param[] = { 0x00,0x00,0x00 };
	if (nStatus = SP_AutoBBTest(g_hDiagPhone, DEVICE_AUTOTEST_SET, param, 1, szSwVer, 128) != 0)
	{
		return nStatus;
	}
	memcpy(SWVerVal.szSwVer, szSwVer, 256);
	DWORD dwRwMask = 0;
	FASTNVACCESS_T nv_data;
	dwRwMask = (DWORD)(FNAMASK_WRITE_WIFIADDR | FNAMASK_WRITE_BTADDR);
	nv_data.dwOperMask = dwRwMask;
	if (nStatus = SP_FastReadNvInfo(g_hDiagPhone, &nv_data) != 0)
	{
		return nStatus;
	}
	memcpy(SWVerVal.szBTAddr, nv_data.szBTAddr, 13);
	memcpy(SWVerVal.szWIFIAddr, nv_data.szWIFIAddr, 13);
	if (nStatus = SP_AutoBBTest(g_hDiagPhone, DEVICE_AUTOTEST_CHIPID, param, 1, szChipID, 128) != 0)
	{
		return nStatus;
	}
	memcpy(SWVerVal.szChipID, szChipID, 256);
	return nStatus;
}

int ATATestClass::RunGPSTest(int StartNum, int StartSignal, char(&Val)[256], unsigned long TimeOut)
{
	char bySend[36];
	int nStatus, sat_number = 0;
	int m, n, j;

	bySend[0] = 2;
	nStatus = SP_AutoBBTest(g_hDiagPhone, DEVICE_AUTOTEST_GPS, (const char*)bySend, 1, NULL, 0);

	if (0 != nStatus)
	{
		return nStatus;
	}

	char svnnumber[256] = { 0 };
	char param[] = { 5,StartNum,StartSignal };
	nStatus = SP_AutoBBTest(g_hDiagPhone, DEVICE_AUTOTEST_GPS,
		(const char*)param, 3, svnnumber, 128, TimeOut);

	if (nStatus < 0)
	{
		return nStatus;
	}

	sat_number = svnnumber[0] > 10 ? 10 : svnnumber[0];

	int k;
	for (m = 1; m < sat_number; m++)   //signal 从大到小排序
	{
		for (n = m + 1; n < sat_number + 1; n++)
		{

			if (svnnumber[m] > svnnumber[n])
			{
				k = svnnumber[m];
				svnnumber[m] = svnnumber[n];
				svnnumber[n] = k;
			}
		}
	}
	memcpy(Val, svnnumber, 256);
	return nStatus;
}

int ATATestClass::RunWifiTest(CString ConnName, int &Rssi, CString &WifiName, unsigned long TimeOut1, unsigned long TimeOut2)//只比较信号
{
	char bySend[36];
	int nStatus = 0;
	char wifiname[32];
	memcpy(wifiname, ConnName.GetBuffer(), strlen(ConnName) + 1);
	DEVICE_AUTOTEST_ID_E eDeviceCmd = DEVICE_AUTOTEST_WIFI;
	bySend[0] = 1;
	memcpy(bySend + 1, wifiname, 32);


	nStatus = SP_AutoBBTest(g_hDiagPhone, eDeviceCmd, (const char*)bySend, 36, NULL, 0, TimeOut1);  //打开wifi，发送热点名称到机子
	if (0 != nStatus)
	{
		return nStatus;
	}

	int retry = 2;
	do {
		bySend[0] = 2;
		nStatus = SP_AutoBBTest(g_hDiagPhone, DEVICE_AUTOTEST_WIFI, (const char*)bySend, 1, NULL, 0, TimeOut2);  //搜索热点

		if (0 == nStatus)
			break;
		Sleep(4500);
		retry--;
	} while ((0 != nStatus) && (retry > 0));
	if (0 != nStatus)
	{
		return nStatus;
	}

	char szWIFIname[256] = { 0 };
	bySend[0] = 3;
	nStatus = SP_AutoBBTest(g_hDiagPhone, DEVICE_AUTOTEST_WIFI, (const char*)bySend, 1, szWIFIname, 128, TimeOut2);  //szWIFIname保存热点名称和信号强度

	if (nStatus < 0)
	{
		return nStatus;
	}


	CString wifiresult = szWIFIname;

	int number2 = wifiresult.Find(':');
	int number1 = wifiresult.Find(',');
	CString output1 = wifiresult.Mid(number2 + 2, number1);
	int number3 = output1.Find(',');
	CString output3 = output1.Mid(0, number3);  //output3  wifi名字
	CString output2 = wifiresult.Mid(number1 + 1, strlen(wifiresult));  //output2  信号强度

	Rssi = atoi(output2);
	WifiName = output3;
	return nStatus;
}

int ATATestClass::RunLedTest(bool &IsPass, int Port)  //人工判断
{
	int nStatus = 0;
	int ret;

	char param2[] = { 0x00 };
	nStatus = SP_AutoBBTest(g_hDiagPhone, DEVICE_AUTOTEST_LED, param2, 1, NULL, 0);

	if (0 != nStatus)
	{
		return nStatus;
	}
	CString show;
	show.Format("请确认端口%d的LED灯是否同时闪亮?",PelePort);
	ret = ::MessageBox(NULL, show, "测试结果", MB_YESNO | MB_SYSTEMMODAL);
	if (ret == IDYES)
	{
		IsPass = true;
	}
	else
	{
		IsPass = false;
	}
	return nStatus;
}

int ATATestClass::RunChargerTest(bool &IsPass)
{
	int nErrorCode;
	char param[] = { 0x00 };

	IsPass = true;

	param[0] = 0x01;
	nErrorCode = SP_AutoBBTest(g_hDiagPhone, DEVICE_AUTOTEST_CHARGE, param, 1, NULL, 0);
	if (nErrorCode != 0)
	{
		IsPass = false;
		return nErrorCode;
	}

	param[0] = 0x02;
	nErrorCode = SP_AutoBBTest(g_hDiagPhone, DEVICE_AUTOTEST_CHARGE, param, 1, NULL, 0);
	if (nErrorCode != 0)
	{
		IsPass = false;
		return nErrorCode;
	}

	param[0] = 0x03;
	nErrorCode = SP_AutoBBTest(g_hDiagPhone, DEVICE_AUTOTEST_CHARGE, param, 1, NULL, 0);
	if (nErrorCode != 0)
	{
		IsPass = false;
		return nErrorCode;
	}

	return nErrorCode;
}

int ATATestClass::RunRTCTest(bool &IsPass, int &SOSStatus)
{
	int nStatus = 0;
	char Rtcstate[] = { 0xff };
	char param[] = { 0x00 };


	nStatus = SP_AutoBBTest(g_hDiagPhone, DEVICE_AUTOTEST_RTC, param, 1, NULL, 0);
	if (nStatus 
< 0)
	{
		IsPass = false;
		return nStatus;
	}

	param[0] = 3;
	nStatus = SP_AutoBBTest(g_hDiagPhone, DEVICE_AUTOTEST_RTC, param, 1, Rtcstate, 1);
	if (nStatus < 0)
	{
		IsPass = false;
		return nStatus;
	}

	param[0] = 4;
	nStatus = SP_AutoBBTest(g_hDiagPhone, DEVICE_AUTOTEST_RTC, param, 1, NULL, 0);
	if (nStatus < 0)
	{
		IsPass = false;
		return nStatus;
	}

	if (Rtcstate[0] < 0) 
	{
		SOSStatus = Rtcstate[0];
		IsPass = false;
		return nStatus;
	}
	else 
	{
		SOSStatus = Rtcstate[0];
		IsPass = true;
		return nStatus;
	}
	return nStatus;
}

int ATATestClass::RunLCMTest(bool &IsPass,int Port)   //人工判断
{
	int nStatus = 0;
	char param[] = { 0x00 };
	ATResult atret;
	int ret;

	param[0] = 17;
	nStatus = SP_AutoBBTest(g_hDiagPhone, DEVICE_AUTOTEST_LCD_PARALLEL, param, 1, NULL, 0);
	if (nStatus < 0)
	{
		IsPass = false;
		return nStatus;
	}

	param[0] = 18;
	nStatus = SP_AutoBBTest(g_hDiagPhone, DEVICE_AUTOTEST_LCD_PARALLEL, param, 1, NULL, 0);
	if (nStatus < 0)
	{
		IsPass = false;
		return nStatus;
	}
	CString show;
	show.Format("请确认端口%d的屏显示是否正确?",Port);
	ret = ::MessageBox(NULL, show, "测试结果", MB_YESNO | MB_SYSTEMMODAL);
	if (ret == IDYES)
	{
		IsPass = true;
	}
	else
	{
		IsPass = false;
	}

	return nStatus;
}

int ATATestClass::RunKeyPadTest(bool &IsPass, int Port)  //人工操作
{
	int ret;

	int nStatus = 0;
	char keystate[128] = { 0 };
	char param[] = { 0x00 };

	param[0] = 1; //按键检测open。
	nStatus = SP_AutoBBTest(g_hDiagPhone, DEVICE_AUTOTEST_KEYPAD, param, 1, NULL, 0);
	if (0 != nStatus)
	{
		IsPass = false;
		return nStatus;
	}
	Sleep(1000);

	CString show;
	show.Format("请确认端口%d的机子按键都已经按下",Port);
	ret = ::MessageBox(NULL, show, "测试结果", MB_YESNO | MB_SYSTEMMODAL);
	if (ret == IDYES)
	{
		param[0] = 4; //按键检测close。
		nStatus = SP_AutoBBTest(g_hDiagPhone, DEVICE_AUTOTEST_KEYPAD, param, 1, keystate, 1);
		if (0 != nStatus)
		{
			IsPass = false;
			return nStatus;
		}
	}
	else
	{
		IsPass = false;
	}

	param[0] = 3; //按键检测close。
	nStatus = SP_AutoBBTest(g_hDiagPhone, DEVICE_AUTOTEST_KEYPAD, param, 1, NULL, 0);
	if (0 != nStatus)
	{

		IsPass = false;
		return nStatus;
	}

	if (1 == keystate[0]) {

		IsPass = true;
		return nStatus;
	}
	return nStatus;
}

int ATATestClass::RunSIMTest(bool &IsPass, int SIMNum)
{
	int nStatus = 0;

	char param2[] = { 0x00,0x03,0x00 };
	nStatus = SP_AutoBBTest(g_hDiagPhone, DEVICE_AUTOTEST_SIM, param2, 3, NULL, 0);
	if (0 != nStatus)
	{
		IsPass = false;
		return nStatus;
	}
	IsPass = true;
	return nStatus;
}

int ATATestClass::RunCorrectFlagTest(bool &IsPass) //校准标志位
{
	int ret = 0;
	unsigned long  puFlag = 0;
	unsigned long ulRecv = 0;
	char szSend[512] = { 0 }, szRecv[512] = { 0 };
	int iOK;
	string strReturn;
	IsPass = true;

	DWORD adc_reserved7 = 0;
	if (!_SPOK(SP_gsmReadCalibrationParam(g_hDiagPhone,true,CALI_ADC,PC_CALI_EGSM900,INDEX_RESERVED7,4,(void *)&adc_reserved7)))
	{
		IsPass = false;
		return -1;
	}
	else
	{
		SP_DataEndianConv32(g_hDiagPhone, (unsigned char *)&adc_reserved7, 4);
	}


	WORD dwFlagWCDMA = 0;
	if (!_SPOK(SP_wcdmaLoadTestFlag(g_hDiagPhone, &dwFlagWCDMA)))
	{
		IsPass = false;
		return -1;
	}
	else
	{
		SP_DataEndianConv16(g_hDiagPhone, (unsigned char *)&dwFlagWCDMA, 2);
	}

	WORD dwFlagLTE = 0;
	LTE_NV_CALDATA_T  data;
	data.eNvType = LTE_NV_TYPE_CALI_PARAM_FLAG;
	data.nDataSize = 1;
	if (_SPOK(SP_LTEReadCalibrationParam(g_hDiagPhone, &data)))
	{
		dwFlagLTE = data.nData[0];
	}
	else
	{
		IsPass = false;
		return -1;
	}

	int nCount = sizeof(strCalFlag) / sizeof(strCalFlag[0]);
	for (int i = 0; i < nCount; i++)
	{
		if (i >= 22 && i <= 27)
		{
			if (GetCalFlagMaskByIndexWCDMA(i))
			{
				DWORD dwFlag = GetCalFlag(i);
				BOOL bRet = CheckOneCalFlag(dwFlagWCDMA, dwFlag, i);
				if (!bRet)
				{
					IsPass = false;
					break;
				}
			}
		}
		else if (i >= 28)
		{
			if (GetCalFlagMaskByIndexLTE(i))
			{
				DWORD dwFlag = GetCalFlag(i);
				BOOL bRet = CheckOneCalFlag(dwFlagLTE, dwFlag, i);
				if (!bRet)
				{
					IsPass = false;
					break;
				}
			}
		}
		else
		{
			if (GetCalFlagMaskByIndex(i))
			{
				DWORD dwFlag = GetCalFlag(i);
				BOOL bRet = CheckOneCalFlag(adc_reserved7, dwFlag, i);
				if (!bRet)
				{
					IsPass = false;
					break;
				}
			}
		}
	}
	return 0;
}
BOOL ATATestClass::GetCalFlagMaskByIndexWCDMA(int nIndex)
{
	int nBit = nIndex - 22;

	if (nIndex == 26)
	{
		nBit = 6;
	}

	if (nIndex == 27)
	{
		nBit = 12;
	}
	return (TEST_FLAG_W& GetCalFlag(nIndex)) >> nBit;
}
DWORD ATATestClass::GetCalFlag(int nIndex)
{
	DWORD dwFlag = 0;

	switch (nIndex)
	{
	case 0:
		dwFlag = TEST_FLAG_CALI_PCS_AGC;
		break;
	case 1:
		dwFlag = TEST_FLAG_CALI_PCS_APC;
		break;
	case 2:
		dwFlag = TEST_FLAG_CALI_DCS_AGC;
		break;
	case 3:
		dwFlag = TEST_FLAG_CALI_DCS_APC;
		break;
	case 4:
		dwFlag = TEST_FLAG_CALI_EGSM_AGC;
		break;
	case 5:
		dwFlag = TEST_FLAG_CALI_EGSM_APC;
		break;
	case 6:
		dwFlag = TEST_FLAG_CALI_GSM850_AGC;
		break;
	case 7:
		dwFlag = TEST_FLAG_CALI_GSM850_APC;
		break;
	case 8:
		dwFlag = TEST_FLAG_CALI_AFC;
		break;
	case 9:
		dwFlag = TEST_FLAG_CALI_ADC;
		break;
	case 10:
		dwFlag = TEST_FLAG_FINAL_PCS;
		break;
	case 11:
		dwFlag = TEST_FLAG_FINAL_DCS;
		break;
	case 12:
		dwFlag = TEST_FLAG_FINAL_EGSM;
		break;
	case 13:
		dwFlag = TEST_FLAG_FINAL_GSM850;
		break;
	case 14:
		dwFlag = TEST_FLAG_CALI_TD_AFC;
		break;
	case 15:
		dwFlag = TEST_FLAG_CALI_TD_APC;
		break;
	case 16:
		dwFlag = TEST_FLAG_CALI_TD_AGC;
		break;
	case 17:
		dwFlag = TEST_FLAG_FINAL_TD;
		break;
	case 18:
		dwFlag = TEST_FLAG_ANTENNA;
		break;
	case 19:
		dwFlag = TEST_FLAG_CALI_EDGE;
		break;
	case 20:
		dwFlag = TEST_FLAG_FINAL_EDGE;
		break;
	case 21:
		dwFlag = TEST_FLAG_MMI_CIT;
		break;
	case 22:
		dwFlag = WCDMA_TESTFLAG_CALI_BANDI;
		break;
	case 23:
		dwFlag = WCDMA_TESTFLAG_CALI_BANDII;
		break;
	case 24:
		dwFlag = WCDMA_TESTFLAG_CALI_BANDV;
		break;
	case 25:
		dwFlag = WCDMA_TESTFLAG_CALI_BANDVIII;
		break;
	case 26:
		dwFlag = WCDMA_TESTFLAG_CALI_BANDIV;
		break;
	case 27:
		dwFlag = WCDMA_TESTFLAG_FINAL_TEST;
		break;
	case 28:
		dwFlag = CALI_FLAG_LTE_AFC_TDD;
		break;
	case 29:
		dwFlag = CALI_FlAG_LTE_AGC_TDD;
		break;
	case 30:
		dwFlag = CALI_FLAG_LTE_APC_TDD;
		break;
	case 31:
		dwFlag = CALI_FLAG_LTE_AFC_FDD;
		break;
	case 32:
		dwFlag = CALI_FlAG_LTE_AGC_FDD;
		break;
	case 33:
		dwFlag = CALI_FLAG_LTE_APC_FDD;
		break;
	case 34:
		dwFlag = FINAL_FLAG_LTE;
		break;
	case 35:
		dwFlag = ANTENNA_FLAG_LTE;
		break;
	case 36:
		dwFlag = FINAL_FLAG_LTE_FDD;
		break;
	case 37:
		dwFlag = ANTENNA_FLAG_LTE_FDD;
		break;
	default:
		break;
	}
	return dwFlag;
}
BOOL ATATestClass::CheckOneCalFlag(DWORD dwValue, DWORD dwFlag, int nIndex)
{
	BOOL bCheck = FALSE;
	if (dwValue & dwFlag)
	{
		bCheck = TRUE;
	}
	else
	{
		bCheck = FALSE;
	}

	return bCheck;
}
BOOL ATATestClass::GetCalFlagMaskByIndexLTE(int nIndex)
{
	int nBit = nIndex - 28;

	if (nIndex == 34)
	{

		nBit = 12;
	}
	else if (nIndex == 35)
	{
		nBit = 13;
	}
	else if (nIndex == 36)
	{
		nBit = 14;
	}
	else if (nIndex == 37)
	{
		nBit = 15;
	}

	return (TEST_FLAG_LTE & GetCalFlag(nIndex)) >> nBit;

}
BOOL ATATestClass::GetCalFlagMaskByIndex(int nIndex)
{
	return (TEST_FLAG_GSM &GetCalFlag(nIndex)) >> nIndex;
}

int ATATestClass::RunAudioTest(int Type, bool &IsPass,int Port,float &SnrVal) //type=2:喇叭    type=1：听筒    type=3：耳机
{
	int ata_result = 0;
	E_ATADLL_RESULT ata_result_final = E_ATADLL_RESULT_SUCCESS;
	float SpeakVal;
	bool IsSucceed = true;
	Sleep(100);

	if (E_SPEAKER == Type)
	{
		ata_result = TestAudio(0,IsSucceed,SpeakVal,Port);
		IsPass = IsSucceed;
		SnrVal = SpeakVal;
	}

	if (E_RECEIVER == Type)
	{
		ata_result = TestAudio(1,IsSucceed,SpeakVal,Port);
		IsPass = IsSucceed;
	}

	if (E_EARTHPHONE == Type)
	{
		ata_result = TestAudio(2,IsSucceed,SpeakVal,Port);
		IsPass = IsSucceed;
	}
	return ata_result_final;
}
int ATATestClass::TestAudio(char index,bool &IsSucceed,float &SNRVal,int Port)
{
	char testItemName[64];
	string atstr = "";
	ATResult atret;
	int nStatus = 0;
	unsigned char data_buff[3000];

	if (index == 0)
	{
		sprintf(testItemName, "speaker");  
		char param2[] = { 0x00,0x01,0x00 };
		int iTemp;
		nStatus = SP_AutoBBTest(g_hDiagPhone, DEVICE_AUTOTEST_SPEAK, param2, 3, NULL, 0);
		if (0 != nStatus)
		{
			IsSucceed = false;
			return nStatus;
		}
		Sleep(1000);
		param2[1] = 0x01;
		nStatus = SP_AutoBBTest(g_hDiagPhone, DEVICE_AUTOTEST_MIC, param2, 3, NULL, 0);
		if (0 != nStatus)
		{
			IsSucceed = false;
			return nStatus;
		}
		Sleep(500);

		param2[1] = 0x02;
		nStatus = SP_AutoBBTest(g_hDiagPhone, DEVICE_AUTOTEST_MIC, param2, 3, data_buff, 128);
		if (0 != nStatus)
		{
			IsSucceed = false;
			return nStatus;
		}

		param2[1] = 0x03;
		unsigned char szRecv_8810[512] = { 0 };
		int iCount;
		for (iTemp = 0; iTemp < 4; iTemp++)
		{
			memset(szRecv_8810, NULL, sizeof(szRecv_8810));
			param2[2] = iTemp;

			//	 Sleep (500);
			nStatus = SP_AutoBBTest(g_hDiagPhone, DEVICE_AUTOTEST_MIC, param2, 3, szRecv_8810, 400);//接收数据

			if (0 != nStatus)
			{
				
			}
			for (iCount = 0; iCount < 400; iCount++)
			{
				data_buff[iTemp * 400 + iCount] += szRecv_8810[iCount];
			}
		}

		char FileName[64] = "pcm.pcm";
		FILE *m_fpDataFile = fopen(FileName, "wb+");
		if (fwrite(data_buff, 1600, 1, m_fpDataFile) != 1)
		{
			IsSucceed = false;
			return -1;
		}

		fflush(m_fpDataFile);
		fclose(m_fpDataFile);
		m_fpDataFile = NULL;

		CString strMicCase_DataTemp1, strMicCase_DataTemp2, strTemp;
		short ReceivedWave_MicShort[1024];
		float fTemp = 0.0;
		double ReceivedWave_Mic[1024];
		CStdioFile file;
		CString strDACDataPath = "ReadData_LR.txt";
		double SNR;
		file.Open(strDACDataPath, CFile::modeCreate | CFile::modeWrite | CFile::typeText);
		//	unsigned short temp_data; 
		for (iCount = 0; iCount < 800; iCount++)
		{
			iTemp = iCount * 2;

			strMicCase_DataTemp1.Format(_T("%02X"), data_buff[iTemp + 1]);
			strMicCase_DataTemp2.Format(_T("%02X"), data_buff[iTemp]);


			strTemp = strMicCase_DataTemp1 + strMicCase_DataTemp2;
			//	ReceivedWave_MicShort[iCount] = (short)(wcstoul(strTemp, NULL, 16));
			//	AddLog ("ReceivedWave_Mic[iCount] =%s\n",strTemp );
			ReceivedWave_MicShort[iCount] = (short)(_tcstoul(strTemp, 0, 16));
			//		AddLog ("ReceivedWave_MicShort[iCount] =%x\n",ReceivedWave_MicShort[iCount] );
			ReceivedWave_Mic[iCount] = ReceivedWave_MicShort[iCount];

			strTemp += _T("\n");
			file.WriteString(strTemp);
			fTemp = (fTemp > ReceivedWave_Mic[iCount]) ? fTemp : ReceivedWave_Mic[iCount];
		}
		file.Close();

		SNR = SNR_Wave_Verification(1000, 16000, 800, ReceivedWave_Mic);
		SNRVal = SNR;

		param2[1] = 0x04;
		nStatus = SP_AutoBBTest(g_hDiagPhone, DEVICE_AUTOTEST_MIC, param2, 3, data_buff, 10240, 2000);
		if (0 > nStatus)
		{
			IsSucceed = false;
			return nStatus;
		}

		param2[1] = 0x03;
		nStatus = SP_AutoBBTest(g_hDiagPhone, DEVICE_AUTOTEST_SPEAK, param2, 3, data_buff, 10240, 2000);
		if (0 > nStatus)
		{
			IsSucceed = false;
			return nStatus;
		}
		IsSucceed = true;
	}
	else if (index == 1)
	{
		sprintf(testItemName, "receiver");
	}
	else if (index == 2)
	{
		char headsetstate[128] = { 0 };

		sprintf(testItemName, "headset");
		char strMessage[100] = "";
		sprintf(strMessage, "[端口%d的设备]请插入耳机", Port);
		::AfxMessageBox(strMessage, 0, MB_SYSTEMMODAL);

		char param1[] = { 0x06 };
		nStatus = SP_AutoBBTest(g_hDiagPhone, DEVICE_AUTOTEST_MISC, param1, 1, headsetstate, 1);
		if (0 != nStatus)
		{
			IsSucceed = false;
			return nStatus;
		}
		else
		{
			if (headsetstate[0] == 0) 
			{
				IsSucceed = false;
				return -1;
			}
		}


		char param2[] = { 0x02,0x01,0x00 };
		param2[0] = 0x02; //HEADSET
		param2[1] = 0x01;
		int iTemp;
		nStatus = SP_AutoBBTest(g_hDiagPhone, DEVICE_AUTOTEST_SPEAK, param2, 3, NULL, 0);
		if (0 != nStatus)
		{
			IsSucceed = false;
			return nStatus;;
		}
		Sleep(1000);
		param2[0] = 0x01; //HEADSET_MIC
		param2[1] = 0x01;
		nStatus = SP_AutoBBTest(g_hDiagPhone, DEVICE_AUTOTEST_MIC, param2, 3, NULL, 0);
		if (0 != nStatus)
		{
			IsSucceed = false;
			return nStatus;
		}
		Sleep(500);

		param2[0] = 0x01; //HEADSET_MIC
		param2[1] = 0x02;
		nStatus = SP_AutoBBTest(g_hDiagPhone, DEVICE_AUTOTEST_MIC, param2, 3, data_buff, 128);
		if (0 != nStatus)
		{
			IsSucceed = false;
			return nStatus;
		}

		param2[0] = 0x01; //HEADSET_MIC
		param2[1] = 0x03;
		unsigned char szRecv_8810[512] = { 0 };
		int iCount;
		for (iTemp = 0; iTemp < 4; iTemp++)
		{
			memset(szRecv_8810, NULL, sizeof(szRecv_8810));
			param2[2] = iTemp;

			//	 Sleep (500);
			nStatus = SP_AutoBBTest(g_hDiagPhone, DEVICE_AUTOTEST_MIC, param2, 3, szRecv_8810, 400);//接收数据

			if (0 != nStatus)
			{
				
			}
			for (iCount = 0; iCount < 400; iCount++)
			{
				data_buff[iTemp * 400 + iCount] += szRecv_8810[iCount];
			}
		}

		char FileName[64] = "pcm.pcm";
		FILE *m_fpDataFile = fopen(FileName, "wb+");
		if (fwrite(data_buff, 1600, 1, m_fpDataFile) != 1)
		{
			IsSucceed = false;
			return -1;
		}

		fflush(m_fpDataFile);
		fclose(m_fpDataFile);
		m_fpDataFile = NULL;

		CString strMicCase_DataTemp1, strMicCase_DataTemp2, strTemp;
		short ReceivedWave_MicShort[1024];
		float fTemp = 0.0;
		double ReceivedWave_Mic[1024];
		CStdioFile file;
		CString strDACDataPath = "ReadData_LR.txt";
		double SNR;
		file.Open(strDACDataPath, CFile::modeCreate | CFile::modeWrite | CFile::typeText);
		//	unsigned short temp_data; 
		for (iCount = 0; iCount < 800; iCount++)
		{
			iTemp = iCount * 2;

			strMicCase_DataTemp1.Format(_T("%02X"), data_buff[iTemp + 1]);
			strMicCase_DataTemp2.Format(_T("%02X"), data_buff[iTemp]);


			strTemp = strMicCase_DataTemp1 + strMicCase_DataTemp2;
			//	ReceivedWave_MicShort[iCount] = (short)(wcstoul(strTemp, NULL, 16));
			//	AddLog ("ReceivedWave_Mic[iCount] =%s\n",strTemp );
			ReceivedWave_MicShort[iCount] = (short)(_tcstoul(strTemp, 0, 16));
			//		AddLog ("ReceivedWave_MicShort[iCount] =%x\n",ReceivedWave_MicShort[iCount] );
			ReceivedWave_Mic[iCount] = ReceivedWave_MicShort[iCount];

			strTemp += _T("\n");
			file.WriteString(strTemp);
			fTemp = (fTemp > ReceivedWave_Mic[iCount]) ? fTemp : ReceivedWave_Mic[iCount];
		}
		file.Close();

		SNR = SNR_Wave_Verification(1000, 16000, 800, ReceivedWave_Mic);
		SNRVal = SNR;

		param2[0] = 0x01; //HEADSET_MIC
		param2[1] = 0x04;
		nStatus = SP_AutoBBTest(g_hDiagPhone, DEVICE_AUTOTEST_MIC, param2, 3, data_buff, 10240, 2000);
		if (0 > nStatus)
		{
			IsSucceed = false;
			return nStatus;
		}

		param2[0] = 0x02; //HEADSET
		param2[1] = 0x03;
		nStatus = SP_AutoBBTest(g_hDiagPhone, DEVICE_AUTOTEST_SPEAK, param2, 3, data_buff, 10240, 2000);
		if (0 > nStatus)
		{
			IsSucceed = false;
			return nStatus;
		}

		sprintf(strMessage, "[端口%d的设备]请拔出耳机", Port);
		::AfxMessageBox(strMessage, 0, MB_SYSTEMMODAL);
	}
	else
	{
		sprintf(testItemName, "Audio");
	}

	return nStatus;
}
float ATATestClass::SNR_Wave_Verification(double Freq, double SampleRate, int SampleNum, double *pReadWaveData)
{
	float fSNR = 0.0;
	int iCount = 0;
	double DFT_m[1024];
	double *pWave = pReadWaveData, *pDFT_m = DFT_m;
	double  pi = 3.141592653589793;

	for (iCount = 0; iCount < SampleNum; iCount++)
	{
		pReadWaveData[iCount] = pReadWaveData[iCount] * (0.5 - 0.5*cos(2 * pi*iCount / (SampleNum - 1))); 
	}

	pWave = pReadWaveData;
	dft(pWave, SampleNum, pDFT_m);

	pDFT_m = DFT_m;  //把指针指回去
	fSNR = (float)SNREstimation(SampleRate, Freq, SampleNum, pDFT_m);

	return fSNR;
}
void ATATestClass::dft(double *input, int N, double *output)
{
	double tempr, tempi;
	int m, n;
	double  pi = 3.141592653589793;
	for (m = 0; m < N; m++)
	{
		tempr = 0.0;
		tempi = 0.0;
		for (n = 0; n < N; n++)
		{
			tempr = tempr + *(input + n) * cos(2 * pi*n*m / N);
			tempi = tempi + *(input + n) * sin(-2 * pi*n*m / N);
		}
		*(output + m) = sqrt(tempr*tempr + tempi * tempi);
	}
}
double ATATestClass::SNREstimation(double SampleRate, double Freq, int SampleNum, double * pSinwav)
{
	double SNR, Signal_power = 0.0, Noise_power = 0.0;
	int sig_position, n, m, Width = 9;

	sig_position = ceil(Freq / SampleRate * (SampleNum)); //要做判断除数是0的处理
	sig_position = ceil((double)(sig_position*SampleNum / SampleNum));
	m = (int)SampleNum / 2;

	for (n = 0; n < m; n++)
	{
		if ((n >= (sig_position - Width)) && (n <= (sig_position + Width)))
			Signal_power = Signal_power + *(pSinwav + n);
		else
			Noise_power = Noise_power + *(pSinwav + n);
	}
	Noise_power = Noise_power / (m - 2 * Width - 1);
	//	SNR=20*log10((double)(Signal_power/Noise_power))-10*log10((double)(SampleNum));
		// add 2012-3-20 防止进来的wave是空的时候算法的bug -Will
	SNR = 10 * log10((double)(Signal_power));
	CString strTemp(_T(""));
	strTemp.Format(_T("%.2f"), SNR);
	if (!strTemp.CompareNoCase(_T("-1.#J")))
	{
		SNR = 0.0;
	}
	return SNR;
}

int ATATestClass::RunTouchPanelTest(bool &IsPass)   //人工操作
{
	int nStatus = 0;
	char tpstate[128] = { 0xff };
	char param[] = { 0x00 };

	param[0] = 1;
	nStatus = SP_AutoBBTest(g_hDiagPhone, DEVICE_AUTOTEST_TOUCHPANEL, param, 1, NULL, 0);
	if (0 != nStatus)
	{
		IsPass = false;
	}
	Sleep(10000);

	param[0] = 2;
	nStatus = SP_AutoBBTest(g_hDiagPhone, DEVICE_AUTOTEST_TOUCHPANEL, param, 1, tpstate, 1);
	if (0 != nStatus)
	{
		IsPass = false;
		//return E_ATADLL_RESULT_SUCCESS;
	}

	if (0 == tpstate[0])
	{
		IsPass = true;
		return 0;
	}
	return E_ATADLL_RESULT_SUCCESS;
}