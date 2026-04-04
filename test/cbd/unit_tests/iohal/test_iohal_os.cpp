#include <gmock/gmock.h>
#include <gtest/gtest.h>

extern "C" {
#include "iohal_os.h"
}

#include "mock_low_level.h"

using testing::InSequence;
using testing::Return;

namespace
{

void DummyTask(void* params)
{
    (void)params;
}

} // namespace

TEST(IoHalOsTest, CreateTaskReturnsNotOkOnNullConfig)
{
    ScopedStrictMockLowLevel scopedLow;
    (void)scopedLow;

    EXPECT_EQ(IoHal_Os_CreateTask(nullptr), (Std_ReturnType)E_NOT_OK);
}

TEST(IoHalOsTest, CreateTaskReturnsOkOnPdPass)
{
    ScopedStrictMockLowLevel scopedLow;
    auto& low = scopedLow.get();

    IoHal_Os_TaskConfigType cfg = {
        .function = DummyTask, .name = "Task", .stackSize = 128U, .priority = 2U};

    EXPECT_CALL(low, xTaskCreate(cfg.function, cfg.name, (configSTACK_DEPTH_TYPE)cfg.stackSize,
                                 nullptr, (UBaseType_t)cfg.priority, (TaskHandle_t*)nullptr))
        .WillOnce(Return(pdPASS));

    EXPECT_EQ(IoHal_Os_CreateTask(&cfg), (Std_ReturnType)E_OK);
}

TEST(IoHalOsTest, CreateTaskReturnsNotOkWhenSchedulerRejectsTask)
{
    ScopedStrictMockLowLevel scopedLow;
    auto& low = scopedLow.get();

    IoHal_Os_TaskConfigType cfg = {
        .function = DummyTask, .name = "Task", .stackSize = 128U, .priority = 2U};

    EXPECT_CALL(low, xTaskCreate(cfg.function, cfg.name, (configSTACK_DEPTH_TYPE)cfg.stackSize,
                                 nullptr, (UBaseType_t)cfg.priority, (TaskHandle_t*)nullptr))
        .WillOnce(Return((BaseType_t)0));

    EXPECT_EQ(IoHal_Os_CreateTask(&cfg), (Std_ReturnType)E_NOT_OK);
}

TEST(IoHalOsTest, StartSchedulerCallsFreeRtos)
{
    ScopedStrictMockLowLevel scopedLow;
    auto& low = scopedLow.get();

    EXPECT_CALL(low, vTaskStartScheduler());
    IoHal_Os_StartScheduler();
}

TEST(IoHalOsTest, DelayUntilCallsFreeRtos)
{
    ScopedStrictMockLowLevel scopedLow;
    auto& low = scopedLow.get();

    IoHal_Os_TickType wake = 10U;
    EXPECT_CALL(low, vTaskDelayUntil(&wake, (IoHal_Os_TickType)5U));

    IoHal_Os_DelayUntil(&wake, (IoHal_Os_TickType)5U);
}

TEST(IoHalOsTest, GetTickCountReturnsFreeRtosValue)
{
    ScopedStrictMockLowLevel scopedLow;
    auto& low = scopedLow.get();

    EXPECT_CALL(low, xTaskGetTickCount()).WillOnce(Return((TickType_t)123U));
    EXPECT_EQ(IoHal_Os_GetTickCount(), (IoHal_Os_TickType)123U);
}
