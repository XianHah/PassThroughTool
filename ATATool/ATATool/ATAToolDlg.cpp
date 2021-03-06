
// ATAToolDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "ATATool.h"
#include "ATAToolDlg.h"
#include "afxdialogex.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
typedef UINT(*ThreadFunc)(LPVOID);

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CATAToolDlg 对话框



CATAToolDlg::CATAToolDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ATATOOL_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CATAToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CATAToolDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CATAToolDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CATAToolDlg::OnBnClickedButton2)
	ON_CBN_DROPDOWN(IDC_COMBO1, &CATAToolDlg::OnDropdownCombo1)
	ON_CBN_DROPDOWN(IDC_COMBO2, &CATAToolDlg::OnDropdownCombo2)
	ON_CBN_DROPDOWN(IDC_COMBO3, &CATAToolDlg::OnDropdownCombo3)
	ON_CBN_DROPDOWN(IDC_COMBO4, &CATAToolDlg::OnDropdownCombo4)
	ON_CBN_DROPDOWN(IDC_COMBO5, &CATAToolDlg::OnDropdownCombo5)
	ON_CBN_DROPDOWN(IDC_COMBO6, &CATAToolDlg::OnDropdownCombo6)
	ON_CBN_DROPDOWN(IDC_COMBO7, &CATAToolDlg::OnDropdownCombo7)
	ON_CBN_DROPDOWN(IDC_COMBO8, &CATAToolDlg::OnDropdownCombo8)
	ON_BN_CLICKED(IDC_BUTTON3, &CATAToolDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CATAToolDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CATAToolDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CATAToolDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &CATAToolDlg::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, &CATAToolDlg::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON9, &CATAToolDlg::OnBnClickedButton9)
	ON_BN_CLICKED(IDC_BUTTON10, &CATAToolDlg::OnBnClickedButton10)
END_MESSAGE_MAP()


// CATAToolDlg 消息处理程序

BOOL CATAToolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	CString com;
	for (int i = 1002; i <= 1009; i++)
	{
		FindCommPort(((CComboBox*)GetDlgItem(i)), com);
	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CATAToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CATAToolDlg::OnPaint()
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
HCURSOR CATAToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CATAToolDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码

	AfxBeginThread(StartThread,this,0,0,THREAD_PRIORITY_NORMAL,NULL);
}


void CATAToolDlg::StartTest()
{
	E_ATDLL_TEST_ITEM_COUNT item = E_IMEI;
	S_ATADLL_TEST_ITEM_T	*itemcfg = new S_ATADLL_TEST_ITEM_T;
	
	TestObj = new ATATestClass;
	TestObj->InitConfig(3);
	if (!TestObj->ConnectTarget(1))
	{
		AfxMessageBox("connect faile");
		return;
	}
	TestObj->ConnectPM();
	Sleep(2000);

	//SWVERSION VerVal;  //测软件版本
	//TestObj->RunSWVersionTest(VerVal);

	/*char val[256];   //Gps
	TestObj->RunGPSTest(2, 20, val,120000);*/

	//int rssi;         //wifi
	//CString name;
	//TestObj->RunWifiTest("jimi", rssi,name);
	
	//bool IsPass;  //led
	//TestObj->RunLedTest(IsPass,TestObj->PelePort);

	//bool IsPass;  //charger
	//TestObj->RunChargerTest(IsPass);

	//bool IsPass = false;   //RTC
	//int SOSStatus;
	//TestObj->RunRTCTest(IsPass,SOSStatus);

	//bool IsPass = false;//LCM
	//TestObj->RunLCMTest(IsPass, TestObj->PelePort);

	//bool IsPass = false;  //key
	//TestObj->RunKeyPadTest(IsPass, TestObj->PelePort);

	//bool IsPass = false; //sim
	//TestObj->RunSIMTest(IsPass);

	/*bool IsPass = false;   //校准标志位
	TestObj->RunCorrectFlagTest(IsPass);*/

	//float val;  //speaker
	//bool IsPass = false;
	//TestObj->RunAudioTest(2,IsPass,TestObj->PelePort,val);  //type=2:喇叭    type=1：听筒    type=3：耳机

	/*bool IsPass = false;  //touchpanel
	TestObj->RunTouchPanelTest(IsPass);*/
}


void CATAToolDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	TestObj->BreakupConnectTarget();
}

void CATAToolDlg::FindCommPort(CComboBox *pComboBox, CString &ComNo)
{
	//先获取当前串口号
	int cur = 0;
	int nSel;
	BOOL curflag = TRUE;
	nSel = pComboBox->GetCurSel();
	if (nSel >= 0)
	{
		pComboBox->GetLBText(nSel, ComNo);
	}

	//先清空一下列表
	pComboBox->ResetContent();

	HKEY hKey;
#ifdef _DEBUG
	ASSERT(pComboBox != NULL);
	pComboBox->AssertValid();
#endif
	if (::RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Hardware\\DeviceMap\\SerialComm", NULL, KEY_READ, &hKey) == ERROR_SUCCESS) // 打开串口注册表
	{
		int i = 0;
		char portName[256], commName[256];
		DWORD dwLong, dwSize;
		while (1)
		{
			dwLong = dwSize = sizeof(portName);
			if (::RegEnumValue(hKey, i, LPSTR(portName), &dwLong, NULL, NULL, (PUCHAR)commName, &dwSize) == ERROR_NO_MORE_ITEMS) // 枚举串口
			{
				break;
			}
			pComboBox->AddString(LPCTSTR(commName));
			i++;
		}
		if (pComboBox->GetCount() == 0)
		{
			pComboBox->ResetContent();
			//SetRicheditText(L"找不到串口！请检测串口设备是否存在问题！", 1);
			//MessageBox(L"找不到串口！请检测串口设备是否存在问题！", L"提示信息", NULL);
			RegCloseKey(hKey);
			return;
		}
		pComboBox->SetCurSel(cur);
		RegCloseKey(hKey);
	}
}

void CATAToolDlg::OnDropdownCombo1()
{
	// TODO: 在此添加控件通知处理程序代码
	CString SubCom;
	FindCommPort(((CComboBox*)GetDlgItem(1002)), SubCom);
}


void CATAToolDlg::OnDropdownCombo2()
{
	// TODO: 在此添加控件通知处理程序代码
	CString SubCom;
	FindCommPort(((CComboBox*)GetDlgItem(1003)), SubCom);
}


void CATAToolDlg::OnDropdownCombo3()
{
	// TODO: 在此添加控件通知处理程序代码
	CString SubCom;
	FindCommPort(((CComboBox*)GetDlgItem(1004)), SubCom);
}


void CATAToolDlg::OnDropdownCombo4()
{
	// TODO: 在此添加控件通知处理程序代码
	CString SubCom;
	FindCommPort(((CComboBox*)GetDlgItem(1005)), SubCom);
}


void CATAToolDlg::OnDropdownCombo5()
{
	// TODO: 在此添加控件通知处理程序代码
	CString SubCom;
	FindCommPort(((CComboBox*)GetDlgItem(1006)), SubCom);
}


void CATAToolDlg::OnDropdownCombo6()
{
	// TODO: 在此添加控件通知处理程序代码
	CString SubCom;
	FindCommPort(((CComboBox*)GetDlgItem(1007)), SubCom);
}


void CATAToolDlg::OnDropdownCombo7()
{
	// TODO: 在此添加控件通知处理程序代码
	CString SubCom;
	FindCommPort(((CComboBox*)GetDlgItem(1008)), SubCom);
}


void CATAToolDlg::OnDropdownCombo8()
{
	// TODO: 在此添加控件通知处理程序代码
	CString SubCom;
	FindCommPort(((CComboBox*)GetDlgItem(1009)), SubCom);
}

bool CATAToolDlg::GetCommPort(CComboBox *pComboBox, CString(&Com)[8],int ArryIndex)
{
	int nSel;
	nSel = pComboBox->GetCurSel();
	pComboBox->GetLBText(nSel, Com[ArryIndex]);

	if (Com[ArryIndex] == "")
	{		
		return false;
	}
	return true;
}

HANDLE CATAToolDlg::InitCom(CString comName)
{
	HANDLE hCom;
	hCom = CreateFile("\\\\.\\" + comName,//COM口
		GENERIC_READ | GENERIC_WRITE, //允许读和写
		0, //独占方式
		NULL,
		OPEN_EXISTING, //打开而不是创建
		0, //同步方式
		NULL);
	if (hCom == INVALID_HANDLE_VALUE)
	{
		//SetRicheditText(comName+L"串口初始化失败！", 1);
		//MessageBox(L"串口初始化失败！", L"提示信息", NULL);
		return NULL;
	}
	SetupComm(hCom, 100, 100); //输入缓冲区和输出缓冲区的大小都是100
	COMMTIMEOUTS TimeOuts;
	//设定读超时
	TimeOuts.ReadIntervalTimeout = MAXDWORD;
	TimeOuts.ReadTotalTimeoutMultiplier = 0;
	TimeOuts.ReadTotalTimeoutConstant = 0;
	//在读一次输入缓冲区的内容后读操作就立即返回，
	//而不管是否读入了要求的字符。
	//设定写超时
	TimeOuts.WriteTotalTimeoutMultiplier = 1500;
	TimeOuts.WriteTotalTimeoutConstant = 1500;
	SetCommTimeouts(hCom, &TimeOuts); //设置超时

	DCB dcb;
	GetCommState(hCom, &dcb);
	dcb.BaudRate = 115200; //波特率
	dcb.ByteSize = 8; //每个字节有8位
	dcb.Parity = NOPARITY; //无奇偶校验位
	dcb.StopBits = ONESTOPBIT; //1个停止位
	SetCommState(hCom, &dcb);
	PurgeComm(hCom, PURGE_TXCLEAR | PURGE_RXCLEAR);
	return hCom;
}

int CATAToolDlg::CloseCom(HANDLE handler)
{
	return CloseHandle(handler);
}

CString CATAToolDlg::ReadPort(HANDLE ComHandle)
{
	char str[200];
	memset(str, 0, sizeof(str) / sizeof(str[0]));;
	DWORD readreal = 0;
	BOOL bReadStat;
	CString strread;
	char szMsg[255] = { 0 };
	char p_StrBcd[255] = { 0 };
	bReadStat = ReadFile(ComHandle, str, 199, &readreal, 0);
	if (bReadStat) {
		strread = str;
	}
	else
	{
		strread = "";
	}
	return strread;
}

bool CATAToolDlg::WritePort(HANDLE ComHandle ,CString val ,DWORD &BytesWrite)
{
	return WriteFile(ComHandle, CT2A(val), val.GetLength(), &BytesWrite, NULL);
}

void CATAToolDlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	StartWork(1020, 1002, 1);
}


void CATAToolDlg::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码
	StartWork(1021, 1003, 2);
}


void CATAToolDlg::OnBnClickedButton5()
{
	// TODO: 在此添加控件通知处理程序代码
	StartWork(1022, 1004, 3);
}


void CATAToolDlg::OnBnClickedButton6()
{
	// TODO: 在此添加控件通知处理程序代码
	StartWork(1023, 1005, 4);
}


void CATAToolDlg::OnBnClickedButton7()
{
	// TODO: 在此添加控件通知处理程序代码
	StartWork(1024, 1006, 5);
}


void CATAToolDlg::OnBnClickedButton8()
{
	// TODO: 在此添加控件通知处理程序代码
	StartWork(1025, 1007, 6);
}


void CATAToolDlg::OnBnClickedButton9()
{
	// TODO: 在此添加控件通知处理程序代码
	StartWork(1026, 1008, 7);
}


void CATAToolDlg::OnBnClickedButton10()
{
	// TODO: 在此添加控件通知处理程序代码
	StartWork(1027, 1009, 8);
}

bool  CATAToolDlg::StartWork(int BtId, int BoxId, int ShowId)
{
	CString ButtomText;
	CString ComPareText,IniFailStr,CloseFailStr;
	ComPareText.Format("透传%d", ShowId);
	IniFailStr = ComPareText+" 串口初始化失败！";
	CloseFailStr = ComPareText+" 关闭串口失败! ";
	GetDlgItem(BtId)->GetWindowText(ButtomText);
	GetDlgItem(BtId)->EnableWindow(false);
	if (ButtomText == ComPareText)
	{
		if (!GetCommPort(((CComboBox*)GetDlgItem(BoxId)), ComName, ShowId - 1))
		{
			MessageBox("请先选择串口号！", "提示信息", NULL);
			GetDlgItem(BtId)->EnableWindow(true);
			return false;
		}
		ComHandle[ShowId-1] = InitCom(ComName[ShowId-1]);
		if (ComHandle[ShowId - 1] == NULL)
		{
			MessageBox(IniFailStr, "提示信息", NULL);
			GetDlgItem(BtId)->EnableWindow(true);
			return false;
		}
		if (ThreadHandle[ShowId - 1]!=NULL)
		{
			ThreadFlag[ShowId - 1] = false;			
			ThreadHandle[ShowId - 1] = NULL;
			Sleep(500);
		}
		ThreadInfo info;
		info.DlgPtr = this;
		info.handle = ComHandle[ShowId - 1];
		info.index = ShowId - 1;
		ThreadFlag[ShowId - 1] = true;
		ThreadHandle[ShowId-1] = AfxBeginThread(StartThread,&info,THREAD_PRIORITY_NORMAL,0,0,NULL);
		if (ThreadHandle[ShowId-1]==NULL)
		{
			MessageBox("线程启动失败！", "提示信息", NULL);
			GetDlgItem(BtId)->EnableWindow(true);
			return false;
		}
		GetDlgItem(BtId)->SetWindowText("停止");
		GetDlgItem(BoxId)->EnableWindow(false);
		GetDlgItem(BoxId + 10)->EnableWindow(false);
	}
	else
	{
		if (!CloseCom(ComHandle[ShowId - 1]))
		{
			MessageBox(CloseFailStr, "提示信息", NULL);
			GetDlgItem(BtId)->EnableWindow(true);
			return false;
		}
		ThreadFlag[ShowId - 1] = false;
		GetDlgItem(BtId)->SetWindowText(ComPareText);
		GetDlgItem(BoxId)->EnableWindow(true);
		GetDlgItem(BoxId + 10)->EnableWindow(true);
	}
	GetDlgItem(BtId)->EnableWindow(true);
	return true;
}

UINT StartThread(LPVOID lparam)
{
	ThreadInfo *info = (ThreadInfo*)lparam;
	CATAToolDlg *dlg = (CATAToolDlg*)info->DlgPtr;
	dlg->RunTestItem(info->handle, info->index);
	return 0;
}

void CATAToolDlg::RunTestItem(HANDLE ComHandle,int ThreadIndex)
{
	PurgeComm(ComHandle, PURGE_RXABORT | PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_TXABORT);
	CString ReadVal;
	while (ThreadFlag[ThreadIndex])
	{
		ReadVal = ReadPort(ComHandle);
		Sleep(500);
	}
}


