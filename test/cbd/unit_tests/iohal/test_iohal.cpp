#include <gmock/gmock.h>
#include <gtest/gtest.h>

extern "C" {
#include "iohal.h"
}

#include "mock_low_level.h"

using testing::InSequence;
using testing::Return;

/*******************************************************************************
 * Sub-module mocks used only by test_iohal.
 *
 * These are local to this translation unit. The extern "C" forwarders below
 * dispatch to them. They must NOT be linked alongside mock_iohal.cpp because
 * they define the same symbols — test_iohal links only mock_low_level.
 *******************************************************************************/
namespace
{

class MockIoHalCan
{
public:
    MOCK_METHOD(Std_ReturnType, IoHal_Can_Init, (), ());
};

class MockIoHalStartup
{
public:
    MOCK_METHOD(Std_ReturnType, IoHal_AppTasks_Init, (), ());
    MOCK_METHOD(void, IoHal_Os_StartScheduler, (), ());
};

testing::StrictMock<MockIoHalCan>* g_mockIoHalCan = nullptr;
testing::StrictMock<MockIoHalStartup>* g_mockIoHalStartup = nullptr;

/** RAII guard — sets and clears g_mockIoHalCan / g_mockIoHalStartup. */
struct ScopedIoHalSubMocks
{
    testing::StrictMock<MockIoHalCan> can;
    testing::StrictMock<MockIoHalStartup> startup;

    ScopedIoHalSubMocks()
    {
        g_mockIoHalCan = &can;
        g_mockIoHalStartup = &startup;
    }
    ~ScopedIoHalSubMocks()
    {
        g_mockIoHalCan = nullptr;
        g_mockIoHalStartup = nullptr;
    }
};

} // namespace

/* -------------------------------------------------------------------------- */
/* extern "C" forwarders — null-guarded, matching mock_iohal.cpp pattern.     */
/* -------------------------------------------------------------------------- */

extern "C" Std_ReturnType IoHal_Can_Init(void)
{
    if (g_mockIoHalCan == nullptr) {
        return E_NOT_OK;
    }
    return g_mockIoHalCan->IoHal_Can_Init();
}

extern "C" Std_ReturnType IoHal_AppTasks_Init(void)
{
    if (g_mockIoHalStartup == nullptr) {
        return E_NOT_OK;
    }
    return g_mockIoHalStartup->IoHal_AppTasks_Init();
}

extern "C" void IoHal_Os_StartScheduler(void)
{
    if (g_mockIoHalStartup == nullptr) {
        return;
    }
    g_mockIoHalStartup->IoHal_Os_StartScheduler();
}

/* -------------------------------------------------------------------------- */
/* Tests                                                                       */
/* -------------------------------------------------------------------------- */

TEST(IoHalTest, InitReturnsOkWhenAllStepsSucceed)
{
    ScopedStrictMockLowLevel scopedLow;
    auto& low = scopedLow.get();
    ScopedIoHalSubMocks sub;

    InSequence seq;
    EXPECT_CALL(low, Platform_Init(testing::_));
    EXPECT_CALL(low, Mcu_Init(testing::_));
    EXPECT_CALL(low, Mcu_InitClock(testing::_)).WillOnce(Return(E_OK));
    EXPECT_CALL(low, Mcu_SetMode(testing::_));
    EXPECT_CALL(low, Port_Init(testing::_));
    EXPECT_CALL(sub.can, IoHal_Can_Init()).WillOnce(Return(E_OK));
    EXPECT_CALL(sub.startup, IoHal_AppTasks_Init()).WillOnce(Return(E_OK));
    EXPECT_CALL(sub.startup, IoHal_Os_StartScheduler());

    EXPECT_EQ(IoHal_Init(), (Std_ReturnType)E_OK);
}

TEST(IoHalTest, InitReturnsNotOkWhenMcuClockInitFails)
{
    ScopedStrictMockLowLevel scopedLow;
    auto& low = scopedLow.get();
    ScopedIoHalSubMocks sub;

    InSequence seq;
    EXPECT_CALL(low, Platform_Init(testing::_));
    EXPECT_CALL(low, Mcu_Init(testing::_));
    EXPECT_CALL(low, Mcu_InitClock(testing::_)).WillOnce(Return(E_NOT_OK));

    EXPECT_EQ(IoHal_Init(), (Std_ReturnType)E_NOT_OK);
}

TEST(IoHalTest, InitReturnsNotOkWhenCanInitFails)
{
    ScopedStrictMockLowLevel scopedLow;
    auto& low = scopedLow.get();
    ScopedIoHalSubMocks sub;

    InSequence seq;
    EXPECT_CALL(low, Platform_Init(testing::_));
    EXPECT_CALL(low, Mcu_Init(testing::_));
    EXPECT_CALL(low, Mcu_InitClock(testing::_)).WillOnce(Return(E_OK));
    EXPECT_CALL(low, Mcu_SetMode(testing::_));
    EXPECT_CALL(low, Port_Init(testing::_));
    EXPECT_CALL(sub.can, IoHal_Can_Init()).WillOnce(Return(E_NOT_OK));

    EXPECT_EQ(IoHal_Init(), (Std_ReturnType)E_NOT_OK);
}

TEST(IoHalTest, InitReturnsNotOkWhenAppTasksInitFails)
{
    ScopedStrictMockLowLevel scopedLow;
    auto& low = scopedLow.get();
    ScopedIoHalSubMocks sub;

    InSequence seq;
    EXPECT_CALL(low, Platform_Init(testing::_));
    EXPECT_CALL(low, Mcu_Init(testing::_));
    EXPECT_CALL(low, Mcu_InitClock(testing::_)).WillOnce(Return(E_OK));
    EXPECT_CALL(low, Mcu_SetMode(testing::_));
    EXPECT_CALL(low, Port_Init(testing::_));
    EXPECT_CALL(sub.can, IoHal_Can_Init()).WillOnce(Return(E_OK));
    EXPECT_CALL(sub.startup, IoHal_AppTasks_Init()).WillOnce(Return(E_NOT_OK));

    EXPECT_EQ(IoHal_Init(), (Std_ReturnType)E_NOT_OK);
}

TEST(IoHalTest, LedToggleCallsDioFlipChannel)
{
    ScopedStrictMockLowLevel scopedLow;
    auto& low = scopedLow.get();

    EXPECT_CALL(low, Dio_FlipChannel(DioConf_DioChannel_DioChannel_0));
    IoHal_Led_Toggle();
}
