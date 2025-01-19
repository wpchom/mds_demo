#include "drv_gpio.h"
#include "board.h"
#include "mds_log.h"

static const struct GPIO_Desc {
    DEV_GPIO_Object_t object;
    DEV_GPIO_Config_t config;
} G_GPIO_DESC[] = {
    [0x00] = {.object = {.parent = "PA0", .GPIOx = GPIOA, .pinMask = GPIO_PIN_0, .initVal = 0x01},  // PA0
              .config = {.mode = DEV_GPIO_MODE_INPUT, .type = DEV_GPIO_TYPE_PP_NO, .intr = DEV_GPIO_INTR_BOTH}},
};

static DEV_GPIO_Module_t g_moduleGPIO;
static DEV_GPIO_Pin_t g_pin[ARRAY_SIZE(G_GPIO_DESC)] = {0};

void BOARD_GPIO_Init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_AFIO_CLK_ENABLE();

    NVIC_EnableIRQ(EXTI0_IRQn);
    NVIC_EnableIRQ(EXTI1_IRQn);
    NVIC_EnableIRQ(EXTI2_IRQn);

    MDS_Err_t err = DEV_GPIO_ModuleInit(&g_moduleGPIO, GPIO_MODULE, &G_DRV_STM32F1XX_GPIO, NULL, NULL);
    if (err != MDS_EOK) {
        MDS_LOG_E("[BRD_GPIO] DEV_GPIO_ModuleInit fail, err:%d", err);
        return;
    }

    for (size_t idx = 0; idx < ARRAY_SIZE(g_pin); idx++) {
        g_pin[idx].object.GPIOx = G_GPIO_DESC[idx].object.GPIOx;
        g_pin[idx].object.pinMask = G_GPIO_DESC[idx].object.pinMask;
        g_pin[idx].object.initVal = G_GPIO_DESC[idx].object.initVal;

        err = DEV_GPIO_PinInit(&(g_pin[idx]), (const char *)G_GPIO_DESC[idx].object.parent, &g_moduleGPIO);
        if (err != MDS_EOK) {
            MDS_LOG_E("[BRD_GPIO] DEV_GPIO_PinInit fail, err:%d", err);
        } else {
            DEV_GPIO_PinConfig(&(g_pin[idx]), &(G_GPIO_DESC[idx].config));
        }
    }
}
MDS_INIT_IMPORT(MDS_INIT_PRIORITY_0, BOARD_GPIO_Init);
