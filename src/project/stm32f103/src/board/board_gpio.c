#include "drv_gpio.h"

void BOARD_GPIO_Init(void)
{
    // DEV_GPIO_Config_t pinConfig = {
    //     .mode = DEV_GPIO_MODE_OUTPUT,
    //     .type = DEV_GPIO_TYPE_PP_UP,
    //     .initVal = DEV_GPIO_LEVEL_HIGH,
    // };

    // DRV_GPIO_PinConfig(GPIOC, GPIO_PIN_13, &pinConfig);
}
MDS_INIT_IMPORT(MDS_INIT_PRIORITY_0, BOARD_GPIO_Init);
