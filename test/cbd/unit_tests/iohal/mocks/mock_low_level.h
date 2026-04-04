#ifndef TEST_CBD_UNIT_TESTS_IOHAL_MOCKS_MOCK_LOW_LEVEL_H
#define TEST_CBD_UNIT_TESTS_IOHAL_MOCKS_MOCK_LOW_LEVEL_H

#include <gmock/gmock.h>

extern "C" {
#include "CanIf.h"
#include "Can_43_FLEXCAN.h"
#include "Dio.h"
#include "FreeRTOS.h"
#include "Mcu.h"
#include "Platform.h"
#include "Port.h"
#include "task.h"
}

class MockLowLevel
{
public:
    MOCK_METHOD(void, Platform_Init, (const void* configPtr), ());
    MOCK_METHOD(void, Port_Init, (const void* configPtr), ());
    MOCK_METHOD(void, Dio_FlipChannel, (Dio_ChannelType channelId), ());

    MOCK_METHOD(void, Mcu_Init, (const Mcu_ConfigType* configPtr), ());
    MOCK_METHOD(Std_ReturnType, Mcu_InitClock, (uint8 clockSetting), ());
    MOCK_METHOD(Mcu_PllStatusType, Mcu_GetPllStatus, (), ());
    MOCK_METHOD(void, Mcu_DistributePllClock, (), ());
    MOCK_METHOD(void, Mcu_SetMode, (uint8 mode), ());

    MOCK_METHOD(void, CanIf_Init, (const void* configPtr), ());
    MOCK_METHOD(void, Can_43_FLEXCAN_Init, (const void* configPtr), ());
    MOCK_METHOD(Std_ReturnType, Can_43_FLEXCAN_SetControllerMode,
                (uint8 controller, Can_ControllerStateType state), ());
    MOCK_METHOD(Can_ReturnType, Can_43_FLEXCAN_Write, (uint16 hwObj, const Can_PduType* pduInfo),
                ());

    MOCK_METHOD(BaseType_t, xTaskCreate,
                (TaskFunction_t taskCode, const char* taskName, configSTACK_DEPTH_TYPE stackDepth,
                 void* taskParams, UBaseType_t taskPriority, TaskHandle_t* createdTask),
                ());
    MOCK_METHOD(void, vTaskStartScheduler, (), ());
    MOCK_METHOD(void, vTaskDelayUntil, (TickType_t * previousWakeTime, TickType_t timeIncrement),
                ());
    MOCK_METHOD(TickType_t, xTaskGetTickCount, (), ());
};

extern testing::StrictMock<MockLowLevel>* g_strictMockLowLevel;

class ScopedStrictMockLowLevel
{
public:
    ScopedStrictMockLowLevel();
    ~ScopedStrictMockLowLevel();

    testing::StrictMock<MockLowLevel>& get();

private:
    testing::StrictMock<MockLowLevel> mock_;
};

#endif /* TEST_CBD_UNIT_TESTS_IOHAL_MOCKS_MOCK_LOW_LEVEL_H */
