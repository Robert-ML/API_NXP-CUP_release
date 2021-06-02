#include "./PID/PIDLoop.hpp"

#include <algorithm>
#include <cmath>

PIDLoop::PIDLoop(struct PID_config_t conf)
    : active(conf.active),
      m_command(0),
      m_integral(0),
      m_pgain(conf.kp),
      m_igain(conf.ki),
      m_dgain(conf.kd),
      max_integral(conf.max_integral),
      max_command(conf.max_command),
      min_command(conf.min_command),
      m_prevError(0) {}

void PIDLoop::update(int32_t error) {
  float pid = 0.0f;

  // integrate integral
  m_integral += error;
  // bound the integral
  m_integral = std::max(std::min(m_integral, max_integral), -max_integral);

  // calculate PID term
  // pid = (error*m_pgain + ((m_integral*m_igain)>>4) + (error - m_prevError)*m_dgain)>>10;
  pid = (error*m_pgain + (m_integral*m_igain) + (error - m_prevError)*m_dgain);

  pid = std::max(std::min((int32_t)pid, max_command), min_command);

  m_command = pid;

  // retain the previous error val so we can calc the derivative
  m_prevError = error;
}
