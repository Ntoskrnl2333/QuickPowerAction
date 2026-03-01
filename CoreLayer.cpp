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