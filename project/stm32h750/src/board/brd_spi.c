#include "board.h"
#include "drv_spi.h"
#include "drv_qspi.h"
#include "drv_gpio.h"

MDS_LOG_MODULE_DECLARE(board, CONFIG_BOARD_LOG_LEVEL);

static DEV_XSPI_Adaptr_t g_adaptrXSPI;
static DRV_QSPI_Handle_t g_handleQSPI;
static DEV_SPI_Adaptr_t g_adaptrSPI1;
static DRV_SPI_Handle_t g_handleSPI1;

static void BOARD_QSPI_Resume(MDS_Arg_t dev, MDS_LPC_Run_t run)
{
    UNUSED(dev);
    UNUSED(run);

    // QSPI_BK1_IO3->PD13
    // QSPI_BK1_IO2->PE2
    // QSPI_BK1_IO1->PD12
    // QSPI_BK1_IO0->PD11
    // QSPI_CLK ->PB2
    // QSPI_BK1_NCS ->PB6

    DEV_GPIO_Config_t pinConfig = {
        .mode = DEV_GPIO_MODE_ALTERNATE,
        .type = DEV_GPIO_TYPE_PP_NO,
        .intr = DEV_GPIO_INTR_NONE,
        .alternate = GPIO_AF9_QUADSPI,
    };

    DRV_GPIO_PinConfig(GPIOD, GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13, &pinConfig);
    DRV_GPIO_PinConfig(GPIOE, GPIO_PIN_2, &pinConfig);
    DRV_GPIO_PinConfig(GPIOB, GPIO_PIN_2, &pinConfig);

    pinConfig.alternate = GPIO_AF10_QUADSPI;
    DRV_GPIO_PinConfig(GPIOB, GPIO_PIN_6, &pinConfig);

    __HAL_RCC_QSPI_CLK_ENABLE();
}

static void BOARD_QSPI_Suspend(MDS_Arg_t dev, MDS_LPC_Sleep_t sleep)
{
    UNUSED(dev);
    UNUSED(sleep);

    __HAL_RCC_QSPI_CLK_DISABLE();
}

static void BOARD_QSPI_Init(void)
{
    static MDS_LPC_Device_t lpcDevQSPI;
    static const MDS_LPC_DeviceOps_t lpcOpsQSPI = {
        .resume = BOARD_QSPI_Resume,
        .suspend = BOARD_QSPI_Suspend,
    };

    MDS_LPC_DeviceRegister(&lpcDevQSPI, MDS_ARG_WITH(&g_adaptrXSPI), &lpcOpsQSPI);

    MDS_Err_t err = DEV_XSPI_AdaptrInit(&g_adaptrXSPI, QSPI_MODULE, &G_DRV_STM32H7XX_QSPI,
                                        MDS_DEVICE_HANDLE(&g_handleQSPI), MDS_ARG_WITH(QUADSPI));
    if (!MDS_ErrIsSame(err, MDS_EOK)) {
        MDS_LOG_E("[BRD_SPI] DEV_XSPI_AdaptrInit fail, err:%d", err.errno);
        return;
    }

    DEV_XSPI_Periph_t *periph = DEV_XSPI_PeriphCreate(QSPI_FLASH_DEVICE, &g_adaptrXSPI);
    if (periph == NULL) {
        MDS_LOG_E("[BRD_I2C] DEV_XSPI_PeriphCreate fail");
    }
}
MDS_INIT_IMPORT(MDS_INIT_PRIORITY_1, BOARD_QSPI_Init);

static void BOARD_SPI1_Resume(MDS_Arg_t dev, MDS_LPC_Run_t run)
{
    UNUSED(dev);
    UNUSED(run);

    // SPI_Flash_MISO->PB4
    // SPI_Flash_MOSI->PD7
    // SPI_Flash_CLK->PB3
    // SPI_Flash_CS->PD6

    DEV_GPIO_Config_t pinConfig = {
        .mode = DEV_GPIO_MODE_OUTPUT,
        .type = DEV_GPIO_TYPE_PP_NO,
        .intr = DEV_GPIO_INTR_NONE,
    };

    DRV_GPIO_PinConfig(GPIOD, GPIO_PIN_6, &pinConfig);

    pinConfig.mode = DEV_GPIO_MODE_ALTERNATE;
    pinConfig.alternate = GPIO_AF5_SPI1,
    DRV_GPIO_PinConfig(GPIOB, GPIO_PIN_3 | GPIO_PIN_4, &pinConfig);
    DRV_GPIO_PinConfig(GPIOE, GPIO_PIN_7, &pinConfig);

    __HAL_RCC_SPI1_CLK_ENABLE();
}

static void BOARD_SPI1_Suspend(MDS_Arg_t dev, MDS_LPC_Sleep_t sleep)
{
    UNUSED(dev);
    UNUSED(sleep);

    __HAL_RCC_SPI1_CLK_DISABLE();
}

static void BOARD_SPI1_Init(void)
{
    static MDS_LPC_Device_t lpcDevSPI1;
    static const MDS_LPC_DeviceOps_t lpcOpsSPI1 = {
        .resume = BOARD_SPI1_Resume,
        .suspend = BOARD_SPI1_Suspend,
    };

    MDS_LPC_DeviceRegister(&lpcDevSPI1, MDS_ARG_WITH(&g_adaptrSPI1), &lpcOpsSPI1);

    MDS_Err_t err = DEV_SPI_AdaptrInit(&g_adaptrSPI1, SPI1_MODULE, &G_DRV_STM32H7XX_SPI,
                                       MDS_DEVICE_HANDLE(&g_handleSPI1), MDS_ARG_WITH(SPI1));
    if (!MDS_ErrIsSame(err, MDS_EOK)) {
        MDS_LOG_E("[BRD_SPI] DEV_SPI_AdaptrInit fail, err:%d", err.errno);
        return;
    }

    DEV_SPI_Periph_t *periph = DEV_SPI_PeriphCreate(SPI1_FLASH_DEVICE, &g_adaptrSPI1);
    if (periph == NULL) {
        MDS_LOG_E("[BRD_I2C] DEV_SPI_PeriphCreate fail");
    }
}
MDS_INIT_IMPORT(MDS_INIT_PRIORITY_2, BOARD_SPI1_Init);
