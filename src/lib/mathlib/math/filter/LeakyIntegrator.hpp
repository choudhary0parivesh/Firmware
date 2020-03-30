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

/*
 * @file LeakyIntegrator.hpp
 *
 * @brief Implementation of a leaky integrator which is also known as alpha filter or forgetting average.
 *
 * @author Matthias Grob <maetugr@gmail.com>
 */

#pragma once

#include <px4_platform_common/defines.h>
#include "../Limits.hpp"

namespace math
{

template<typename T>
class LeakyIntegrator
{
public:
	LeakyIntegrator() = default;
	~LeakyIntegrator() = default;

	/**
	 * Set filter parameters for time abstraction
	 *
	 * @param sample_freq sample frequency
	 * @param time_constant filter time constant determining convergence
	 */
	void setParameters(float sample_freq, float time_constant)
	{
		_alpha = time_constant / (time_constant + (static_cast<T>(1) / sample_freq));
		_alpha = math::constrain(_alpha, static_cast<T>(0), static_cast<T>(1));
		printf("_alpha %.6f\n", (double)_alpha);
	}

	/**
	 * Set filter state to an initial value
	 *
	 * @param sample new initial value
	 */
	void reset(const T &sample) { _filter_state = sample; }

	/**
	 * Add a new raw value to the filter
	 *
	 * @return retrieve the filtered result
	 */
	T apply(const T &sample)
	{
		const float new_filter_state = _alpha * _filter_state + (static_cast<T>(1) - _alpha) * sample;

		if (PX4_ISFINITE(new_filter_state)) {
			_filter_state = new_filter_state;
		}

		return _filter_state;
	}

	const T get() const { return _filter_state; }

protected:
	T _alpha{0};
	T _filter_state{0};
};

} // namespace math
