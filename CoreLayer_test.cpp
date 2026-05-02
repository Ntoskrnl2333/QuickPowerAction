// CoreLayer_test.cpp - 单元测试入口点
// 仅在 UNIT_TEST_MODE 下编译

#include "pch.h"
#include "CoreLayer.h"

#include <algorithm>
#include <vector>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

TEST_CASE("CoreLayer constructor initializes correctly") {
    CoreLayer layer;
    
    CHECK(layer.GetIsForce() == false);
    CHECK(layer.GetPowerAction() == CoreLayer::PA_None);
    CHECK(layer.GetSleepMode() == CoreLayer::SM_None);
}

TEST_CASE("CoreLayer SetPowerAction") {
    CoreLayer layer;
    
    CHECK(layer.SetPowerAction(CoreLayer::PA_Shutdown) == CoreLayer::ER_Success);
    CHECK(layer.GetPowerAction() == CoreLayer::PA_Shutdown);
    
    CHECK(layer.SetPowerAction(CoreLayer::PA_Reboot) == CoreLayer::ER_Success);
    CHECK(layer.GetPowerAction() == CoreLayer::PA_Reboot);
    
    CHECK(layer.SetPowerAction(CoreLayer::PA_Sleep) == CoreLayer::ER_Success);
    CHECK(layer.GetPowerAction() == CoreLayer::PA_Sleep);
    
    CHECK(layer.SetPowerAction(CoreLayer::PA_Hibernate) == CoreLayer::ER_Success);
    CHECK(layer.GetPowerAction() == CoreLayer::PA_Hibernate);
    
    CHECK(layer.SetPowerAction(static_cast<CoreLayer::PowerAction>(99)) == CoreLayer::ER_BadArguments);
    CHECK(layer.SetPowerAction(static_cast<CoreLayer::PowerAction>(-1)) == CoreLayer::ER_BadArguments);
}

TEST_CASE("CoreLayer SetSleepMode") {
    CoreLayer layer;
    
    CHECK(layer.SetSleepMode(CoreLayer::SM_S1) == CoreLayer::ER_Success);
    CHECK(layer.GetSleepMode() == CoreLayer::SM_S1);
    
    CHECK(layer.SetSleepMode(CoreLayer::SM_S2) == CoreLayer::ER_Success);
    CHECK(layer.GetSleepMode() == CoreLayer::SM_S2);
    
    CHECK(layer.SetSleepMode(CoreLayer::SM_S3) == CoreLayer::ER_Success);
    CHECK(layer.GetSleepMode() == CoreLayer::SM_S3);
    
    CHECK(layer.SetSleepMode(CoreLayer::SM_None) == CoreLayer::ER_Success);
    CHECK(layer.GetSleepMode() == CoreLayer::SM_None);
    
    CHECK(layer.SetSleepMode(static_cast<CoreLayer::SleepMode>(0xFF)) == CoreLayer::ER_BadArguments);
}

TEST_CASE("CoreLayer SetIsForce") {
    CoreLayer layer;
    
    CHECK(layer.SetIsForce(true) == CoreLayer::ER_Success);
    CHECK(layer.GetIsForce() == true);
    
    CHECK(layer.SetIsForce(false) == CoreLayer::ER_Success);
    CHECK(layer.GetIsForce() == false);
}

TEST_CASE("CoreLayer LoadDLL") {
    CoreLayer layer;
    CHECK(layer.LoadDLL() == CoreLayer::ER_Success);
}

TEST_CASE("CoreLayer ReachFunction") {
    CoreLayer layer;

    CoreLayer::ExecResult result = layer.ReachFunction();
    bool ok = (result == CoreLayer::ER_Success);
    ok = ok || (result == CoreLayer::ER_CannotReachFunction);
    CHECK(ok);
}

TEST_CASE("CoreLayer GetSupportedPowerAction") {
    CoreLayer layer;
    std::vector<CoreLayer::PowerAction> supported = layer.GetSupportedPowerAction();
    
    bool hasLock = std::find(supported.begin(), supported.end(), CoreLayer::PA_Lock) != supported.end();
    bool hasLogoff = std::find(supported.begin(), supported.end(), CoreLayer::PA_Logoff) != supported.end();
    CHECK(hasLock == true);
    CHECK(hasLogoff == true);
    
    CHECK(supported.size() >= 2);
}

TEST_CASE("CoreLayer GetSupportedSleepMode") {
    CoreLayer layer;
    std::vector<CoreLayer::SleepMode> supported = layer.GetSupportedSleepMode();
    
    for (auto mode : supported) {
        CHECK(mode >= CoreLayer::SM_S1);
        CHECK(mode <= CoreLayer::SM_S3);
    }
}

TEST_CASE("CoreLayer ExecutePowerAction BadArguments") {
    CoreLayer layer;
    
    layer.SetPowerAction(CoreLayer::PA_None);
    CHECK(layer.ExecutePowerAction() == CoreLayer::ER_BadArguments);
    
    layer.SetPowerAction(CoreLayer::PA_Sleep);
    layer.SetSleepMode(CoreLayer::SM_None);
    CHECK(layer.ExecutePowerAction() == CoreLayer::ER_BadArguments);
}
