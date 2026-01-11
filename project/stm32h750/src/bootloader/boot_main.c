#include "mds_sys.h"
#include "stm32h7xx_hal.h"
#include "drv_gpio.h"

MDS_LOG_MODULE_DEFINE(boot);

void InitThread(MDS_Arg_t *arg)
{
    UNUSED(arg);

    __HAL_RCC_GPIOE_CLK_ENABLE();

    DEV_GPIO_Pin_t *pin = NULL;
    DEV_GPIO_Module_t *gpio = DEV_GPIO_ModuleCreate("gpio", &G_DRV_STM32H7XX_GPIO, NULL);
    if (gpio != NULL) {
        pin = DEV_GPIO_PinCreate("led", gpio);
    }
    if (pin != NULL) {
        pin->object.GPIOx = GPIOE;
        pin->object.pinMask = 1 << 3;

        DEV_GPIO_Config_t config = {
            .mode = DEV_GPIO_MODE_OUTPUT,
            .type = DEV_GPIO_TYPE_PP_UP,
            .intr = DEV_GPIO_INTR_NONE,
        };
        DEV_GPIO_PinConfig(pin, &config);
    }

    for (;;) {
        if (pin != NULL) {
            DEV_GPIO_PinToggle(pin);
        }
        MDS_ThreadDelay(MDS_TIMEOUT_MS(500));
    }
}

int main(void)
{
    MDS_KernelInit();

    HAL_Init();
    SysTick_Config(SystemCoreClock / CONFIG_MDS_CLOCK_TICK_FREQ_HZ);

    MDS_LOG_D("startup");

    MDS_Thread_t *thread = MDS_ThreadCreate("init", InitThread, NULL, 1024, MDS_THREAD_PRIORITY(10),
                                            MDS_TIMEOUT_MS(10));
    if (thread != NULL) {
        MDS_ThreadStartup(thread);
    }

    MDS_KernelStartup();
}

void SysTick_Handler(void)
{
    MDS_SysTickHandler();
}
