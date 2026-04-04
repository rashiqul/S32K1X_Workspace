/*******************************************************************************
 *
 *  File Name : src/main.c
 *  Description: S32K144 firmware entry point.
 *
 *               Initialises hardware (IoHal), creates application tasks
 *               (AppTasks), then hands control to the RTOS scheduler.
 *               All scheduling, protocol, and driver logic lives in the
 *               respective modules — this file is intentionally minimal.
 *
 *  Hardware  : S32K144EVB-Q100
 *  AUTOSAR   : 4.7.0  |  RTD 3.0.0
 *  FreeRTOS  : V11.1.0 (via IoHal OS abstraction)
 *  Author    : Mohammad Rashiqul Alam
 *
 *******************************************************************************/

#include "iohal.h"
#include "iohal_app_tasks.h"

int main(void)
{
    IoHal_Init();
    IoHal_AppTasks_Init();
    IoHal_Os_StartScheduler();

    for (;;) {
        /* intentional infinite loop — trap runaway execution */
    }

    return 0;
}
