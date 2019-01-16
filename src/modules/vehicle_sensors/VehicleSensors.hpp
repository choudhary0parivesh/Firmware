/****************************************************************************
 *
 *   Copyright (c) 2012-2018 PX4 Development Team. All rights reserved.
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

#pragma once

#include <lib/perf/perf_counter.h>
#include <px4_config.h>
#include <px4_getopt.h>
#include <px4_log.h>
#include <px4_module.h>
#include <px4_module_params.h>
#include <matrix/matrix/math.hpp>
#include <mathlib/math/Limits.hpp>
#include <mathlib/math/Functions.hpp>
#include <conversion/rotation.h>


#include <lib/work_queue/WorkItem.hpp>
#include <uORB/topics/sensor_bias.h>
#include <uORB/topics/sensor_correction.h>
#include <uORB/topics/sensor_gyro.h>
#include <uORB/topics/vehicle_rates.h>


#define MAX_GYRO_COUNT 3

class VehicleSensors : public ModuleBase<VehicleSensors>, public px4::WorkItem, public ModuleParams
{
public:

	VehicleSensors();
	virtual ~VehicleSensors();

	/** @see ModuleBase */
	static int task_spawn(int argc, char *argv[]);

	/** @see ModuleBase */
	static int custom_command(int argc, char *argv[]);

	/** @see ModuleBase */
	static int print_usage(const char *reason = nullptr);

	/** @see ModuleBase::run() */
	//void run() override;

	/**
	 * run the main loop: if running as task, continuously iterate, otherwise execute only one single cycle
	 */
	void Run() override;

	/** @see ModuleBase::print_status() */
	int print_status() override;

	int	init();

private:

	void sensor_correction_poll();
	void sensor_bias_poll();


	DEFINE_PARAMETERS(
		(ParamInt<px4::params::SENS_BOARD_ROT>) _board_rotation_param,

		(ParamFloat<px4::params::SENS_BOARD_X_OFF>) _board_offset_x,
		(ParamFloat<px4::params::SENS_BOARD_Y_OFF>) _board_offset_y,
		(ParamFloat<px4::params::SENS_BOARD_Z_OFF>) _board_offset_z
	)

	matrix::Dcmf _board_rotation;			/**< rotation matrix for the orientation that the board is mounted */

	sensor_gyro_s			_sensor_gyro {};	/**< gyro data before thermal correctons and ekf bias estimates are applied */
	sensor_correction_s		_sensor_correction {};	/**< sensor thermal corrections */
	sensor_bias_s			_sensor_bias {};	/**< sensor in-run bias corrections */

	int		_sensor_gyro_sub[MAX_GYRO_COUNT] {};	/**< gyro data subscription */
	int		_sensor_correction_sub{-1};	/**< sensor thermal correction subscription */
	int		_sensor_bias_sub{-1};		/**< sensor in-run bias correction subscription */

	orb_advert_t	_vehicle_rates_pub{nullptr};

	unsigned _gyro_count{1};
	int _selected_gyro{0};

	perf_counter_t      _cycle_perf;
	perf_counter_t      _sensor_gyro_latency_perf;

};