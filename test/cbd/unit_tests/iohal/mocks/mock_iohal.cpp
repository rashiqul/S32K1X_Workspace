#include "mock_iohal.h"

testing::StrictMock<MockIoHal>* g_strictMockIoHal = nullptr;

ScopedStrictMockIoHal::ScopedStrictMockIoHal()
{
    g_strictMockIoHal = &mock_;
}

ScopedStrictMockIoHal::~ScopedStrictMockIoHal()
{
    g_strictMockIoHal = nullptr;
}

testing::StrictMock<MockIoHal>& ScopedStrictMockIoHal::get()
{
    return mock_;
}

extern "C" Std_ReturnType IoHal_Init(void)
{
    if (g_strictMockIoHal == nullptr) {
        return E_NOT_OK;
    }
    return g_strictMockIoHal->IoHal_Init();
}

extern "C" void IoHal_Led_Toggle(void)
{
    if (g_strictMockIoHal == nullptr) {
        return;
    }
    g_strictMockIoHal->IoHal_Led_Toggle();
}

extern "C" uint8 IoHal_Led_GetState(void)
{
    if (g_strictMockIoHal == nullptr) {
        return (uint8)0U;
    }
    return g_strictMockIoHal->IoHal_Led_GetState();
}

extern "C" Std_ReturnType IoHal_Can_Init(void)
{
    if (g_strictMockIoHal == nullptr) {
        return E_NOT_OK;
    }
    return g_strictMockIoHal->IoHal_Can_Init();
}

extern "C" Std_ReturnType IoHal_Can_Transmit(const IoHal_Can_PduType* pdu)
{
    if (g_strictMockIoHal == nullptr) {
        return E_NOT_OK;
    }
    return g_strictMockIoHal->IoHal_Can_Transmit(pdu);
}

extern "C" void IoHal_Can_RegisterTxConfirmCallback(IoHal_Can_TxConfirmCallbackType callback)
{
    if (g_strictMockIoHal == nullptr) {
        return;
    }
    g_strictMockIoHal->IoHal_Can_RegisterTxConfirmCallback(callback);
}

extern "C" void App_CanTxConfirmation(PduIdType canIfTxPduId, Std_ReturnType result)
{
    if (g_strictMockIoHal == nullptr) {
        return;
    }
    g_strictMockIoHal->App_CanTxConfirmation(canIfTxPduId, result);
}

extern "C" Std_ReturnType IoHal_Os_CreateTask(const IoHal_Os_TaskConfigType* cfg)
{
    if (g_strictMockIoHal == nullptr) {
        return E_NOT_OK;
    }
    return g_strictMockIoHal->IoHal_Os_CreateTask(cfg);
}

extern "C" void IoHal_Os_StartScheduler(void)
{
    if (g_strictMockIoHal == nullptr) {
        return;
    }
    g_strictMockIoHal->IoHal_Os_StartScheduler();
}

extern "C" void IoHal_Os_DelayUntil(IoHal_Os_TickType* previousWakeTime,
                                    IoHal_Os_TickType timeIncrement)
{
    if (g_strictMockIoHal == nullptr) {
        return;
    }
    g_strictMockIoHal->IoHal_Os_DelayUntil(previousWakeTime, timeIncrement);
}

extern "C" IoHal_Os_TickType IoHal_Os_GetTickCount(void)
{
    if (g_strictMockIoHal == nullptr) {
        return (IoHal_Os_TickType)0U;
    }
    return g_strictMockIoHal->IoHal_Os_GetTickCount();
}
