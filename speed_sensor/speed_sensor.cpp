// Copyright 2021 Lica Robert-Mihai <robertlica21@gmail.com>

#include "./speed_sensor.hpp"

#include <stdio.h>
#include "fsl_debug_console.h"
#include "pin_mux.h"

#include "../timers/timer.hpp"

SpS& SpS::get_instance() {
	static SpS instance;
	return instance;
}

SpS::SpS()
    : raw_speed_a(0),
      raw_speed_b(0) {}

float SpS::get_rps_a() {
#if SPS_MOT_A
  return raw_speed_a / signal_angle;
#else
#if SPS_MOT_B
  return raw_speed_b / signal_angle;
#else
  return 0;
#endif
#endif
}

float SpS::get_rps_b() {
#if SPS_MOT_B
  return raw_speed_b / signal_angle;
#else
#if SPS_MOT_A
  return raw_speed_a / signal_angle;
#else
  return 0;
#endif
#endif
}

//*****************************************************************************
// Called by the interrupts

void SpS::sensor_interrupt() {
#if SPS_MOT_A
  static unsigned long old_tick_A = 0;

  if (PORTA->ISFR & (1 << BOARD_INITPINS_MOT_A_SPEED_A_PIN)) {
    uint32_t new_tick_A = Timer::get_instance().micros();

    if (SPS_MOT_A_FORWARD(GPIOA->PDIR & (1 << BOARD_INITPINS_MOT_A_SPEED_B_PIN))) {
      SpS::get_instance().sensor_mot_a_speed(1000000 / (new_tick_A - old_tick_A));
    } else {
      SpS::get_instance().sensor_mot_a_speed(-((int32_t)(1000000 / (new_tick_A - old_tick_A))));
    }
    old_tick_A = new_tick_A;

    PORTA->ISFR |= (1 << BOARD_INITPINS_MOT_A_SPEED_A_PIN);
  }
#endif

#if SPS_MOT_B
  static unsigned long old_tick_B = 0;

  if (PORTA->ISFR & (1 << BOARD_INITPINS_MOT_B_SPEED_A_PIN)) {
    uint32_t new_tick_B = Timer::get_instance().micros();

    if (SPS_MOT_A_FORWARD(GPIOA->PDIR & (1 << BOARD_INITPINS_MOT_B_SPEED_B_PIN))) {
      SpS::get_instance().sensor_mot_b_speed(1000000 / (new_tick_B - old_tick_B));
    } else {
      SpS::get_instance().sensor_mot_b_speed(-((int32_t)(1000000 / (new_tick_B - old_tick_B))));
    }
    old_tick_B = new_tick_B;

    PORTA->ISFR |= (1 << BOARD_INITPINS_MOT_B_SPEED_A_PIN);
  }
#endif
}
