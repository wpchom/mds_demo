#ifndef __BOARD_H__
#define __BOARD_H__

#include "mds_lpc.h"
#include "drv_chip.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CONFIG_BOARD_LOG_LEVEL MDS_LOG_LEVEL_WRN

void BOARD_CLOCK_Init(void);

#define GPIO_MODULE "gpio"
#define QSPI_MODULE "qspi"
#define SPI1_MODULE "spi1"

#define QSPI_FLASH_DEVICE "xip"
#define SPI1_FLASH_DEVICE "exflash"

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H__ */
