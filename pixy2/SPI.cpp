#include "./SPI.h"

void SPI_Pixy2_Class::begin() {
	isTransferCompleted = false;
	isMasterOnTransmit  = false;
	isMasterOnReceive   = false;

	/* DMA Mux init and EDMA init */
	edma_config_t edmaConfig = {0};
	EDMA_GetDefaultConfig(&edmaConfig);
	EDMA_Init(DSPI_MASTER_DMA_BASEADDR, &edmaConfig);
	DMAMUX_Init(DSPI_MASTER_DMA_MUX_BASEADDR);

	/*DSPI master init*/
	DRIVER_MASTER_SPI.Initialize(DSPI_MasterSignalEvent_t);
	DRIVER_MASTER_SPI.PowerControl(ARM_POWER_FULL);
	DRIVER_MASTER_SPI.Control(
			ARM_SPI_MODE_MASTER |
			ARM_SPI_CPOL1_CPHA1 |
			ARM_SPI_MSB_LSB 	|
			ARM_SPI_DATA_BITS(PIXY_SPI_FRAME_SIZE), PIXY_SPI_CLOCKRATE);
}

void SPI_Pixy2_Class::send(uint8_t *buf, uint8_t len) {
	DRIVER_MASTER_SPI.Send(buf, len);
}

void SPI_Pixy2_Class::recv(uint8_t *buf, uint8_t len) {
	DRIVER_MASTER_SPI.Receive(masterRxData, TRANSFER_SIZE);
	// Not needed right now because the pixy driver already waits for a response
	/* Wait until transfer completed */
//	while (!isTransferCompleted) {}
}

void SPI_Pixy2_Class::DSPI_MasterSignalEvent_t(uint32_t event) {
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
