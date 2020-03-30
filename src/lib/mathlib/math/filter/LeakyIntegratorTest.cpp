/****************************************************************************
 *
 *   Copyright (C) 2020 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/**
 * @file LeakyIntegratorTest.cpp
 *
 * @brief Google test for the Leaky Integrator
 */

#include <gtest/gtest.h>

#include "LeakyIntegrator.hpp"

using namespace math;

TEST(LeakyIntegratorTest, AllZeroTest)
{
	LeakyIntegrator<float> _leaky_integrator;
	EXPECT_FLOAT_EQ(_leaky_integrator.apply(0.f), 0.f);
}

TEST(LeakyIntegratorTest, AlphaOneTest)
{
	LeakyIntegrator<float> _leaky_integrator;
	_leaky_integrator.setParameters(100000.f, 100000.f);

	for (int i = 0; i < 100; i++) {
		EXPECT_FLOAT_EQ(_leaky_integrator.apply(1.f), 0.f);
	}
}

TEST(LeakyIntegratorTest, AlphaZeroTest)
{
	LeakyIntegrator<float> _leaky_integrator;
	_leaky_integrator.setParameters(10.f, 0.f);

	for (int i = 0; i < 100; i++) {
		const float new_smaple = static_cast<float>(i);
		EXPECT_FLOAT_EQ(_leaky_integrator.apply(new_smaple), new_smaple);
	}
}

TEST(LeakyIntegratorTest, ConvergenceTest)
{
	LeakyIntegrator<float> _leaky_integrator;
	_leaky_integrator.setParameters(10.f, 1.f);

	float last_value{0.f};

	for (int i = 0; i < 100; i++) {
		EXPECT_GE(_leaky_integrator.apply(1.f), last_value);
		last_value = _leaky_integrator.get();
	}

	EXPECT_NEAR(last_value, 1.f, 1e-4f);

	for (int i = 0; i < 1000; i++) {
		EXPECT_LE(_leaky_integrator.apply(-100.f), last_value);
		last_value = _leaky_integrator.get();
	}

	EXPECT_NEAR(last_value, -100.f, 1e-4f);
}

TEST(LeakyIntegratorTest, NANTest)
{
	LeakyIntegrator<float> _leaky_integrator;
	_leaky_integrator.reset(2.f);
	_leaky_integrator.setParameters(NAN, 1.f);

	EXPECT_FLOAT_EQ(_leaky_integrator.apply(1.f), 2.f);

	_leaky_integrator.setParameters(10.f, NAN);

	EXPECT_FLOAT_EQ(_leaky_integrator.apply(1.f), 2.f);

	_leaky_integrator.setParameters(10.f, 1.f);

	EXPECT_FLOAT_EQ(_leaky_integrator.apply(NAN), 2.f);
	EXPECT_LT(_leaky_integrator.apply(1.f), 2.f);
}
