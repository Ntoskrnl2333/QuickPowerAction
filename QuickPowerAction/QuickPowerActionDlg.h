
// QuickPowerActionDlg.h: 头文件
//

#pragma once

enum {
	OPT_NULL,
	OPT_SHUTDOWN,
	OPT_REBOOT,
	OPT_SLEEP,
	OPT_HIBER
};


// CQuickPowerActionDlg 对话框
class CQuickPowerActionDlg : public CDialogEx
{
// 构造
public:
	CQuickPowerActionDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_QUICKPOWERACTION_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedShutdown();
	CComboBox List1;
	afx_msg void OnBnClickedReboot();
	afx_msg void OnBnClickedSleep();
	afx_msg void OnBnClickedHiber();

public:
	int option = 0, sleepmode=1, usedll;
	afx_msg void OnCbnSelchangeCombo1();
	CComboBox List2;
	afx_msg void OnCbnSelchangeCombo2();
};
