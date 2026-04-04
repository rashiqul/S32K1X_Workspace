#include <string>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

extern "C" {
#include "iohal_app_tasks.h"
}

#include "mock_iohal.h"

using testing::_;
using testing::InSequence;
using testing::NotNull;
using testing::Return;
using testing::SaveArg;
using testing::Truly;

extern "C" {
extern volatile uint32 task_10ms_count;
extern volatile uint32 task_20ms_count;
extern volatile uint32 task_1000ms_count;
}

namespace
{

bool MatchesTaskConfig(const IoHal_Os_TaskConfigType* cfg, const char* expectedName,
                       uint32 expectedPriority)
{
    if (cfg == nullptr || cfg->name == nullptr || cfg->function == nullptr) {
        return false;
    }

    return std::string(cfg->name) == expectedName && cfg->priority == expectedPriority;
}

} // namespace

TEST(IoHalAppTasksTest, InitRegistersCallbackAndCreatesAllTasks)
{
    ScopedStrictMockIoHal scopedMock;
    auto& mock = scopedMock.get();

    InSequence seq;

    EXPECT_CALL(mock, IoHal_Can_RegisterTxConfirmCallback(NotNull()));
    EXPECT_CALL(mock, IoHal_Os_CreateTask(Truly([](const IoHal_Os_TaskConfigType* cfg) {
                    return MatchesTaskConfig(cfg, "Task_10ms",
                                             (uint32)(IOHAL_OS_IDLE_PRIORITY + 3U));
                })))
        .WillOnce(Return(E_OK));
    EXPECT_CALL(mock, IoHal_Os_CreateTask(Truly([](const IoHal_Os_TaskConfigType* cfg) {
                    return MatchesTaskConfig(cfg, "Task_20ms",
                                             (uint32)(IOHAL_OS_IDLE_PRIORITY + 2U));
                })))
        .WillOnce(Return(E_OK));
    EXPECT_CALL(mock, IoHal_Os_CreateTask(Truly([](const IoHal_Os_TaskConfigType* cfg) {
                    return MatchesTaskConfig(cfg, "Task_1000ms",
                                             (uint32)(IOHAL_OS_IDLE_PRIORITY + 1U));
                })))
        .WillOnce(Return(E_OK));

    EXPECT_EQ(IoHal_AppTasks_Init(), (Std_ReturnType)E_OK);
}

TEST(IoHalAppTasksTest, RegisteredTxCallbackTogglesLed)
{
    ScopedStrictMockIoHal scopedMock;
    auto& mock = scopedMock.get();

    IoHal_Can_TxConfirmCallbackType cb = nullptr;

    InSequence seq;
    EXPECT_CALL(mock, IoHal_Can_RegisterTxConfirmCallback(NotNull())).WillOnce(SaveArg<0>(&cb));
    EXPECT_CALL(mock, IoHal_Os_CreateTask(_)).Times(3).WillRepeatedly(Return(E_OK));

    EXPECT_EQ(IoHal_AppTasks_Init(), (Std_ReturnType)E_OK);
    ASSERT_NE(cb, nullptr);

    EXPECT_CALL(mock, IoHal_Led_Toggle());
    cb(E_OK);
}

TEST(IoHalAppTasksTest, TaskFunctionsRunSingleIterationInUnitTestMode)
{
    ScopedStrictMockIoHal scopedMock;
    auto& mock = scopedMock.get();

    const IoHal_Os_TaskConfigType* cfg10 = nullptr;
    const IoHal_Os_TaskConfigType* cfg20 = nullptr;
    const IoHal_Os_TaskConfigType* cfg1000 = nullptr;

    EXPECT_CALL(mock, IoHal_Can_RegisterTxConfirmCallback(NotNull()));
    EXPECT_CALL(mock, IoHal_Os_CreateTask(_))
        .Times(3)
        .WillOnce(testing::DoAll(SaveArg<0>(&cfg10), Return(E_OK)))
        .WillOnce(testing::DoAll(SaveArg<0>(&cfg20), Return(E_OK)))
        .WillOnce(testing::DoAll(SaveArg<0>(&cfg1000), Return(E_OK)));

    ASSERT_EQ(IoHal_AppTasks_Init(), (Std_ReturnType)E_OK);
    ASSERT_NE(cfg10, nullptr);
    ASSERT_NE(cfg20, nullptr);
    ASSERT_NE(cfg1000, nullptr);

    task_10ms_count = 0U;
    task_20ms_count = 0U;
    task_1000ms_count = 0U;

    EXPECT_CALL(mock, IoHal_Os_GetTickCount())
        .Times(3)
        .WillRepeatedly(Return((IoHal_Os_TickType)100U));
    EXPECT_CALL(mock, IoHal_Os_DelayUntil(_, _)).Times(3);
    EXPECT_CALL(mock, IoHal_Can_Transmit(_)).WillOnce(Return(E_OK));

    cfg10->function(nullptr);
    cfg20->function(nullptr);
    cfg1000->function(nullptr);

    EXPECT_EQ(task_10ms_count, (uint32)1U);
    EXPECT_EQ(task_20ms_count, (uint32)1U);
    EXPECT_EQ(task_1000ms_count, (uint32)1U);
}

TEST(IoHalAppTasksTest, InitStopsOnTaskCreationFailure)
{
    ScopedStrictMockIoHal scopedMock;
    auto& mock = scopedMock.get();

    InSequence seq;

    EXPECT_CALL(mock, IoHal_Can_RegisterTxConfirmCallback(NotNull()));
    EXPECT_CALL(mock, IoHal_Os_CreateTask(_)).WillOnce(Return(E_OK));
    EXPECT_CALL(mock, IoHal_Os_CreateTask(_)).WillOnce(Return(E_NOT_OK));

    EXPECT_EQ(IoHal_AppTasks_Init(), (Std_ReturnType)E_NOT_OK);
}

TEST(IoHalAppTasksTest, InitStopsImmediatelyWhenFirstTaskCreateFails)
{
    ScopedStrictMockIoHal scopedMock;
    auto& mock = scopedMock.get();

    InSequence seq;

    EXPECT_CALL(mock, IoHal_Can_RegisterTxConfirmCallback(NotNull()));
    EXPECT_CALL(mock, IoHal_Os_CreateTask(_)).WillOnce(Return(E_NOT_OK));

    EXPECT_EQ(IoHal_AppTasks_Init(), (Std_ReturnType)E_NOT_OK);
}

TEST(IoHalAppTasksTest, InitReturnsNotOkWhenThirdTaskCreateFails)
{
    ScopedStrictMockIoHal scopedMock;
    auto& mock = scopedMock.get();

    InSequence seq;

    EXPECT_CALL(mock, IoHal_Can_RegisterTxConfirmCallback(NotNull()));
    EXPECT_CALL(mock, IoHal_Os_CreateTask(_)).WillOnce(Return(E_OK));
    EXPECT_CALL(mock, IoHal_Os_CreateTask(_)).WillOnce(Return(E_OK));
    EXPECT_CALL(mock, IoHal_Os_CreateTask(_)).WillOnce(Return(E_NOT_OK));

    EXPECT_EQ(IoHal_AppTasks_Init(), (Std_ReturnType)E_NOT_OK);
}
