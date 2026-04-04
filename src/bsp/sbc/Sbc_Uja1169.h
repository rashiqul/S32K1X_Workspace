/*******************************************************************************
 *
 *  File Name: src/bsp/sbc/Sbc_Uja1169.h
 *  Description: Minimal UJA1169 SBC driver — bit-bang SPI on PTB14-17.
 *
 *               Puts the UJA1169 transceiver into Normal mode so that the
 *               FlexCAN physical layer is active and CAN frames appear on
 *               the bus.  Must be called before Can_43_FLEXCAN_Init().
 *
 *               SPI pin mapping (LPSPI1 physical pins, used as GPIO):
 *                 PTB14 — SCK  (output)
 *                 PTB15 — MISO (input)
 *                 PTB16 — MOSI (output)
 *                 PTB17 — /CS  (output, active-low)
 *
 *               Protocol: Mode 3 (CPOL=1, CPHA=1), 16-bit MSB-first frame.
 *               Frame format: { addr[6:0], RnW(0=write), data[7:0] }
 *
 *  Hardware  : NXP S32K144EVB-Q100  /  UJA1169ATK/F SBC
 *  Author    : Mohammad Rashiqul Alam
 *
 *******************************************************************************/

#ifndef SBC_UJA1169_H
#define SBC_UJA1169_H

#include "Std_Types.h"

/*******************************************************************************
 * Public API
 *******************************************************************************/

/**
 * @brief  Initialise the UJA1169 SBC and place the CAN transceiver into
 *         Normal (active) operating mode.
 *
 *         Sequence:
 *           1. Configure PTB14/16/17 as GPIO outputs, PTB15 as GPIO input.
 *           2. De-assert /CS; set SCK high (Mode 3 idle).
 *           3. Write 0x07 to UJA1169 Mode Control Register (address 0x01)
 *              → switches SBC from Standby to Normal mode.
 *           4. Small busy-wait after mode switch to allow transceiver to settle.
 *
 *         This function is called once from main() before CAN initialisation
 *         and before vTaskStartScheduler(), so it is safe to use a simple
 *         busy-wait delay (no FreeRTOS tick available yet).
 */
void Sbc_Uja1169_Init(void);

#endif /* SBC_UJA1169_H */
