#ifndef _SPI_PIXY2_H
#define _SPI_PIXY2_H

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "fsl_dspi_cmsis.h"
#include "board.h"

#include "pin_mux.h"
#include "peripherals.h"
#include "clock_config.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

class SPI_Pixy2_Class {
public:
	SPI_Pixy2_Class() { }

	static void begin() { }

	static void send(uint8_t *buf, uint8_t len) {
		static dspi_transfer_t xfer;
		uint8_t rx_buf[len] = {0};
		xfer.rxData = rx_buf;
		xfer.txData = buf;
		xfer.dataSize = len;
		DSPI_MasterTransferBlocking(DSPI_0_PERIPHERAL, &xfer);
	}

	static void recv(uint8_t *buf, uint8_t len) {
		static dspi_transfer_t xfer;
		uint8_t tx_buf[len] = {0};
		xfer.rxData = buf;
		xfer.txData = tx_buf;
		xfer.dataSize = len;
		DSPI_MasterTransferBlocking(DSPI_0_PERIPHERAL, &xfer);
	}

	/* DSPI user SignalEvent */
	static void DSPI_MasterSignalEvent_t(uint32_t event) {
		if (true == isMasterOnReceive)
		    {
		        PRINTF("This is DSPI_MasterSignalEvent_t\r\n");
		        PRINTF("Master receive data from slave has completed!\r\n");
		        isMasterOnReceive = false;
		    }
		    if (true == isMasterOnTransmit)
		    {
		        PRINTF("This is DSPI_MasterSignalEvent_t\r\n");
		        PRINTF("Master transmit data to slave has completed!\r\n");
		        isMasterOnTransmit = false;
		    }
		    isTransferCompleted = true;
	}

private:
	volatile static bool isTransferCompleted;
	volatile static bool isMasterOnTransmit;
	volatile static bool isMasterOnReceive;
};

extern SPI_Pixy2_Class SPI;

#endif // _SPI_H
