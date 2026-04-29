#pragma once

#include <vector>
#include <windows.h>
#include <PowrProf.h>
#pragma comment(lib, "PowrProf.lib")


class CoreLayer
{
public:
	CoreLayer();
	
	enum PowerAction {
		PA_None,
		PA_Shutdown,
		PA_Reboot,
		PA_Lock,
		PA_Logoff,
		PA_Sleep,
		PA_Hibernate,
	};

	enum SleepMode {
		SM_None = 0,
		SM_S0 = 1 << 0,
		SM_S1 = 1 << 1,
		SM_S2 = 1 << 2,
		SM_S3 = 1 << 3
	};

	enum ExecResult {
		ER_Success,
		ER_UnknownError,
		ER_OutOfMemory,
		ER_BadArguments,
		ER_PowerActionNotSupported,
		ER_NoPrivilege,
		ER_CannotLoadDLL,
		ER_CannotReachFunction,
	};

	ExecResult GetPrivilege();
	ExecResult LoadDLL();
	ExecResult ReachFunction();
	ExecResult CallFunction();
	
	ExecResult ExecutePowerAction();

	std::vector<PowerAction> GetSupportedPowerAction();
	std::vector<SleepMode> GetSupportedSleepMode();

	ExecResult SetIsForce(bool force);
	ExecResult SetPowerAction(PowerAction action);
	ExecResult SetSleepMode(SleepMode sleep);
	
	bool GetIsForce()const;
	PowerAction GetPowerAction()const;
	SleepMode GetSleepMode()const;
private:
	bool m_curisforce;
	PowerAction m_curaction;
	SleepMode m_cursleep;

	DWORD WINAPI SysAdjustPrivilege(LPCSTR lpPrivilegeName, BOOL fEnable)const;
};

