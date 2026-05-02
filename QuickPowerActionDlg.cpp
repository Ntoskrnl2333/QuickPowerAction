// QuickPowerActionDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "QuickPowerAction.h"
#include "QuickPowerActionDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序"关于"菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
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

CAboutDlg::CAboutDlg() : CDialog(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CQuickPowerActionDlg 对话框



CQuickPowerActionDlg::CQuickPowerActionDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_QUICKPOWERACTION_DIALOG, pParent)
	, m_nPowerAction(0)
	, m_nSleepMode(0)
	, m_bForce(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CQuickPowerActionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_SHUTDOWN, m_nPowerAction);
	DDX_CBIndex(pDX, IDC_COMBO_SLEEP_MODE, m_nSleepMode);
	DDX_Check(pDX, IDC_CHECK_FORCE, m_bForce);
}

BEGIN_MESSAGE_MAP(CQuickPowerActionDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_EXECUTE, &CQuickPowerActionDlg::OnBnClickedExecute)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO_SHUTDOWN, IDC_RADIO_HIBERNATE, &CQuickPowerActionDlg::OnBnClickedRadioPowerAction)
END_MESSAGE_MAP()


// CQuickPowerActionDlg 消息处理程序

BOOL CQuickPowerActionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将"关于..."菜单项添加到系统菜单中。

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

	// 初始化睡眠模式下拉框
	UpdateSleepModeCombo();

	// 根据系统支持的电源操作启用/禁用单选按钮
	EnableControlsForAction();

	// 默认选中关机
	m_nPowerAction = 0;
	UpdateData(FALSE);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CQuickPowerActionDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CQuickPowerActionDlg::OnPaint()
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
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CQuickPowerActionDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CQuickPowerActionDlg::UpdateSleepModeCombo()
{
	CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_SLEEP_MODE);
	if (pCombo == nullptr)
		return;

	pCombo->ResetContent();

	// 获取系统支持的睡眠模式
	std::vector<CoreLayer::SleepMode> supportedModes = m_coreLayer.GetSupportedSleepMode();

	for (CoreLayer::SleepMode mode : supportedModes)
	{
		CString strMode;
		switch (mode)
		{
		case CoreLayer::SM_S1:
			strMode = _T("S1");
			break;
		case CoreLayer::SM_S2:
			strMode = _T("S2");
			break;
		case CoreLayer::SM_S3:
			strMode = _T("S3");
			break;
		default:
			continue;
		}
		pCombo->AddString(strMode);
	}

	// 默认选择第一项
	if (pCombo->GetCount() > 0)
	{
		pCombo->SetCurSel(0);
		m_nSleepMode = 0;
	}
}

void CQuickPowerActionDlg::EnableControlsForAction()
{
	// 获取系统支持的电源操作
	std::vector<CoreLayer::PowerAction> supportedActions = m_coreLayer.GetSupportedPowerAction();

	// 映射电源操作到单选按钮 ID
	struct ActionToRadio
	{
		CoreLayer::PowerAction action;
		int radioId;
	};

	ActionToRadio mapping[] = {
		{ CoreLayer::PA_Shutdown, IDC_RADIO_SHUTDOWN },
		{ CoreLayer::PA_Reboot, IDC_RADIO_REBOOT },
		{ CoreLayer::PA_Lock, IDC_RADIO_LOCK },
		{ CoreLayer::PA_Logoff, IDC_RADIO_LOGOFF },
		{ CoreLayer::PA_Sleep, IDC_RADIO_SLEEP },
		{ CoreLayer::PA_Hibernate, IDC_RADIO_HIBERNATE }
	};

	// 禁用所有单选按钮
	for (const auto& item : mapping)
	{
		CWnd* pWnd = GetDlgItem(item.radioId);
		if (pWnd != nullptr)
		{
			pWnd->EnableWindow(FALSE);
		}
	}

	// 启用支持的电源操作
	for (const auto& item : mapping)
	{
		for (CoreLayer::PowerAction action : supportedActions)
		{
			if (item.action == action)
			{
				CWnd* pWnd = GetDlgItem(item.radioId);
				if (pWnd != nullptr)
				{
					pWnd->EnableWindow(TRUE);
				}
				break;
			}
		}
	}
}

void CQuickPowerActionDlg::OnBnClickedRadioPowerAction(UINT nID)
{
	// 更新数据
	UpdateData(TRUE);

	// 根据选中的电源操作启用/禁用睡眠模式
	CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_SLEEP_MODE);
	CWnd* pStatic = GetDlgItem(IDC_STATIC_SLEEP_MODE);

	// 判断是否选中了睡眠选项
	bool isSleep = (nID == IDC_RADIO_SLEEP);

	if (pCombo != nullptr)
	{
		pCombo->EnableWindow(isSleep);
	}
	if (pStatic != nullptr)
	{
		pStatic->EnableWindow(isSleep);
	}
}

void CQuickPowerActionDlg::OnBnClickedExecute()
{
	UpdateData(TRUE);

	// 将单选按钮索引映射到电源操作
	CoreLayer::PowerAction action;
	switch (m_nPowerAction)
	{
	case 0: action = CoreLayer::PA_Shutdown; break;
	case 1: action = CoreLayer::PA_Reboot; break;
	case 2: action = CoreLayer::PA_Lock; break;
	case 3: action = CoreLayer::PA_Logoff; break;
	case 4: action = CoreLayer::PA_Sleep; break;
	case 5: action = CoreLayer::PA_Hibernate; break;
	default:
		AfxMessageBox(_T("请选择一个电源操作"), MB_ICONWARNING);
		return;
	}

	// 设置电源操作
	CoreLayer::ExecResult result = m_coreLayer.SetPowerAction(action);
	if (result != CoreLayer::ER_Success)
	{
		AfxMessageBox(_T("设置电源操作失败"), MB_ICONERROR);
		return;
	}

	// 如果是睡眠，设置睡眠模式
	if (action == CoreLayer::PA_Sleep)
	{
		// 获取选中的睡眠模式
		std::vector<CoreLayer::SleepMode> supportedModes = m_coreLayer.GetSupportedSleepMode();
		if (m_nSleepMode >= 0 && m_nSleepMode < (int)supportedModes.size())
		{
			result = m_coreLayer.SetSleepMode(supportedModes[m_nSleepMode]);
			if (result != CoreLayer::ER_Success)
			{
				AfxMessageBox(_T("设置睡眠模式失败"), MB_ICONERROR);
				return;
			}
		}
		else
		{
			AfxMessageBox(_T("请选择睡眠模式"), MB_ICONWARNING);
			return;
		}
	}

	// 设置强制执行
	m_coreLayer.SetIsForce(m_bForce != FALSE);

	// 执行电源操作
	result = m_coreLayer.ExecutePowerAction();

	// 根据结果显示错误信息（成功时通常不会返回）
	CString strError;
	switch (result)
	{
	case CoreLayer::ER_Success:
		// 执行成功，无需显示信息
		break;
	case CoreLayer::ER_BadArguments:
		strError = _T("参数错误");
		break;
	case CoreLayer::ER_PowerActionNotSupported:
		strError = _T("此电源操作不受支持");
		break;
	case CoreLayer::ER_NoPrivilege:
		strError = _T("没有足够的权限执行此操作\n请以管理员身份运行程序");
		break;
	case CoreLayer::ER_CannotLoadDLL:
		strError = _T("无法加载 NtDll.dll");
		break;
	case CoreLayer::ER_CannotReachFunction:
		strError = _T("无法找到所需的系统函数");
		break;
	case CoreLayer::ER_OutOfMemory:
		strError = _T("内存不足");
		break;
	default:
		strError = _T("未知错误");
		break;
	}

	if (!strError.IsEmpty())
	{
		AfxMessageBox(strError, MB_ICONERROR);
	}
}