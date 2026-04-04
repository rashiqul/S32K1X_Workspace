#include <gmock/gmock.h>
#include <gtest/gtest.h>

extern "C" {
#include "iohal_can.h"
}

#include "mock_low_level.h"

using testing::InSequence;
using testing::NotNull;
using testing::Return;

TEST(IoHalCanTest, InitReturnsOkWhenControllerStartSucceeds)
{
    ScopedStrictMockLowLevel scopedLow;
    auto& low = scopedLow.get();

    InSequence seq;
    EXPECT_CALL(low, CanIf_Init(testing::_));
    EXPECT_CALL(low, Can_43_FLEXCAN_Init(testing::_));
    EXPECT_CALL(low, Can_43_FLEXCAN_SetControllerMode(testing::_, CAN_CS_STARTED))
        .WillOnce(Return(E_OK));

    EXPECT_EQ(IoHal_Can_Init(), (Std_ReturnType)E_OK);
}

TEST(IoHalCanTest, InitReturnsNotOkWhenControllerStartFails)
{
    ScopedStrictMockLowLevel scopedLow;
    auto& low = scopedLow.get();

    InSequence seq;
    EXPECT_CALL(low, CanIf_Init(testing::_));
    EXPECT_CALL(low, Can_43_FLEXCAN_Init(testing::_));
    EXPECT_CALL(low, Can_43_FLEXCAN_SetControllerMode(testing::_, CAN_CS_STARTED))
        .WillOnce(Return(E_NOT_OK));

    EXPECT_EQ(IoHal_Can_Init(), (Std_ReturnType)E_NOT_OK);
}

TEST(IoHalCanTest, TransmitReturnsNotOkOnNullPdu)
{
    ScopedStrictMockLowLevel scopedLow;
    (void)scopedLow;

    EXPECT_EQ(IoHal_Can_Transmit(nullptr), (Std_ReturnType)E_NOT_OK);
}

TEST(IoHalCanTest, TransmitReturnsOkWhenDriverAcceptsFrame)
{
    ScopedStrictMockLowLevel scopedLow;
    auto& low = scopedLow.get();

    uint8 payload[8] = {0U};
    IoHal_Can_PduType pdu = {.id = 0x123U, .length = 8U, .sdu = payload};

    EXPECT_CALL(low, Can_43_FLEXCAN_Write(testing::_, NotNull())).WillOnce(Return(CAN_OK));
    EXPECT_EQ(IoHal_Can_Transmit(&pdu), (Std_ReturnType)E_OK);
}

TEST(IoHalCanTest, TransmitReturnsNotOkWhenDriverRejectsFrame)
{
    ScopedStrictMockLowLevel scopedLow;
    auto& low = scopedLow.get();

    uint8 payload[8] = {0U};
    IoHal_Can_PduType pdu = {.id = 0x456U, .length = 8U, .sdu = payload};

    EXPECT_CALL(low, Can_43_FLEXCAN_Write(testing::_, NotNull())).WillOnce(Return(CAN_NOT_OK));
    EXPECT_EQ(IoHal_Can_Transmit(&pdu), (Std_ReturnType)E_NOT_OK);
}

namespace
{

bool g_txCallbackCalled = false;

void TestTxCb(Std_ReturnType result)
{
    g_txCallbackCalled = (result == E_OK);
}

} // namespace

TEST(IoHalCanTest, TxConfirmationDispatchesRegisteredCallback)
{
    ScopedStrictMockLowLevel scopedLow;
    (void)scopedLow;

    g_txCallbackCalled = false;
    IoHal_Can_RegisterTxConfirmCallback(TestTxCb);

    App_CanTxConfirmation((PduIdType)0U, E_OK);
    EXPECT_TRUE(g_txCallbackCalled);
}

TEST(IoHalCanTest, TxConfirmationIsNoopWhenCallbackNotRegistered)
{
    ScopedStrictMockLowLevel scopedLow;
    (void)scopedLow;

    IoHal_Can_RegisterTxConfirmCallback((IoHal_Can_TxConfirmCallbackType) nullptr);
    App_CanTxConfirmation((PduIdType)0U, E_OK);
}
