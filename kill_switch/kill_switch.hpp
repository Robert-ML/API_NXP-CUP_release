// Copyright 2021 Lica Robert-Mihai <robertlica21@gmail.com>
#ifndef KILL_SWITCH_HPP_
#define KILL_SWITCH_HPP_

#define KILL_SWITCH_ENABLE 1 // to enable the kill_switch
//#define KILL_SWITCH_ENABLE 0 // to disable the kill_switch

#if KILL_SWITCH_ENABLE
/*! @name PORTA13 (number 43), KILL_SWITCH
  @{ */
#define BOARD_INITPINS_KILL_SWITCH_GPIO GPIOA /*!<@brief GPIO device name: GPIOA */
#define BOARD_INITPINS_KILL_SWITCH_PORT PORTA /*!<@brief PORT device name: PORTA */
#define BOARD_INITPINS_KILL_SWITCH_PIN 13U    /*!<@brief PORTA pin index: 13 */
                                              /* @} */
#endif

void kill_switch_init();
void kill_switch_interrupt();

#endif /* KILL_SWITCH_HPP_ */
