
// QuickPowerActionDlg.h: 头文件
//

#pragma once
#include "CoreLayer.h"

// CQuickPowerActionDlg 对话框
class CQuickPowerActionDlg : public CDialog
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
	CoreLayer m_coreLayer;

	// 控件变量
	int m_nPowerAction;		// 当前选中的电源操作
	int m_nSleepMode;		// 当前选中的睡眠模式
	BOOL m_bForce;			// 是否强制执行

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedExecute();
	afx_msg void OnBnClickedRadioPowerAction(UINT nID);
	DECLARE_MESSAGE_MAP()

private:
	void UpdateSleepModeCombo();
	void EnableControlsForAction();
};
