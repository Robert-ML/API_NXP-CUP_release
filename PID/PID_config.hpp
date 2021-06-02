#ifndef PID_CONFIG_HPP_
#define PID_CONFIG_HPP_

struct PID_config_t {
  bool active;
  float kp;
	float ki;
	float kd;
	float max_integral;
  int32_t max_command;
  int32_t min_command;
};

#endif // PID_CONFIG_HPP_
