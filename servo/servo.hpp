// Copyright 2021 Lica Robert-Mihai <robertlica21@gmail.com>
/*
 * Warning: uses FTM2.
 */

#ifndef SERVO_HPP_
#define SERVO_HPP_

#include <stdio.h>
#include "fsl_debug_console.h"
#include "fsl_ftm.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* The Flextimer instance/channel used for board */
#define SV_FTM_BASEADDR FTM2
#define SV1_FTM_CHANNEL kFTM_Chnl_0
#define SV2_FTM_CHANNEL kFTM_Chnl_1

/* Get source clock for FTM driver */
#define FTM_SOURCE_CLOCK CLOCK_GetFreq(kCLOCK_McgFixedFreqClk)
#define FTM_PRESCALER kFTM_Prescale_Divide_1
#define sv_pwm_freq 50U

/* Constants */
#define SV_NOT_INIT 		  -100
#define SV_DEFAULT_POS 		  90
#define SV_MIN_ANG 			  0
#define SV_MID_ANG 			  SV_DEFAULT_POS
#define SV_MAX_ANG 			  180
#define SV_BASE_DC			  5      // 5% is the base dc in the 0 degree position
#define SV_DC_PROC_PER_DEGREE 0.0276 // 0.0276% of the dc is 1 degree of the angle
#define SV_ONE_PROC_IN_DEGREE 36.2   // 36.2 degrees per 1% of the dc

// no servo
const uint16_t noServo = 1;

// fakeMG996R
const uint16_t fakeMG996R_min = 0;
const uint16_t fakeMG996R_max = 180;

#define SV1 true
#define SV2 false

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
class Servo{
	bool sv1;
	uint16_t sv1_min, sv1_max;
	uint16_t sv1_angle = SV_NOT_INIT;

	bool sv2;
	uint16_t sv2_min, sv2_max;
	uint16_t sv2_angle = SV_NOT_INIT;

	ftm_config_t ftm_info;
	ftm_pwm_mode_t pwm_mode;
	ftm_pwm_level_select_t pwm_level;
	ftm_chnl_pwm_signal_param_t param_svx_inn[2];

	Servo();
	void init();

 public:
/*!
 *  Enables FTM2 and channels 0 and/or 1. Reinitialization of the FTM2 driver
 * will result in undefined behaviour.
 */
	static Servo& get_instance();

	void set_angle(ftm_chnl_t sv, uint16_t angle);
	void set_dutycycle(ftm_chnl_t sv, uint8_t dc);

private:
	void set_directly(ftm_chnl_t sv, uint16_t cnv);
};


#endif /* SERVO_HPP_ */
