// Copyright 2021 Lica Robert-Mihai <robertlica21@gmail.com>
#ifndef TIMER_HPP_
#define TIMER_HPP_

#include <vector>

#include "fsl_ftm.h"

#define TIME_INTERVAL_TO_CALL_IRQ_VECTOR 200000 // in us

/* The Flextimer instance/channel used for board */
#define TIMER_FTM_BASEADDR FTM1
/* Interrupt number and interrupt handler for the FTM instance used */
#define TIMER_FTM_IRQ_NUM FTM1_IRQn
#define TIMER_FTM_HANDLER FTM1_IRQHandler
#define FTM_PRESCALLER 64
#define TIMER_FTM_SOURCE_CLOCK (CLOCK_GetFreq(kCLOCK_CoreSysClk) / FTM_PRESCALLER)

// THe timer starts automaticly after you create the first instance
class Timer {
  std::vector<void (*)()> irq_handlers;

  Timer();
  void init();

  static ftm_clock_prescale_t get_prescale_t(uint8_t div);

 public:
  volatile uint32_t oflow_cnt = 0;

  const uint32_t freq_after_presc = 937500;
  const double scaler = 1.0 / (freq_after_presc / 1000000.0); // correct for the errors
  const uint32_t oflow_real_time_us = scaler * 0xFFFFU;
  const uint32_t call_irq_time_us = TIME_INTERVAL_TO_CALL_IRQ_VECTOR; // one second
  const uint32_t call_irq_oflow_cnt = call_irq_time_us / oflow_real_time_us; // how many times to loop before it calls the functions

  // Timer is not copyable
  Timer(const Timer&) = delete;
  Timer& operator=(const Timer&) = delete;

  static Timer& get_instance();

  void start_timer();
  void stop_timer();

  uint32_t micros();
  uint32_t millis();
  void delayMicroseconds(uint32_t t);
  void delay(uint32_t t);

  uint32_t add_irq_handler(void (*time_IRQn_handler)(void));
  void call_irq_handlers();

};

#endif /* TIMER_HPP_ */
