#ifndef __BOARD_H__
#define __BOARD_H__

#include "mds_lpc.h"
#include "drv_chip.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void BOARD_CLOCK_Init(void);

#define FLASH_MODULE "flash"
#define GPIO_MODULE  "gpio"
#define I2C1_MODULE  "i2c1"

#define FLASH_NV  "flashNV"
#define FLASH_UV  "flashUV"
#define FLASH_DFT "flashDFT"

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H__ */
