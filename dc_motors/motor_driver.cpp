// Copyright 2021 Lica Robert-Mihai <robertlica21@gmail.com>
#include "./motor_driver.hpp"

#include "./motor_config.hpp"
#include "../speed_sensor/speed_sensor.hpp"
#include "../timers/timer.hpp"

#include "fsl_ftm.h"
#include "fsl_gpio.h"
#include "fsl_debug_console.h"

struct motor_config_t Motors::motor_conf = motor_get_default_config();

Motors& Motors::get_instance() {
  static Motors instance;
  return instance;
}

Motors::Motors()
    : interrupt_motors(false),
      en_mot(false), d1_ma(true), d1_mb(true),
      command(0), rps_target(0),
      pid(Motors::motor_conf.pid_conf) {
  init();
}

void Motors::init() {
  FTM_GetDefaultConfig(&Motors::motor_conf.ftm_info);
  /* Initialize FTM module */
  FTM_Init(Motors::motor_conf.FTM_base, &Motors::motor_conf.ftm_info);

  FTM_SetupPwm(
      Motors::motor_conf.FTM_base,
      Motors::motor_conf.param_mx_in,
      sizeof(Motors::motor_conf.param_mx_in),
      Motors::motor_conf.pwm_mode,
      Motors::motor_conf.mot_pwm_freq,
      CLOCK_GetFreq(Motors::motor_conf.clock_src));

  FTM_StartTimer(Motors::motor_conf.FTM_base, kFTM_SystemClock);

  	 this->set_command(this->command);

  // initiate the motor speed sensor
  SpS::get_instance();
  // PID
  Timer::get_instance().add_irq_handler(this->pid_update);
}

void Motors::set_command(int8_t command) {
  command = std::max(
      std::min((int32_t) command, Motors::motor_conf.pid_conf.max_command),
          Motors::motor_conf.pid_conf.min_command);

  if (this->command == command) {
    return;
  }
  this->command = command;

  // transform from speed to duty cycle
  int32_t zero_command = Motors::motor_conf.mid_command;
  if (command > zero_command) {
    Motors::motor_conf.give_command(command, zero_command, command, zero_command);
  } else if (command < zero_command) {
    command = std::abs(command);
    Motors::motor_conf.give_command(zero_command, command, zero_command, command);
  } else { // speed == 0
    Motors::motor_conf.give_command(zero_command, zero_command, zero_command, zero_command);
  }

//  /* Disable H-Bridge output */
//  bool output_was_disabled = d1_ma || d1_mb;
//  disable_motor_outputs();

  /* Disable channel output before updating the dutycycle */
  FTM_Type *FTM_base = Motors::motor_conf.FTM_base;
  FTM_UpdateChnlEdgeLevelSelect(
      FTM_base,
      Motors::motor_conf.param_mx_in[0].chnlNumber,
      kFTM_NoPwmSignal);
  FTM_UpdateChnlEdgeLevelSelect(
      FTM_base,
      Motors::motor_conf.param_mx_in[1].chnlNumber,
      kFTM_NoPwmSignal);
  FTM_UpdateChnlEdgeLevelSelect(
      FTM_base,
      Motors::motor_conf.param_mx_in[2].chnlNumber,
      kFTM_NoPwmSignal);
  FTM_UpdateChnlEdgeLevelSelect(
      FTM_base,
      Motors::motor_conf.param_mx_in[3].chnlNumber,
      kFTM_NoPwmSignal);

  /* Update PWM duty cycle */
  Motors::motor_conf.update_motor_dc();

  /* Software trigger to update registers */
  FTM_SetSoftwareTrigger(FTM_base, true);

  /* Start channel output with updated dutycycle */
  Motors::motor_conf.update_motor_chnl_edge_level_select();

//  /* Enable H-Bridge output if it was enable to begin with */
//  if (output_was_disabled == false) {
//    this->enable_motor_outputs();
//  }
}

void Motors::stop_interrupt(const bool state) {
  static bool _en_mot;
  static bool _d1_ma;
  static bool _d1_mb;
  if (state == true) {
    PRINTF("***KILL TRUE\n");
    _en_mot = en_mot;
    _d1_ma = d1_ma;
    _d1_mb = d1_mb;
    interrupt_motors = true;
    this->pid.m_command = 0;
    this->set_command(0);
    if (d1_ma == false || d1_mb == false) {
      disable_motor_outputs();
    }
    if (en_mot == true) {
      disable_motor_driver();
    }
  } else if (state == false) {
	PRINTF("***KILL FALSE\n");
    if (interrupt_motors == true) {
		if (_en_mot == true) {
		  enable_motor_driver();
		}
		if (_d1_ma == false && _d1_mb == false) {
		  enable_motor_outputs();
		}
    }
    interrupt_motors = false;
  } else {
    PRINTF("***KILL UNKNOWN %d\n", state);
  }
}

void Motors::pid_update() {
  Motors& m = Motors::get_instance();
  SpS& ss = SpS::get_instance();

  if (m.interrupt_motors == true || m.pid.active == false) return;

  PRINTF("Read: %d | Error: %d | ", (int)ss.get_rps_a(), (int)(m.rps_target - ss.get_rps_a()));
  m.pid.update(m.rps_target - ss.get_rps_a());
  m.set_command(m.pid.m_command);
  PRINTF("command: %d\n", (int)m.pid.m_command);
}

void Motors::enable_motor_driver() {
  GPIO_PinWrite(BOARD_INITPINS_EN_MOT_GPIO, BOARD_INITPINS_EN_MOT_PIN, 1);
  en_mot = true;
}
void Motors::disable_motor_driver() {
  this->rps_target = 0;
  GPIO_PinWrite(BOARD_INITPINS_EN_MOT_GPIO, BOARD_INITPINS_EN_MOT_PIN, 0);
  en_mot = false;
}
void Motors::enable_motor_outputs() {
  GPIO_PinWrite(BOARD_INITPINS_D1_MA_GPIO, BOARD_INITPINS_D1_MA_PIN, 0);
  GPIO_PinWrite(BOARD_INITPINS_D1_MB_GPIO, BOARD_INITPINS_D1_MB_PIN, 0);
  d1_ma = false;
  d1_mb = false;
}
void Motors::disable_motor_outputs() {
  this->rps_target = 0;
  GPIO_PinWrite(BOARD_INITPINS_D1_MA_GPIO, BOARD_INITPINS_D1_MA_PIN, 1);
  GPIO_PinWrite(BOARD_INITPINS_D1_MB_GPIO, BOARD_INITPINS_D1_MB_PIN, 1);
  d1_ma = true;
  d1_mb = true;
}
void Motors::update_motor_status() {
  GPIO_PinWrite(BOARD_INITPINS_EN_MOT_GPIO, BOARD_INITPINS_EN_MOT_PIN, en_mot);
  GPIO_PinWrite(BOARD_INITPINS_D1_MA_GPIO, BOARD_INITPINS_D1_MA_PIN, d1_ma);
  GPIO_PinWrite(BOARD_INITPINS_D1_MB_GPIO, BOARD_INITPINS_D1_MB_PIN, d1_mb);
}

