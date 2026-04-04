/*******************************************************************************
 *
 *  File Name: src/bsp/sbc/Sbc_Uja1169.c
 *  Description: Minimal UJA1169 SBC driver — bit-bang SPI on PTB14-17.
 *
 *               Registers used (all direct MMIO — no AUTOSAR driver layer,
 *               intentional: this module runs before Port_Init is called in
 *               some sequences and must be self-contained):
 *
 *               PORTB PCR base : 0x4004A000
 *                 PCR[14] offset 0x038  — SCK
 *                 PCR[15] offset 0x03C  — MISO
 *                 PCR[16] offset 0x040  — MOSI
 *                 PCR[17] offset 0x044  — /CS
 *
 *               GPIOB base     : 0x400FF040
 *                 PDOR offset  0x00   — port data output
 *                 PSOR offset  0x04   — set output register
 *                 PCOR offset  0x08   — clear output register
 *                 PDIR offset  0x10   — port data input
 *                 PDDR offset  0x14   — port data direction (1=output)
 *
 *  Hardware  : NXP S32K144EVB-Q100  /  UJA1169ATK/F SBC
 *  Author    : Mohammad Rashiqul Alam
 *
 *******************************************************************************/

#include "Sbc_Uja1169.h"
#include <stdint.h>

/*******************************************************************************
 * PORTB / GPIOB register addresses (S32K144 Reference Manual, Chapter 12/13)
 *******************************************************************************/
#define PORTB_PCR_BASE (0x4004A000UL)
#define GPIOB_BASE     (0x400FF040UL)

/* PCR field: MUX=001 (GPIO), DSE=1 (high drive), no pull */
#define PCR_GPIO_OUTPUT (0x00000140UL) /* MUX=001 << 8, DSE bit 6 */
#define PCR_GPIO_INPUT  (0x00000100UL) /* MUX=001 << 8, no drive   */

/* GPIOB register offsets */
#define GPIOB_PDOR (*((volatile uint32_t *)(GPIOB_BASE + 0x00U)))
#define GPIOB_PSOR (*((volatile uint32_t *)(GPIOB_BASE + 0x04U)))
#define GPIOB_PCOR (*((volatile uint32_t *)(GPIOB_BASE + 0x08U)))
#define GPIOB_PDIR (*((volatile uint32_t *)(GPIOB_BASE + 0x10U)))
#define GPIOB_PDDR (*((volatile uint32_t *)(GPIOB_BASE + 0x14U)))

/* PORTB PCR registers (one per pin) */
#define PORTB_PCR(n) (*((volatile uint32_t *)(PORTB_PCR_BASE + ((uint32_t)(n) * 4U))))

/* Pin numbers */
#define PIN_SCK  (14U)
#define PIN_MISO (15U)
#define PIN_MOSI (16U)
#define PIN_CS   (17U)

#define PIN_MASK_SCK  (1UL << PIN_SCK)
#define PIN_MASK_MISO (1UL << PIN_MISO)
#define PIN_MASK_MOSI (1UL << PIN_MOSI)
#define PIN_MASK_CS   (1UL << PIN_CS)

/*******************************************************************************
 * UJA1169 SPI frame constants
 * Frame: [15:9] = addr[6:0], [8] = RnW (0=write), [7:0] = data
 *******************************************************************************/
#define UJA1169_ADDR_MODE_CTRL (0x01U) /* Mode Control Register address */
#define UJA1169_MODE_NORMAL    (0x07U) /* Normal mode: MC[2:0] = 111 */

/*******************************************************************************
 * Private helpers
 *******************************************************************************/
static void sck_high(void)  { GPIOB_PSOR = PIN_MASK_SCK; }
static void sck_low(void)   { GPIOB_PCOR = PIN_MASK_SCK; }
static void mosi_high(void) { GPIOB_PSOR = PIN_MASK_MOSI; }
static void mosi_low(void)  { GPIOB_PCOR = PIN_MASK_MOSI; }
static void cs_assert(void) { GPIOB_PCOR = PIN_MASK_CS; }  /* active-low */
static void cs_deassert(void) { GPIOB_PSOR = PIN_MASK_CS; }

/* Short delay: ~16 cycles @ 48 MHz ≈ 333 ns per half-bit.
 * UJA1169 max SPI clock is 4 MHz; 333 ns half-period → ~1.5 MHz — safe margin. */
static void spi_delay(void)
{
    volatile uint8_t i;
    for (i = 0U; i < 8U; i++) {
        __asm volatile("nop");
    }
}

/**
 * @brief  Transfer one 16-bit word, MSB first, SPI Mode 3.
 *         Clock idles HIGH; data captured on rising edge, shifted on falling.
 * @param  txData  16-bit word to send.
 * @return 16-bit word received (MISO).
 */
static uint16_t spi_transfer16(uint16_t txData)
{
    uint16_t rxData = 0U;
    uint8_t  bit;

    for (bit = 0U; bit < 16U; bit++) {
        /* Place MOSI — MSB first */
        if ((txData & 0x8000U) != 0U) {
            mosi_high();
        } else {
            mosi_low();
        }
        txData = (uint16_t)(txData << 1U);

        spi_delay();

        /* Rising edge — UJA1169 samples MOSI here */
        sck_high();
        spi_delay();

        /* Sample MISO on high phase */
        rxData = (uint16_t)(rxData << 1U);
        if ((GPIOB_PDIR & PIN_MASK_MISO) != 0U) {
            rxData |= 0x0001U;
        }

        /* Falling edge — shift next bit out */
        sck_low();
    }

    return rxData;
}

/**
 * @brief  Write one byte to a UJA1169 register.
 * @param  addr  7-bit register address.
 * @param  data  8-bit value to write.
 */
static void uja1169_write_reg(uint8_t addr, uint8_t data)
{
    /* Frame: [15:9] addr, [8] RnW=0, [7:0] data */
    uint16_t frame = (uint16_t)(((uint16_t)addr << 9U) | (uint16_t)data);

    cs_assert();
    spi_delay();
    spi_transfer16(frame);
    spi_delay();
    cs_deassert();
}

/**
 * @brief  Busy-wait ~1 ms at 48 MHz (48,000 iterations @ 1 cycle each + overhead).
 *         Used to allow UJA1169 to settle after mode switch.
 */
static void delay_1ms(void)
{
    volatile uint32_t i;
    for (i = 0U; i < 12000U; i++) {
        __asm volatile("nop");
    }
}

/*******************************************************************************
 * Public API
 *******************************************************************************/
void Sbc_Uja1169_Init(void)
{
    /*--------------------------------------------------------------------------
     * 1. Configure PORTB pins as GPIO
     *------------------------------------------------------------------------*/
    PORTB_PCR(PIN_SCK)  = PCR_GPIO_OUTPUT;
    PORTB_PCR(PIN_MOSI) = PCR_GPIO_OUTPUT;
    PORTB_PCR(PIN_CS)   = PCR_GPIO_OUTPUT;
    PORTB_PCR(PIN_MISO) = PCR_GPIO_INPUT;

    /* Set direction: SCK, MOSI, CS = output; MISO = input */
    GPIOB_PDDR |= (PIN_MASK_SCK | PIN_MASK_MOSI | PIN_MASK_CS);
    GPIOB_PDDR &= ~PIN_MASK_MISO;

    /*--------------------------------------------------------------------------
     * 2. Drive idle state: SCK high (Mode 3), /CS de-asserted, MOSI low
     *------------------------------------------------------------------------*/
    sck_high();
    cs_deassert();
    mosi_low();

    /* Small settle delay before first transaction */
    delay_1ms();

    /*--------------------------------------------------------------------------
     * 3. Write Mode Control Register → Normal mode
     *    UJA1169 register 0x01, value 0x07: MC[2:0] = 111 = Normal
     *------------------------------------------------------------------------*/
    uja1169_write_reg(UJA1169_ADDR_MODE_CTRL, UJA1169_MODE_NORMAL);

    /*--------------------------------------------------------------------------
     * 4. Wait for transceiver to become active (~1 ms typical)
     *------------------------------------------------------------------------*/
    delay_1ms();
}
