
// ATAToolDlg.h: 头文件
//

#pragma once
#include "ATATestClass.h"
// CATAToolDlg 对话框
class CATAToolDlg : public CDialogEx
{
// 构造
public:
	CATAToolDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ATATOOL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	void StartTest();
	ATATestClass *TestObj;
	afx_msg void OnBnClickedButton2();
	void FindCommPort(CComboBox * pComboBox, CString & ComNo);
	afx_msg void OnDropdownCombo1();
	afx_msg void OnDropdownCombo2();
	afx_msg void OnDropdownCombo3();
	afx_msg void OnDropdownCombo4();
	afx_msg void OnDropdownCombo5();
	afx_msg void OnDropdownCombo6();
	afx_msg void OnDropdownCombo7();
	afx_msg void OnDropdownCombo8();

	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedButton9();
	afx_msg void OnBnClickedButton10();

public:
	CString ComName[8];
	HANDLE ComHandle[8] = { 0 };
	CWinThread *ThreadHandle[8] = { 0 };
	UINT pFunc[8] = { 0 };
	bool ThreadFlag[8] = { 0 };

	bool GetCommPort(CComboBox * pComboBox, CString(&Com)[8], int ArryIndex);
	HANDLE InitCom(CString comName);
	int CloseCom(HANDLE handler);
	CString ReadPort(HANDLE ComHandle);
	bool WritePort(HANDLE ComHandle, CString val, DWORD & BytesWrite);
	bool StartWork(int BtId, int BoxId, int ShowId);
	void RunTestItem(HANDLE ComHandle, int ThreadIndex);
};

UINT StartThread(LPVOID lparam);

struct ThreadInfo 
{
	CATAToolDlg *DlgPtr;
	int index;
	HANDLE handle;
};
