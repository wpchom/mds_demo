#include "board.h"

#define BOARD_LPC_SLEEP_THRESHOLD 300
#define BOARD_LPC_SLEEP_MIN       10

static MDS_Tick_t BPARD_LPC_SleepDeep(MDS_Tick_t ticksleep)
{
    if (ticksleep < BOARD_LPC_SLEEP_MIN) {
        HAL_PWR_EnterSLEEPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFI);
        return (0);
    }

    MDS_Tick_t sleeptick = 0;

    // stm32f103 rtc not support wakeup timer
    // or use tim1/2 with etr

    return (sleeptick);
}

static MDS_Tick_t BOARD_LPC_Sleep(MDS_LPC_Sleep_t sleep, MDS_Tick_t ticksleep)
{
    MDS_Tick_t sleeptick = 0;

    switch (sleep) {
        case MDS_LPC_SLEEP_LIGHT:
        case MDS_LPC_SLEEP_DEEP:
            sleeptick = BPARD_LPC_SleepDeep(ticksleep);
            break;
        case MDS_LPC_SLEEP_RESET:
        case MDS_LPC_SLEEP_SHUTDOWN:
            HAL_NVIC_SystemReset();
            break;
        default:
            HAL_PWR_EnterSLEEPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFI);
            break;
    }

    return (sleeptick);
}

static MDS_LPC_Run_t BOARD_LPC_GetRunMode(void)
{
    MDS_LPC_Run_t run;
    uint32_t sysClockFreq = HAL_RCC_GetSysClockFreq();

    if (sysClockFreq == 64000000) {
        run = MDS_LPC_RUN_HIGH;
    } else if (sysClockFreq == 32000000) {
        run = MDS_LPC_RUN_NORMAL;
    } else {  // HSI_VALUE
        run = MDS_LPC_RUN_LOW;
    }

    return (run);
}

static MDS_LPC_Run_t BOARD_LPC_Run(MDS_LPC_Run_t run)
{
    uint32_t FLatency;
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.LSIState = RCC_LSI_ON;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;

    HAL_RCC_DisableCSS();
    if (run == MDS_LPC_RUN_HIGH) {  // 64MHz
        RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
        RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
        RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
        RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV4;
        RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
        RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;
        FLatency = FLASH_LATENCY_2;
    } else if (run == MDS_LPC_RUN_NORMAL) {  // 32MHz
        RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
        RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
        RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL8;
        RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
        RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
        RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
        FLatency = FLASH_LATENCY_1;
    } else {  // HSI_VALUE = 8MHz
        RCC_OscInitStruct.PLL.PLLState = RCC_PLL_OFF;
        RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
        RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
        RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
        RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
        FLatency = FLASH_LATENCY_0;
    }

    do {
        if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
            MDS_LOG_E("[BOARD_LPC_Run] HAL_RCC_OscConfig failed run:%d", run);
            break;
        }
        if (RCC_OscInitStruct.PLL.PLLState == RCC_PLL_ON) {
            HAL_RCC_EnableCSS();
        }
        if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLatency) != HAL_OK) {
            MDS_LOG_E("[BOARD_LPC_Run] HAL_RCC_ClockConfig failed run:%d", run);
            break;
        }
    } while (0);

    return (BOARD_LPC_GetRunMode());
}

static const MDS_LPC_ManagerOps_t G_LPC_OPS = {
    .sleep = BOARD_LPC_Sleep,
    .run = BOARD_LPC_Run,
};

void BOARD_CLOCK_Init(void)
{
    HAL_Init();

    SysTick_Config(SystemCoreClock / MDS_CLOCK_TICK_FREQ_HZ);

    MDS_LPC_Init(&G_LPC_OPS, BOARD_LPC_SLEEP_THRESHOLD, MDS_LPC_SLEEP_IDLE, MDS_LPC_RUN_LOW);

    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_ADC;
    PeriphClkInitStruct.AdcClockSelection = RCC_ADCPCLK2_DIV2;

    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
        MDS_LOG_E("[BOARD_CLOCK_Init] HAL_RCCEx_PeriphCLKConfig failed");
        return;
    }
}

void SysTick_Handler(void)
{
    MDS_ClockIncTickCount();
}
