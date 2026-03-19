/** \file       test_template.cpp
 *
 *  \brief      Unit tests for template component
 *
 *  \copyright
 *      Copyright (c) 2025. All rights reserved.
 */
#include "template.h"
#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

class TemplateTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        spdlog::set_level(spdlog::level::info);
    }

    void TearDown() override
    {
        // Cleanup code if needed
    }
};

TEST_F(TemplateTest, AddPositiveNumbers)
{
    spdlog::info("Testing addition of positive numbers");
    EXPECT_EQ(template_add(5, 3), 8);
    EXPECT_EQ(template_add(100, 200), 300);
}

TEST_F(TemplateTest, AddNegativeNumbers)
{
    spdlog::info("Testing addition with negative numbers");
    EXPECT_EQ(template_add(-5, -3), -8);
    EXPECT_EQ(template_add(-10, 5), -5);
}

TEST_F(TemplateTest, SubtractNumbers)
{
    spdlog::info("Testing subtraction");
    EXPECT_EQ(template_subtract(10, 3), 7);
    EXPECT_EQ(template_subtract(5, 10), -5);
    EXPECT_EQ(template_subtract(0, 0), 0);
}

TEST_F(TemplateTest, IsPositiveCheck)
{
    spdlog::info("Testing positive number check");
    EXPECT_EQ(template_is_positive(5), TRUE);
    EXPECT_EQ(template_is_positive(0), FALSE);
    EXPECT_EQ(template_is_positive(-5), FALSE);
    EXPECT_EQ(template_is_positive(1), TRUE);
}

TEST_F(TemplateTest, InitializeModule)
{
    spdlog::info("Testing module initialization");
    Std_ReturnType result = template_init();
    EXPECT_EQ(result, (Std_ReturnType)E_OK);
}

TEST_F(TemplateTest, DoubleInitializeModuleFails)
{
    spdlog::info("Testing double initialization fails");
    // Call init twice - second should fail
    template_init();
    Std_ReturnType result = template_init();
    EXPECT_EQ(result, (Std_ReturnType)E_NOT_OK);
}

TEST_F(TemplateTest, LegacyPolyspaceFunction)
{
    spdlog::info("Testing legacy polyspace function");
    // Just verify it doesn't crash
    EXPECT_NO_THROW(template_polyspace());
}
