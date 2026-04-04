/** \file       template.h
 *
 *  \brief      Template component header with sample functions
 *
 *  \author     Mohammad Rashiqul Alam
 *
 *  \copyright
 *      Copyright (c) 2025. All rights reserved.
 */

#ifndef TEMPLATE_H
#define TEMPLATE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <Std_Types.h>

/**
 * @brief Add two integers
 * @param a First integer
 * @param b Second integer
 * @return Sum of a and b
 */
sint32 template_add(sint32 a, sint32 b);

/**
 * @brief Subtract two integers
 * @param a First integer
 * @param b Second integer
 * @return Difference of a and b (a - b)
 */
sint32 template_subtract(sint32 a, sint32 b);

/**
 * @brief Check if a number is positive
 * @param value Number to check
 * @return TRUE if positive, FALSE otherwise
 */
boolean template_is_positive(sint32 value);

/**
 * @brief Initialize template module
 * @return E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType template_init(void);

/**
 * @brief Polyspace template function (legacy)
 */
void template_polyspace(void);

#ifdef __cplusplus
}
#endif

#endif /* TEMPLATE_H */
