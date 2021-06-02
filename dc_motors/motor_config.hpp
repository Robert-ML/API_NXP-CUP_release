#ifndef MOTOR_CONFIG_HPP_
#define MOTOR_CONFIG_HPP_

#include "MK64F12.h"
#include "fsl_ftm.h"
#include "../PID/PID_config.hpp"

struct motor_config_t {
  FTM_Type *FTM_base;
  clock_name_t clock_src;
  uint32_t mot_pwm_freq;

  ftm_config_t ftm_info;
  ftm_pwm_mode_t pwm_mode;
  ftm_chnl_pwm_signal_param_t *param_mx_in;

  bool MA_direction; // left
  bool MB_direction; // right
  int32_t max_command;
  int32_t mid_command;
  int32_t min_command;

  struct PID_config_t pid_conf;

  void give_command(uint8_t ma_in1,
      uint8_t ma_in2,
      uint8_t mb_in1,
      uint8_t mb_in2);
  void update_motor_dc();
  void update_motor_chnl_edge_level_select();
};

struct motor_config_t motor_get_default_config();


#endif // MOTOR_CONFIG_HPP_
