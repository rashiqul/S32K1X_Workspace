/** \file       test_package.cpp
 *
 *  \copyright
 *      Copyright (c) 2025. All rights reserved.
 */

#include <stdio.h>
#include "config.h"

const char version[] = MY_PROJECT_VERSION;

int main(void)
{
    printf("Test Package application. Conan package version: %s", version );
    return 0;
}
