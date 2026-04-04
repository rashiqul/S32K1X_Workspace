#include <gmock/gmock.h>
#include <gtest/gtest.h>

extern "C" {
#include "iohal.h"
}

#include "mock_low_level.h"

using testing::InSequence;
using testing::Return;

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

} // namespace

extern "C" Std_ReturnType IoHal_Can_Init(void)
{
    return g_mockIoHalCan->IoHal_Can_Init();
}

extern "C" Std_ReturnType IoHal_AppTasks_Init(void)
{
    return g_mockIoHalStartup->IoHal_AppTasks_Init();
}

extern "C" void IoHal_Os_StartScheduler(void)
{
    g_mockIoHalStartup->IoHal_Os_StartScheduler();
}

TEST(IoHalTest, InitReturnsOkWhenAllStepsSucceed)
{
    ScopedStrictMockLowLevel scopedLow;
    auto& low = scopedLow.get();
    testing::StrictMock<MockIoHalCan> canMock;
    testing::StrictMock<MockIoHalStartup> startupMock;
    g_mockIoHalCan = &canMock;
    g_mockIoHalStartup = &startupMock;

    InSequence seq;
    EXPECT_CALL(low, Platform_Init(testing::_));
    EXPECT_CALL(low, Mcu_Init(testing::_));
    EXPECT_CALL(low, Mcu_InitClock(testing::_)).WillOnce(Return(E_OK));
    EXPECT_CALL(low, Mcu_SetMode(testing::_));
    EXPECT_CALL(low, Port_Init(testing::_));
    EXPECT_CALL(canMock, IoHal_Can_Init()).WillOnce(Return(E_OK));
    EXPECT_CALL(startupMock, IoHal_AppTasks_Init()).WillOnce(Return(E_OK));
    EXPECT_CALL(startupMock, IoHal_Os_StartScheduler());

    EXPECT_EQ(IoHal_Init(), (Std_ReturnType)E_OK);
    g_mockIoHalCan = nullptr;
    g_mockIoHalStartup = nullptr;
}

TEST(IoHalTest, InitReturnsNotOkWhenMcuClockInitFails)
{
    ScopedStrictMockLowLevel scopedLow;
    auto& low = scopedLow.get();
    testing::StrictMock<MockIoHalCan> canMock;
    testing::StrictMock<MockIoHalStartup> startupMock;
    g_mockIoHalCan = &canMock;
    g_mockIoHalStartup = &startupMock;

    InSequence seq;
    EXPECT_CALL(low, Platform_Init(testing::_));
    EXPECT_CALL(low, Mcu_Init(testing::_));
    EXPECT_CALL(low, Mcu_InitClock(testing::_)).WillOnce(Return(E_NOT_OK));

    EXPECT_EQ(IoHal_Init(), (Std_ReturnType)E_NOT_OK);
    g_mockIoHalCan = nullptr;
    g_mockIoHalStartup = nullptr;
}

TEST(IoHalTest, InitReturnsNotOkWhenCanInitFails)
{
    ScopedStrictMockLowLevel scopedLow;
    auto& low = scopedLow.get();
    testing::StrictMock<MockIoHalCan> canMock;
    testing::StrictMock<MockIoHalStartup> startupMock;
    g_mockIoHalCan = &canMock;
    g_mockIoHalStartup = &startupMock;

    InSequence seq;
    EXPECT_CALL(low, Platform_Init(testing::_));
    EXPECT_CALL(low, Mcu_Init(testing::_));
    EXPECT_CALL(low, Mcu_InitClock(testing::_)).WillOnce(Return(E_OK));
    EXPECT_CALL(low, Mcu_SetMode(testing::_));
    EXPECT_CALL(low, Port_Init(testing::_));
    EXPECT_CALL(canMock, IoHal_Can_Init()).WillOnce(Return(E_NOT_OK));

    EXPECT_EQ(IoHal_Init(), (Std_ReturnType)E_NOT_OK);
    g_mockIoHalCan = nullptr;
    g_mockIoHalStartup = nullptr;
}

TEST(IoHalTest, InitReturnsNotOkWhenAppTasksInitFails)
{
    ScopedStrictMockLowLevel scopedLow;
    auto& low = scopedLow.get();
    testing::StrictMock<MockIoHalCan> canMock;
    testing::StrictMock<MockIoHalStartup> startupMock;
    g_mockIoHalCan = &canMock;
    g_mockIoHalStartup = &startupMock;

    InSequence seq;
    EXPECT_CALL(low, Platform_Init(testing::_));
    EXPECT_CALL(low, Mcu_Init(testing::_));
    EXPECT_CALL(low, Mcu_InitClock(testing::_)).WillOnce(Return(E_OK));
    EXPECT_CALL(low, Mcu_SetMode(testing::_));
    EXPECT_CALL(low, Port_Init(testing::_));
    EXPECT_CALL(canMock, IoHal_Can_Init()).WillOnce(Return(E_OK));
    EXPECT_CALL(startupMock, IoHal_AppTasks_Init()).WillOnce(Return(E_NOT_OK));

    EXPECT_EQ(IoHal_Init(), (Std_ReturnType)E_NOT_OK);
    g_mockIoHalCan = nullptr;
    g_mockIoHalStartup = nullptr;
}

TEST(IoHalTest, LedToggleCallsDioFlipChannel)
{
    ScopedStrictMockLowLevel scopedLow;
    auto& low = scopedLow.get();

    EXPECT_CALL(low, Dio_FlipChannel(DioConf_DioChannel_DioChannel_0));
    IoHal_Led_Toggle();
}
