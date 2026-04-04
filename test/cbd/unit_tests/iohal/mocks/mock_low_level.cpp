#include "mock_low_level.h"

testing::StrictMock<MockLowLevel>* g_strictMockLowLevel = nullptr;

extern "C" const Mcu_ConfigType Mcu_Config_VS_0 = {(uint8)0U};
extern "C" const Can_43_FLEXCAN_ConfigType Can_43_FLEXCAN_Config = {(uint8)0U};

ScopedStrictMockLowLevel::ScopedStrictMockLowLevel()
{
    g_strictMockLowLevel = &mock_;
}

ScopedStrictMockLowLevel::~ScopedStrictMockLowLevel()
{
    g_strictMockLowLevel = nullptr;
}

testing::StrictMock<MockLowLevel>& ScopedStrictMockLowLevel::get()
{
    return mock_;
}

extern "C" void Platform_Init(const void* configPtr)
{
    g_strictMockLowLevel->Platform_Init(configPtr);
}

extern "C" void Port_Init(const void* configPtr)
{
    g_strictMockLowLevel->Port_Init(configPtr);
}

extern "C" void Dio_FlipChannel(Dio_ChannelType channelId)
{
    g_strictMockLowLevel->Dio_FlipChannel(channelId);
}

extern "C" void Mcu_Init(const Mcu_ConfigType* configPtr)
{
    g_strictMockLowLevel->Mcu_Init(configPtr);
}

extern "C" Std_ReturnType Mcu_InitClock(uint8 clockSetting)
{
    return g_strictMockLowLevel->Mcu_InitClock(clockSetting);
}

extern "C" Mcu_PllStatusType Mcu_GetPllStatus(void)
{
    return g_strictMockLowLevel->Mcu_GetPllStatus();
}

extern "C" void Mcu_DistributePllClock(void)
{
    g_strictMockLowLevel->Mcu_DistributePllClock();
}

extern "C" void Mcu_SetMode(uint8 mode)
{
    g_strictMockLowLevel->Mcu_SetMode(mode);
}

extern "C" void CanIf_Init(const void* configPtr)
{
    g_strictMockLowLevel->CanIf_Init(configPtr);
}

extern "C" void Can_43_FLEXCAN_Init(const void* configPtr)
{
    g_strictMockLowLevel->Can_43_FLEXCAN_Init(configPtr);
}

extern "C" Std_ReturnType Can_43_FLEXCAN_SetControllerMode(uint8 controller,
                                                           Can_ControllerStateType state)
{
    return g_strictMockLowLevel->Can_43_FLEXCAN_SetControllerMode(controller, state);
}

extern "C" Can_ReturnType Can_43_FLEXCAN_Write(uint16 hwObj, const Can_PduType* pduInfo)
{
    return g_strictMockLowLevel->Can_43_FLEXCAN_Write(hwObj, pduInfo);
}

extern "C" BaseType_t xTaskCreate(TaskFunction_t taskCode, const char* taskName,
                                  configSTACK_DEPTH_TYPE stackDepth, void* taskParams,
                                  UBaseType_t taskPriority, TaskHandle_t* createdTask)
{
    return g_strictMockLowLevel->xTaskCreate(taskCode, taskName, stackDepth, taskParams,
                                             taskPriority, createdTask);
}

extern "C" void vTaskStartScheduler(void)
{
    g_strictMockLowLevel->vTaskStartScheduler();
}

extern "C" void vTaskDelayUntil(TickType_t* previousWakeTime, TickType_t timeIncrement)
{
    g_strictMockLowLevel->vTaskDelayUntil(previousWakeTime, timeIncrement);
}

extern "C" TickType_t xTaskGetTickCount(void)
{
    return g_strictMockLowLevel->xTaskGetTickCount();
}
