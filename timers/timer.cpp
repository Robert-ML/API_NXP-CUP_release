// Copyright 2021 Lica Robert-Mihai <robertlica21@gmail.com>

#include "./timer.hpp"

#include <vector>

#include "fsl_debug_console.h"
#include "fsl_ftm.h"

#include "fsl_gpio.h"
#include "pin_mux.h"

Timer& Timer::get_instance() {
  static Timer instance;
  return instance;
}

void Timer::init() {
	ftm_config_t ftmInfo;
	FTM_GetDefaultConfig(&ftmInfo);
	ftmInfo.prescale = Timer::get_prescale_t(FTM_PRESCALLER);
	/* Initialize FTM module */
	FTM_Init(TIMER_FTM_BASEADDR, &ftmInfo);
	/*
	 * Set timer period.
	 */
	FTM_SetTimerPeriod(TIMER_FTM_BASEADDR, (uint32_t) 0xFFFFU);
	// start from 0
	TIMER_FTM_BASEADDR->CNTIN = 0;
	FTM_EnableInterrupts(TIMER_FTM_BASEADDR, kFTM_TimeOverflowInterruptEnable);
	EnableIRQ(TIMER_FTM_IRQ_NUM);
	PRINTF("TIMER_FTM_SOURCE_CLOCK: %ld\n", TIMER_FTM_SOURCE_CLOCK);
	this->start_timer();
}

Timer::Timer() {
	this->init();
}

void Timer::start_timer() {
  FTM_StartTimer(TIMER_FTM_BASEADDR, kFTM_SystemClock);
}

void Timer::stop_timer() {
  FTM_StopTimer(TIMER_FTM_BASEADDR);
}

uint32_t Timer::micros() {
	return oflow_cnt * oflow_real_time_us + FTM_GetCurrentTimerCount(TIMER_FTM_BASEADDR) * scaler;
}

uint32_t Timer::millis() {
	return oflow_cnt * oflow_real_time_us / 1000 + FTM_GetCurrentTimerCount(TIMER_FTM_BASEADDR) * scaler / 1000;
}

void Timer::delayMicroseconds(uint32_t t) {
	uint32_t start = micros();
	while (micros() - start < t) {
		for (uint32_t i = 0; i < 8000; ++i) {
			__asm volatile ("nop");
		}
	}
}

void Timer::delay(uint32_t t) {
	uint32_t start = millis();
	while (millis() - start < t) {
		for (uint32_t i = 0; i < 8000; ++i) {
			__asm volatile ("nop");
		}
	}
}

uint32_t Timer::add_irq_handler(void (*time_IRQn_handler)(void)) {
	uint32_t index = irq_handlers.size();
	irq_handlers.push_back(time_IRQn_handler);
	return index;
}

void Timer::call_irq_handlers() {
  for (auto& x : irq_handlers) {
    x();
  }
}

ftm_clock_prescale_t Timer::get_prescale_t(uint8_t div) {
	switch (div) {
		case   1: return kFTM_Prescale_Divide_1;   /*!< Divide by 1 */
		case   2: return kFTM_Prescale_Divide_2;   /*!< Divide by 2 */
		case   4: return kFTM_Prescale_Divide_4;   /*!< Divide by 4 */
		case   8: return kFTM_Prescale_Divide_8;   /*!< Divide by 8 */
		case  16: return kFTM_Prescale_Divide_16;  /*!< Divide by 16 */
		case  32: return kFTM_Prescale_Divide_32;  /*!< Divide by 32 */
		case  64: return kFTM_Prescale_Divide_64;  /*!< Divide by 64 */
		case 128: return kFTM_Prescale_Divide_128; /*!< Divide by 128 */
		default: return kFTM_Prescale_Divide_1;
	}
}

//*****************************************************************************
// The interrupt handler
#if defined (__cplusplus)
extern "C" {
#endif

void TIMER_FTM_HANDLER() {
	static uint32_t pass = 1;

	Timer& t = Timer::get_instance();
	++t.oflow_cnt;

	if (t.call_irq_oflow_cnt * pass < t.oflow_cnt) {
		++pass;
		t.call_irq_handlers();
	}

	FTM_ClearStatusFlags(TIMER_FTM_BASEADDR, kFTM_TimeOverflowFlag);
    __DSB();
}

#if defined (__cplusplus)
}
#endif

