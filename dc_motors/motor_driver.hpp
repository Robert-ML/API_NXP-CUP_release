// Copyright 2021 Lica Robert-Mihai <robertlica21@gmail.com>
#ifndef MOTOR_DRIVER_HPP_
#define MOTOR_DRIVER_HPP_

#include "fsl_ftm.h"
#include "fsl_gpio.h"
#include "pin_mux.h"

#include "./motor_config.hpp"
#include "../PID/PIDLoop.hpp"

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
class Motors{
 public:
/*!
 * FTM0 and channels 0, 1, 2 and 3 are used. Reinitialization of the FTM0 driver
 * will result in undefined behaviour.
 */
  static Motors& get_instance();
  static void give_config(struct motor_config_t& conf) { Motors::motor_conf = conf; }

  // Motor control
  void enable_motor_driver();
  void disable_motor_driver();
  void enable_motor_outputs();
  void disable_motor_outputs();
  void update_motor_status();

  // give demand in rotations per second
  void set_rps_target(const float rps) { this->rps_target = rps; }
  float get_rps_target() { return this->rps_target; }
  // PID
  void enable_pid_control(const int8_t new_command = 0) {
	  if (this->pid.active == true) return;
	  this->pid.active = true;
	  this->rps_target = 0;
	  set_command(new_command);
  }
  void disable_pid_control() {
    if (this->pid.active == false) return;
	this->pid.active = false;
	this->rps_target = 0;
    set_command(0);
  }
  static void pid_update();
  // command
  void set_command(int8_t command);
  int8_t get_command() {return command; }
  

  void stop_interrupt(const bool state);

 private:
  Motors();
  Motors(const Motors& other) = delete;
  Motors& operator=(const Motors& other) = delete;

  void init();

  static struct motor_config_t motor_conf;

  volatile bool interrupt_motors = false;

  volatile bool en_mot = false;
  volatile bool d1_ma = true;
  volatile bool d1_mb = true;

  // check if volatile is needed
  volatile int8_t command = 0;
  // check if volatile is needed
  volatile float rps_target = 0;
  PIDLoop pid;
};

#endif /* MOTOR_DRIVER_HPP_ */
