/*
 * Copyright (c) 2016 Louis Langholtz https://github.com/louis-langholtz/Box2D
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include "gtest/gtest.h"
#include <Box2D/Dynamics/TimeStep.hpp>

using namespace box2d;

TEST(TimeStep, ByteSizeIs48)
{
	EXPECT_EQ(sizeof(TimeStep), size_t(48));
}

TEST(TimeStep, maxTranslation)
{
	const auto v = float_t(1);
	const auto n = std::nextafter(v, float_t(0));
	const auto inc = v - n;
	ASSERT_GT(inc, float_t(0));
	ASSERT_LT(inc, float_t(1));
	const auto max_inc = inc * TimeStep{}.maxTranslation;
	EXPECT_GT(max_inc, float_t(0));
	EXPECT_LT(max_inc, LinearSlop / 2);
#if 0
	std::cout << std::setprecision(std::numeric_limits<long double>::digits10 + 1);
	std::cout << " inc=" << inc;
	std::cout << " max_inc=" << max_inc;
	std::cout << " slop=" << LinearSlop;
	std::cout << std::endl;
#endif
}

TEST(TimeStep, maxRotation)
{
	const auto v = float_t(1);
	const auto n = std::nextafter(v, float_t(0));
	const auto inc = v - n;
	ASSERT_GT(inc, float_t(0));
	ASSERT_LT(inc, float_t(1));
	const auto max_inc = inc * TimeStep{}.maxRotation / 1_rad;
	EXPECT_GT(max_inc, float_t(0));
	EXPECT_LT(max_inc, AngularSlop / 2);
#if 0
	std::cout << std::setprecision(std::numeric_limits<long double>::digits10 + 1);
	std::cout << " inc=" << inc;
	std::cout << " max_inc=" << max_inc;
	std::cout << " slop=" << AngularSlop;
	std::cout << std::endl;
#endif
}
