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
