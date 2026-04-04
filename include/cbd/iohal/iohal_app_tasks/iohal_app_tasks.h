/*******************************************************************************
 *
 *  File Name   : iohal_app_tasks.h
 *  Module      : IoHal — Application Tasks sub-module
 *  Description : Public interface for the application task layer.
 *
 *                Owns all periodic task definitions, scheduling configuration,
 *                and CAN TX callback registration.  Keeps main.c as a pure
 *                startup sequencer with no scheduling or protocol knowledge.
 *
 *  Author      : Mohammad Rashiqul Alam
 *
 *******************************************************************************/

#ifndef IOHAL_APP_TASKS_H
#define IOHAL_APP_TASKS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "Std_Types.h"

/**
 * @brief  Initialise all application tasks.
 *
 *         Registers the CAN TX confirmation callback and creates all
 *         FreeRTOS periodic tasks via IoHal_Os_CreateTask().
 *         Call after IoHal_Init() and before IoHal_Os_StartScheduler().
 *
 * @return E_OK on success, E_NOT_OK if any task creation fails.
 */
Std_ReturnType IoHal_AppTasks_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* IOHAL_APP_TASKS_H */
