// Copyright 2021 Lica Robert-Mihai <robertlica21@gmail.com>
#include "./servo.hpp"

#include <cmath>
#include "fsl_ftm.h"
#include "pin_mux.h"

Servo& Servo::get_instance() {
	static Servo instance;
	return instance;
}

Servo::Servo() : sv1(SV1), sv1_min(fakeMG996R_min), sv1_max(fakeMG996R_max),
				 sv2(SV2), sv2_min(noServo), sv2_max(noServo) {
	init();
}

void Servo::init() {
	pwm_level = kFTM_HighTrue;
	pwm_mode = kFTM_EdgeAlignedPwm;

	uint8_t iterator = 0;
	if (sv1) {
		sv1_angle = SV_DEFAULT_POS;
		param_svx_inn[iterator].chnlNumber 			  = SV1_FTM_CHANNEL;
		param_svx_inn[iterator].level 				  = pwm_level;
		param_svx_inn[iterator].dutyCyclePercent 	  = 0U;
		param_svx_inn[iterator].firstEdgeDelayPercent = 0U;
		++iterator;
	}
	if (sv2) {
		sv2_angle = SV_DEFAULT_POS;
		param_svx_inn[iterator].chnlNumber 			  = SV2_FTM_CHANNEL;
		param_svx_inn[iterator].level 				  = pwm_level;
		param_svx_inn[iterator].dutyCyclePercent 	  = 0U;
		param_svx_inn[iterator].firstEdgeDelayPercent = 0U;
		++iterator;
	}

	int ret;

	FTM_GetDefaultConfig(&ftm_info);
	ftm_info.prescale = FTM_PRESCALER;
	/* Initialize FTM module */
	ret = FTM_Init(SV_FTM_BASEADDR, &ftm_info);
	if (ret != kStatus_Success) {
		PRINTF("Error in Servo: failed the initialization\n");
	}

	ret = FTM_SetupPwm(SV_FTM_BASEADDR, param_svx_inn, iterator, pwm_mode, sv_pwm_freq, FTM_SOURCE_CLOCK);
	if (ret != kStatus_Success) {
		PRINTF("Error in Servo: failed to set the pwm\n");
	}

	FTM_StartTimer(SV_FTM_BASEADDR, kFTM_FixedClock);

	if (sv1) {
		this->set_angle(SV1_FTM_CHANNEL, SV_DEFAULT_POS);
	}
	if (sv2) {
		this->set_angle(SV2_FTM_CHANNEL, SV_DEFAULT_POS);
	}
}

void Servo::set_angle(ftm_chnl_t sv, uint16_t angle) {
	if (sv == SV1_FTM_CHANNEL && sv1 == false) return;
	if (sv == SV2_FTM_CHANNEL && sv2 == false) return;
	if (sv != SV1_FTM_CHANNEL && sv != SV2_FTM_CHANNEL) return;

//	uint8_t sv_min;
	uint8_t sv_max;
	if (sv == SV1_FTM_CHANNEL) {
//		sv_min = sv1_min;
		sv_max = sv1_max;
	} else if (sv == SV2_FTM_CHANNEL){
//		sv_min = sv2_min;
		sv_max = sv2_max;
	}

	if (angle > sv_max) {
		angle = sv_max;
	}

	uint16_t mod = SV_FTM_BASEADDR->MOD;

	float cnv = mod * 0.05;
	float temp = (1.0 * sv_max) / angle;
	cnv = cnv + cnv / temp;

	uint16_t cnv_i = (uint16_t) cnv;
//	PRINTF("Cnv: %d\n", cnv_i);
	if (sv == SV1_FTM_CHANNEL) {
		sv1_angle = angle;
	} else {
		sv2_angle = angle;
	}
	this->set_directly(sv, cnv_i);
}

void Servo::set_dutycycle(ftm_chnl_t sv, uint8_t dc) {
	FTM_UpdateChnlEdgeLevelSelect(SV_FTM_BASEADDR, sv, kFTM_NoPwmSignal);

	FTM_UpdatePwmDutycycle(SV_FTM_BASEADDR, SV1_FTM_CHANNEL, pwm_mode, dc);

	FTM_SetSoftwareTrigger(SV_FTM_BASEADDR, true);

	FTM_UpdateChnlEdgeLevelSelect(SV_FTM_BASEADDR, sv, pwm_level);
}

void Servo::set_directly(ftm_chnl_t sv, uint16_t cnv) {
	FTM_UpdateChnlEdgeLevelSelect(SV_FTM_BASEADDR, sv, kFTM_NoPwmSignal);

	SV_FTM_BASEADDR->CONTROLS[sv].CnV = cnv;

	FTM_SetSoftwareTrigger(SV_FTM_BASEADDR, true);

	FTM_UpdateChnlEdgeLevelSelect(SV_FTM_BASEADDR, sv, pwm_level);
}
