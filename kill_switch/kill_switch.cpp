// Copyright 2021 Lica Robert-Mihai <robertlica21@gmail.com>
#include "./kill_switch.hpp"

#if KILL_SWITCH_ENABLE
#include "fsl_gpio.h"
#include "pin_mux.h"
#include "fsl_port.h"

#include "../dc_motors/motor_driver.hpp"

void kill_switch_init() {
	/* Port A Clock Gate Control: Clock enabled */
	CLOCK_EnableClock(kCLOCK_PortA);

    gpio_pin_config_t KILL_SWITCH_config = {
        .pinDirection = kGPIO_DigitalInput,
        .outputLogic = 0U
    };
    /* Initialize GPIO functionality on pin PTA13 (pin 43)  */
    GPIO_PinInit(BOARD_INITPINS_KILL_SWITCH_GPIO, BOARD_INITPINS_KILL_SWITCH_PIN, &KILL_SWITCH_config);

    /* PORTA13 (pin 43) is configured as PTA13 */
	PORT_SetPinMux(BOARD_INITPINS_KILL_SWITCH_PORT, BOARD_INITPINS_KILL_SWITCH_PIN, kPORT_MuxAsGpio);

	/* Interrupt configuration on PORTA13 (pin 43): Interrupt on either edge */
	PORT_SetPinInterruptConfig(BOARD_INITPINS_KILL_SWITCH_PORT, BOARD_INITPINS_KILL_SWITCH_PIN, kPORT_InterruptEitherEdge);

	PORTA->PCR[13] = ((PORTA->PCR[13] &
					   /* Mask bits to zero which are setting */
					   (~(PORT_PCR_PS_MASK | PORT_PCR_PE_MASK | PORT_PCR_ISF_MASK)))

					  /* Pull Select: Internal pullup resistor is enabled on the corresponding pin, if the
					   * corresponding PE field is set. */
					  | (uint32_t)(kPORT_PullUp));
}

void kill_switch_interrupt() {
	if (BOARD_INITPINS_KILL_SWITCH_PORT->ISFR & (1 << BOARD_INITPINS_KILL_SWITCH_PIN)) {
		if (GPIO_PinRead(BOARD_INITPINS_KILL_SWITCH_GPIO, BOARD_INITPINS_KILL_SWITCH_PIN)) {
			Motors::get_instance().stop_interrupt(true);
			GPIO_PinWrite(BOARD_INITPINS_LED2_GPIO, BOARD_INITPINS_LED2_PIN, 1);
		} else {
			Motors::get_instance().stop_interrupt(false);
			GPIO_PinWrite(BOARD_INITPINS_LED2_GPIO, BOARD_INITPINS_LED2_PIN, 0);
		}
		BOARD_INITPINS_KILL_SWITCH_PORT->ISFR &= (1 << BOARD_INITPINS_KILL_SWITCH_PIN);
	}
}

#else
void kill_switch_init() { }
void kill_switch_interrupt() { }
#endif
