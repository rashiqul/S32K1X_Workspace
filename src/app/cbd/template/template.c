/** \file       template.c
 *
 *  \brief      Template component implementation with sample functions
 *
 *  \copyright
 *      Copyright (c) 2025. All rights reserved.
 */
#include "template.h"

static boolean module_initialized = FALSE;

/**
 * @brief Add two integers
 */
sint32 template_add(sint32 a, sint32 b)
{
    return a + b;
}

/**
 * @brief Subtract two integers
 */
sint32 template_subtract(sint32 a, sint32 b)
{
    return a - b;
}

/**
 * @brief Check if a number is positive
 */
boolean template_is_positive(sint32 value)
{
    return (value > 0) ? TRUE : FALSE;
}

/**
 * @brief Initialize template module
 */
Std_ReturnType template_init(void)
{
    if (module_initialized == TRUE) {
        return E_NOT_OK; /* Already initialized */
    }

    module_initialized = TRUE;
    return E_OK;
}

/**
 * Polyspace template file required to make CI/CD pipeline pass (legacy)
 */
void template_polyspace(void)
{
    /* Empty function for compatibility */
}
