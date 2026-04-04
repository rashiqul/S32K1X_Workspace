#ifndef TEST_CBD_UNIT_TESTS_IOHAL_MOCKS_MOCK_IOHAL_HPP
#define TEST_CBD_UNIT_TESTS_IOHAL_MOCKS_MOCK_IOHAL_HPP

#include <gmock/gmock.h>

extern "C" {
#include "iohal.h"
}

class MockIoHal
{
public:
    MOCK_METHOD(Std_ReturnType, IoHal_Init, (), ());
    MOCK_METHOD(void, IoHal_Led_Toggle, (), ());

    MOCK_METHOD(Std_ReturnType, IoHal_Can_Init, (), ());
    MOCK_METHOD(Std_ReturnType, IoHal_Can_Transmit, (const IoHal_Can_PduType* pdu), ());
    MOCK_METHOD(void, IoHal_Can_RegisterTxConfirmCallback,
                (IoHal_Can_TxConfirmCallbackType callback), ());
    MOCK_METHOD(void, App_CanTxConfirmation, (PduIdType canIfTxPduId, Std_ReturnType result), ());

    MOCK_METHOD(Std_ReturnType, IoHal_Os_CreateTask, (const IoHal_Os_TaskConfigType* cfg), ());
    MOCK_METHOD(void, IoHal_Os_StartScheduler, (), ());
    MOCK_METHOD(void, IoHal_Os_DelayUntil,
                (IoHal_Os_TickType * previousWakeTime, IoHal_Os_TickType timeIncrement), ());
    MOCK_METHOD(IoHal_Os_TickType, IoHal_Os_GetTickCount, (), ());
};

extern testing::StrictMock<MockIoHal>* g_strictMockIoHal;

class ScopedStrictMockIoHal
{
public:
    ScopedStrictMockIoHal();
    ~ScopedStrictMockIoHal();

    testing::StrictMock<MockIoHal>& get();

private:
    testing::StrictMock<MockIoHal> mock_;
};

#endif /* TEST_CBD_UNIT_TESTS_IOHAL_MOCKS_MOCK_IOHAL_HPP */
