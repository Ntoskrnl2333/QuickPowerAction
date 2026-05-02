#include "pch.h"
#include "CoreLayer.h"

CoreLayer::CoreLayer() {
	m_curisforce = false;
	m_curaction = PA_None;
	m_cursleep = SM_None;
}

CoreLayer::ExecResult CoreLayer::SetIsForce(bool force) {
	m_curisforce = force;
	return ER_Success;
}

CoreLayer::ExecResult CoreLayer::SetPowerAction(CoreLayer::PowerAction pa) {
	if (pa >= PA_None && pa <= PA_Hibernate) {
		m_curaction = pa;
		return ER_Success;
	}
	else
		return ER_BadArguments;
}

CoreLayer::ExecResult CoreLayer::SetSleepMode(CoreLayer::SleepMode sm) {
	if (sm >= SM_None && sm <= SM_S3) {
		m_cursleep = sm;
		return ER_Success;
	}
	else
		return ER_BadArguments;
}

bool CoreLayer::GetIsForce() const {
	return m_curisforce;
}

CoreLayer::PowerAction CoreLayer::GetPowerAction() const {
	return m_curaction;
}

CoreLayer::SleepMode CoreLayer::GetSleepMode() const {
	return m_cursleep;
}

DWORD WINAPI CoreLayer::SysAdjustPrivilege(LPCSTR lpPrivilegeName, BOOL fEnable) const {
	HANDLE hToken = NULL;
	TOKEN_PRIVILEGES NewState;
	LUID luidPrivilegeLUID;

	// 1. 打开 token（增加 TOKEN_QUERY）
	if (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
		&hToken))
	{
		return GetLastError();
	}

	// 2. 查找 LUID（必须检查）
	if (!LookupPrivilegeValueA(NULL, lpPrivilegeName, &luidPrivilegeLUID))
	{
		DWORD err = GetLastError();
		CloseHandle(hToken);
		return err;
	}

	// 3. 构造结构（支持 enable / disable 单个权限）
	NewState.PrivilegeCount = 1;
	NewState.Privileges[0].Luid = luidPrivilegeLUID;
	NewState.Privileges[0].Attributes = fEnable ? SE_PRIVILEGE_ENABLED : 0;

	// 4. 调整权限
	if (!AdjustTokenPrivileges(hToken, FALSE, &NewState, 0, NULL, NULL))
	{
		DWORD err = GetLastError();
		CloseHandle(hToken);
		return err;
	}

	// 5. 关键：检查是否真的生效
	DWORD err = GetLastError();

	// 6. 释放句柄（避免泄漏）
	CloseHandle(hToken);

	if (err == ERROR_NOT_ALL_ASSIGNED)
	{
		// 当前 token 没有该权限
		return err;
	}

	return ERROR_SUCCESS;
}

CoreLayer::ExecResult CoreLayer::GetPrivilege() {
	DWORD ret = SysAdjustPrivilege("SeShutdownPrivilege", TRUE);
	switch (ret) {
	case ERROR_SUCCESS:
		return ER_Success;
	case ERROR_NOT_ALL_ASSIGNED:
		return ER_NoPrivilege;
	case ERROR_INVALID_PARAMETER:
	case ERROR_NO_SUCH_PRIVILEGE:
		return ER_BadArguments;
	case ERROR_OUTOFMEMORY:
		return ER_OutOfMemory;
	case ERROR_ACCESS_DENIED:
		return ER_NoPrivilege;
	default:
		return ER_UnknownError;
	}
}

CoreLayer::ExecResult CoreLayer::LoadDLL() {
	HMODULE hDll = GetModuleHandleA("NtDll.dll");
	if (hDll == NULL) {
		return ER_CannotLoadDLL;
	}
	return ER_Success;
}

CoreLayer::ExecResult CoreLayer::ReachFunction() {
	HMODULE hDll = GetModuleHandleA("NtDll.dll");
	if (hDll == NULL) {
		return ER_CannotLoadDLL;
	}

	FARPROC pNtShutdownSystem = GetProcAddress(hDll, "NtShutdownSystem");
	FARPROC pNtInitiatePowerAction = GetProcAddress(hDll, "NtInitiatePowerAction");

	if (pNtShutdownSystem == NULL || pNtInitiatePowerAction == NULL) {
		return ER_CannotReachFunction;
	}

	return ER_Success;
}

typedef void(NTAPI* TYPE_NtShutdownSystem)(DWORD);
typedef void(NTAPI* TYPE_NtInitiatePowerAction)(
	POWER_ACTION SystemAction,
	SYSTEM_POWER_STATE MinSystemState,
	ULONG Flags,
	BOOLEAN Asynchronous);

CoreLayer::ExecResult CoreLayer::CallFunction() {
	HMODULE hDll = GetModuleHandleA("NtDll.dll");
	if (hDll == NULL) {
		return ER_CannotLoadDLL;
	}

	switch (m_curaction) {
	case PA_Shutdown:
	case PA_Reboot: {
		TYPE_NtShutdownSystem NtShutdownSystem =
			(TYPE_NtShutdownSystem)GetProcAddress(hDll, "NtShutdownSystem");
		if (NtShutdownSystem == NULL) {
			return ER_CannotReachFunction;
		}
		NtShutdownSystem(m_curaction == PA_Shutdown ? 0 : 1);
		break;
	}
	case PA_Sleep: {
		TYPE_NtInitiatePowerAction NtInitiatePowerAction =
			(TYPE_NtInitiatePowerAction)GetProcAddress(hDll, "NtInitiatePowerAction");
		if (NtInitiatePowerAction == NULL) {
			return ER_CannotReachFunction;
		}

		SYSTEM_POWER_STATE sleepState = PowerSystemSleeping1;
		switch (m_cursleep) {
		case SM_S1:
			sleepState = PowerSystemSleeping1;
			break;
		case SM_S2:
			sleepState = PowerSystemSleeping2;
			break;
		case SM_S3:
			sleepState = PowerSystemSleeping3;
			break;
		default:
			return ER_BadArguments;
		}

		NtInitiatePowerAction(PowerActionSleep, sleepState, 0, TRUE);
		break;
	}
	case PA_Hibernate: {
		TYPE_NtInitiatePowerAction NtInitiatePowerAction =
			(TYPE_NtInitiatePowerAction)GetProcAddress(hDll, "NtInitiatePowerAction");
		if (NtInitiatePowerAction == NULL) {
			return ER_CannotReachFunction;
		}
		NtInitiatePowerAction(PowerActionHibernate, PowerSystemHibernate, 0, TRUE);
		break;
	}
	default:
		return ER_PowerActionNotSupported;
	}

	return ER_Success;
}

CoreLayer::ExecResult CoreLayer::ExecutePowerAction() {
	if (m_curaction == PA_None) {
		return ER_BadArguments;
	}

	if (m_curaction == PA_Sleep && m_cursleep == SM_None) {
		return ER_BadArguments;
	}

	ExecResult ret;

	ret = GetPrivilege();
	if (ret != ER_Success) {
		return ret;
	}

	ret = LoadDLL();
	if (ret != ER_Success) {
		return ret;
	}

	ret = ReachFunction();
	if (ret != ER_Success) {
		return ret;
	}

	ret = CallFunction();
	return ret;
}

std::vector<CoreLayer::PowerAction> CoreLayer::GetSupportedPowerAction() {
	std::vector<PowerAction> supported;
	SYSTEM_POWER_CAPABILITIES caps = {};

	// 始终支持锁屏和注销（不需要特殊硬件支持）
	supported.push_back(PA_Lock);
	supported.push_back(PA_Logoff);

	// 获取系统电源能力
	if (!GetPwrCapabilities(&caps)) {
		return supported;
	}

	// 关机/重启：需要 SE_SHUTDOWN_PRIVILEGE 权限
	supported.push_back(PA_Shutdown);
	supported.push_back(PA_Reboot);

	// 睡眠：需要至少一种睡眠状态可用
	if (caps.SystemS1 || caps.SystemS2 || caps.SystemS3) {
		supported.push_back(PA_Sleep);
	}

	// 休眠：需要休眠文件存在
	if (caps.HiberFilePresent) {
		supported.push_back(PA_Hibernate);
	}

	return supported;
}

std::vector<CoreLayer::SleepMode> CoreLayer::GetSupportedSleepMode() {
	std::vector<SleepMode> supported;
	SYSTEM_POWER_CAPABILITIES caps = {};

	if (!GetPwrCapabilities(&caps)) {
		return supported;
	}

	// 按睡眠深度顺序添加支持的睡眠模式
	if (caps.SystemS1) {
		supported.push_back(SM_S1);
	}
	if (caps.SystemS2) {
		supported.push_back(SM_S2);
	}
	if (caps.SystemS3) {
		supported.push_back(SM_S3);
	}

	return supported;
}
