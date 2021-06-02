// Copyright 2021 Lica Robert-Mihai <robertlica21@gmail.com>
// Speed Sensor class for a 2 sensor encoder with A and B
#ifndef SPEED_SENSOR_HPP_
#define SPEED_SENSOR_HPP_

#include <stdio.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define SPS_MOT_A 1
#define SPS_MOT_B 0

#if SPS_MOT_A
#define SPS_MOT_A_FORWARD(x) (x)
#endif
#if SPS_MOT_B
#define SPS_MOT_B_FORWARD(x) (x)
#endif

class SpS {
	SpS();

 public:
	// SpS is not copyable
	SpS(const SpS&) = delete;
	SpS& operator=(const SpS&) = delete;

	static SpS& get_instance();

	float get_rps_a();
	float get_rps_b();

	void sensor_interrupt();

 private:
// only called on the rising edge, if called on the falling edge, the direction will be negated
	void sensor_mot_a_speed(int32_t speed) { raw_speed_a = speed; }
	void sensor_mot_b_speed(int32_t speed) { raw_speed_b = speed; }

	const int no_magnets = 9;
	const float signal_angle = 360.0f / no_magnets;

	volatile int32_t raw_speed_a;
	volatile int32_t raw_speed_b;
};

/*******************************************************************************
 * Undefines
 ******************************************************************************/

#endif /* SPEED_SENSOR_HPP_ */
