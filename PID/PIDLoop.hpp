//
// begin license header
//
// This file is part of Pixy CMUcam5 or "Pixy" for short
//
// All Pixy source code is provided under the terms of the
// GNU General Public License v2 (http://www.gnu.org/licenses/gpl-2.0.html).
// Those wishing to use Pixy source code, software and/or
// technologies under different licensing terms should contact us at
// cmucam@cs.cmu.edu. Such licensing terms are available for
// all portions of the Pixy codebase presented here.
//
// end license header
//

#ifndef PID_PIDLOOP_HPP
#define PID_PIDLOOP_HPP

#include "fsl_debug_console.h"

#include "./PID_config.hpp"

class PIDLoop {
 public:
  PIDLoop(struct PID_config_t conf);

  void update(int32_t error);

  bool active;
  volatile float m_command;

 private:
  float m_integral;
  float m_pgain;
  float m_igain;
  float m_dgain;
  float max_integral;

  int32_t max_command;
  int32_t min_command;
  int32_t m_prevError;
};

#endif

