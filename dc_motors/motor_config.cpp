#include "./motor_config.hpp"

#include "fsl_debug_console.h"

static struct PID_config_t __motor_pid_get_default_config(
    const int32_t max_command,
    const int32_t min_command);

void motor_config_t::give_command(uint8_t ma_in1,
      uint8_t ma_in2,
      uint8_t mb_in1,
      uint8_t mb_in2) {
    if (MA_direction) {
      param_mx_in[0].dutyCyclePercent = ma_in1;
      param_mx_in[3].dutyCyclePercent = ma_in2;
    } else {
      param_mx_in[0].dutyCyclePercent = ma_in2;
      param_mx_in[3].dutyCyclePercent = ma_in1;
    }
    if (MB_direction) {
      param_mx_in[1].dutyCyclePercent = mb_in1;
      param_mx_in[2].dutyCyclePercent = mb_in2;
    } else {
      param_mx_in[1].dutyCyclePercent = mb_in2;
      param_mx_in[2].dutyCyclePercent = mb_in1;
    }
}

void motor_config_t::update_motor_dc() {
  // TO_DO: test the i bound
  for (size_t i = 0; i < sizeof(param_mx_in); ++i) {
    FTM_UpdatePwmDutycycle(FTM_base, param_mx_in[i].chnlNumber,
        pwm_mode, param_mx_in[i].dutyCyclePercent);
  }
}

void motor_config_t::update_motor_chnl_edge_level_select() {
  // TO_DO: test the i bound
  for (size_t i = 0; i < sizeof(param_mx_in); ++i) {
    FTM_UpdateChnlEdgeLevelSelect(FTM_base,
        param_mx_in[i].chnlNumber,
        param_mx_in[i].level);
  }
}

struct motor_config_t motor_get_default_config() {
  const ftm_chnl_t MA_IN1_FTM_CHANNEL = kFTM_Chnl_0;
  const ftm_chnl_t MB_IN1_FTM_CHANNEL = kFTM_Chnl_1;
  const ftm_chnl_t MB_IN2_FTM_CHANNEL = kFTM_Chnl_2;
  const ftm_chnl_t MA_IN2_FTM_CHANNEL = kFTM_Chnl_3;

  const ftm_pwm_level_select_t pwm_level = kFTM_HighTrue;
  const int32_t max_command = 100;
  const int32_t mid_command = 0;
  const int32_t min_command = -100;

  static ftm_chnl_pwm_signal_param_t param_mx_in[] = {
    {
      .chnlNumber = MA_IN1_FTM_CHANNEL,
      .level = pwm_level,
      .dutyCyclePercent = 0U,
      .firstEdgeDelayPercent = 0U
    },
    {
      .chnlNumber = MB_IN1_FTM_CHANNEL,
      .level = pwm_level,
      .dutyCyclePercent = 0U,
      .firstEdgeDelayPercent = 0U
    },
    {
      .chnlNumber = MB_IN2_FTM_CHANNEL,
      .level = pwm_level,
      .dutyCyclePercent = 0U,
      .firstEdgeDelayPercent = 0U
    },
    {
      .chnlNumber = MA_IN2_FTM_CHANNEL,
      .level = pwm_level,
      .dutyCyclePercent = 0U,
      .firstEdgeDelayPercent = 0U
    }
  };

  const struct motor_config_t value = {
    .FTM_base = ((FTM_Type *)FTM0_BASE),
    .clock_src = kCLOCK_BusClk,
    .mot_pwm_freq = 4000U,

    .pwm_mode = kFTM_EdgeAlignedPwm,
    .param_mx_in = param_mx_in,

    .MA_direction = true,
    .MB_direction = false,
    .max_command = max_command,
    .mid_command = mid_command,
    .min_command = min_command,

    .pid_conf = __motor_pid_get_default_config(
        max_command, min_command),
  };
  return value;
}

struct PID_config_t __motor_pid_get_default_config(
    const int32_t max_command,
    const int32_t min_command) {
  const struct PID_config_t value = {
    .active = true,
    .kp = 1.1f,
    .ki = 1.1f,
    .kd = 0.0f,
    .max_integral = 50,
    .max_command = max_command,
    .min_command = min_command
  };
  return value;
}
