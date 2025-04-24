#ifndef __BOARD_H__
#define __BOARD_H__

#include "mds_lpc.h"
#include "drv_chip.h"

#ifdef __cplusplus
extern "C" {
#endif

void BOARD_CLOCK_Init(void);

#define GPIO_MODULE "gpio"
#define I2C1_MODULE "i2c1"

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H__ */
