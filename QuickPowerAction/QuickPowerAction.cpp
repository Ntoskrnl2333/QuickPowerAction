
// QuickPowerAction.cpp: 定义应用程序的类行为。
//

#include "pch.h"
#include "framework.h"
#include "QuickPowerAction.h"
#include "QuickPowerActionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CApp

BEGIN_MESSAGE_MAP(CApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CApp 构造

CApp::CApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的 CApp 对象

CApp theApp;


DWORD
WINAPI
AdjustPrivilege(LPCSTR lpPrivilegeName, BOOL fEnable) {

	HANDLE hToken;
	TOKEN_PRIVILEGES NewState;
	LUID luidPrivilegeLUID;


	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken)) /* Open process token */
		return GetLastError();


	if (fEnable == FALSE) /*We disable all the privileges */
	{
		if (!AdjustTokenPrivileges(hToken, TRUE, NULL, 0, NULL, NULL))
			return GetLastError();

		else return ERROR_SUCCESS;
	}
	/*Look up the LUID value of the privilege */
	LookupPrivilegeValueA(NULL, lpPrivilegeName, &luidPrivilegeLUID);

	/* Fill the TOKEN_STATE structure*/
	NewState.PrivilegeCount = 1;
	NewState.Privileges[0].Luid = luidPrivilegeLUID;
	NewState.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;


	/* Adjust the process token's privileges */
	if (!AdjustTokenPrivileges(hToken, FALSE, &NewState, 0, NULL, NULL))
		return GetLastError();



	/* We've done successfully, return */
	return ERROR_SUCCESS;
}

typedef void(NTAPI* TYPE_NtShutdownSystem)(DWORD);
typedef void(NTAPI* TYPE_NtInitiatePowerAction)(
	POWER_ACTION SystemAction,
	SYSTEM_POWER_STATE MinSystemState,
	ULONG Flags,
	BOOLEAN Asynchronous);


// CApp 初始化

BOOL CApp::InitInstance()
{
BEGIN:
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	// 创建 shell 管理器，以防对话框包含
	// 任何 shell 树视图控件或 shell 列表视图控件。
	CShellManager *pShellManager = new CShellManager;

	// 激活“Windows Native”视觉管理器，以便在 MFC 控件中启用主题
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("Ntoskrnl"));

	CQuickPowerActionDlg dlg;
	m_pMainWnd = &dlg;
	dlg.usedll = -1;
	dlg.sleepmode = -1;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		AdjustPrivilege("SeShutdownPrivilege", TRUE);
		HMODULE hDll = GetModuleHandleA("NtDll.dll");
		switch (dlg.option) {
		case OPT_SHUTDOWN:
		case OPT_REBOOT:
			if (dlg.usedll == -1) {
				MessageBoxA(NULL,"请选择操作方式！","关闭 Windows",MB_OK|MB_ICONERROR);
				//goto BEGIN;
			}
			else if (dlg.usedll) {
				TYPE_NtShutdownSystem NtShutdownSystem = (TYPE_NtShutdownSystem)GetProcAddress(hDll, "NtShutdownSystem");
				NtShutdownSystem(dlg.option == OPT_SHUTDOWN ? 0 : 1);
			}
			else {
				if (dlg.option == OPT_SHUTDOWN)
					system("shutdown -s -t 1");
				else
					system("shutdown -r -t 1");
			}
			break;
		case OPT_SLEEP: {
			TYPE_NtInitiatePowerAction NtInitiatePowerAction = (TYPE_NtInitiatePowerAction)GetProcAddress(hDll, "NtInitiatePowerAction");
			switch (dlg.sleepmode) {
			case 1:
				NtInitiatePowerAction(PowerActionSleep, PowerSystemSleeping1, 0, TRUE);
				break;
			case 2:
				NtInitiatePowerAction(PowerActionSleep, PowerSystemSleeping2, 0, TRUE);
				break;
			case 3:
				NtInitiatePowerAction(PowerActionSleep, PowerSystemSleeping3, 0, TRUE);
				break;
			case -1:
				MessageBoxA(NULL, "请选择睡眠深度！", "关闭 Windows", MB_OK | MB_ICONERROR);
				//goto BEGIN;
				break;
			default:
				MessageBox(NULL,_T("未知错误！"),_T("关闭 Windows"),MB_ICONERROR|MB_OK);
			}
			break;
		}
		case OPT_HIBER: {
			TYPE_NtInitiatePowerAction NtInitiatePowerAction = (TYPE_NtInitiatePowerAction)GetProcAddress(hDll, "NtInitiatePowerAction");
			NtInitiatePowerAction(PowerActionHibernate, PowerSystemHibernate, 0, TRUE);
			break;
		}
		}
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "警告: 对话框创建失败，应用程序将意外终止。\n");
		TRACE(traceAppMsg, 0, "警告: 如果您在对话框上使用 MFC 控件，则无法 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS。\n");
	}

	// 删除上面创建的 shell 管理器。
	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	return FALSE;
}

