#include "board.h"

MDS_LOG_MODULE_DEFINE(board, CONFIG_BOARD_LOG_LEVEL);

#define BOARD_LPC_SLEEP_THRESHOLD MDS_TIMEOUT_MS(300)
#define BOARD_LPC_SLEEP_MIN       MDS_TIMEOUT_MS(10)
#define BOARD_LPC_SLEEP_MAX       MDS_TIMEOUT_MS(20000)

static RTC_HandleTypeDef hRTC_Handle = {0};
static LPTIM_HandleTypeDef hlptim5 = {0};

static volatile uint32_t g_cnt = 0;

static MDS_LPC_Run_t BOARD_LPC_GetRunMode(void)
{
    MDS_LPC_Run_t run;
    uint32_t sysClockFreq = HAL_RCC_GetSysClockFreq();

    if (sysClockFreq == 480000000UL) {
        run = MDS_LPC_RUN_HIGH;
    } else if (sysClockFreq == 240000000UL) {
        run = MDS_LPC_RUN_NORMAL;
    } else { // HSI_VALUE 64MHz
        run = MDS_LPC_RUN_LOW;
    }

    return (run);
}

static MDS_LPC_Run_t BOARD_LPC_Run(MDS_LPC_Run_t run)
{
    uint32_t FLatency;
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_HSE |
        RCC_OSCILLATORTYPE_LSE | RCC_OSCILLATORTYPE_LSI;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.LSEState = RCC_LSE_OFF;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.LSIState = RCC_LSI_ON;
    RCC_OscInitStruct.HSI48State = RCC_HSI48_OFF;
    RCC_OscInitStruct.CSIState = RCC_CSI_OFF;
    RCC_OscInitStruct.CSICalibrationValue = RCC_CSICALIBRATION_DEFAULT;

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 |
        RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_D1PCLK1 | RCC_CLOCKTYPE_D3PCLK1;

    HAL_RCC_DisableCSS();
    if (run == MDS_LPC_RUN_HIGH) { // 480MHz
        RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
        RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
        RCC_OscInitStruct.PLL.PLLM = 5;   // 5MHz
        RCC_OscInitStruct.PLL.PLLN = 160; // 5*192=960
        RCC_OscInitStruct.PLL.PLLP = 2;   // SYSCLK 480MHz
        RCC_OscInitStruct.PLL.PLLQ = 5;   // 192MHz
        RCC_OscInitStruct.PLL.PLLR = 2;   // 480HMz
        RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
        RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
        RCC_OscInitStruct.PLL.PLLFRACN = 0;

        RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
        RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1; // D1CPRE
        RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;   // HPRE
        RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;  // D2PPRE1
        RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;  // D2PPRE2
        RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;  // D1PPRE
        RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;  // D3PPRE

        FLatency = FLASH_LATENCY_5;
    } else if (run == MDS_LPC_RUN_NORMAL) { // 240MHz
        RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
        RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
        RCC_OscInitStruct.PLL.PLLM = 5;   // 5MHz
        RCC_OscInitStruct.PLL.PLLN = 192; // 5*192=960
        RCC_OscInitStruct.PLL.PLLP = 4;   // SYSCLK 240MHz
        RCC_OscInitStruct.PLL.PLLQ = 5;   // USBCLK 192MHz
        RCC_OscInitStruct.PLL.PLLR = 4;   // 240MHz
        RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
        RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
        RCC_OscInitStruct.PLL.PLLFRACN = 0;

        RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
        RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1; // D1CPRE
        RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;   // HPRE
        RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;  // D2PPRE1
        RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV1;  // D2PPRE2
        RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV1;  // D1PPRE
        RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;  // D3PPRE

        FLatency = FLASH_LATENCY_3;
    } else { // HSI_VALUE = 64MHz
        RCC_OscInitStruct.PLL.PLLState = RCC_PLL_OFF;

        RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
        RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1; // D1CPRE
        RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;   // HPRE
        RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;  // D2PPRE1
        RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV1;  // D2PPRE2
        RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV1;  // D1PPRE
        RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;  // D3PPRE

        FLatency = FLASH_LATENCY_1;
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

void RTC_WKUP_IRQHandler(void)
{
    HAL_RTCEx_WakeUpTimerIRQHandler(&hRTC_Handle);
}

void LPTIM5_IRQHandler(void)
{
    g_cnt++;
    HAL_LPTIM_IRQHandler(&hlptim5);

    MDS_Tick_t sleepcnt = 1000 * (LSI_VALUE / 16) / CONFIG_MDS_CLOCK_TICK_FREQ_HZ;
    HAL_LPTIM_TimeOut_Start_IT(&hlptim5, 0, sleepcnt);
}

static void BOARD_LPTIM5_Init(void)
{
    // __HAL_RCC_RTC_ENABLE();

    // hRTC_Handle.Instance = RTC;
    // hRTC_Handle.Init.HourFormat = RTC_HOURFORMAT_24;
    // hRTC_Handle.Init.AsynchPrediv = 31;
    // hRTC_Handle.Init.SynchPrediv = 999;
    // hRTC_Handle.Init.OutPut = RTC_OUTPUT_DISABLE;
    // hRTC_Handle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    // hRTC_Handle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;

    // if (HAL_RTC_Init(&hRTC_Handle) != HAL_OK) {
    //     MDS_LOG_E("[BOARD_CLOCK_Init] HAL_RTC_Init failed");
    //     return;
    // }

    // HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);

    __HAL_RCC_LPTIM5_CLK_ENABLE();

    hlptim5.Instance = LPTIM5;
    hlptim5.Init.Clock.Source = LPTIM_CLOCKSOURCE_APBCLOCK_LPOSC;
    hlptim5.Init.Clock.Prescaler = LPTIM_PRESCALER_DIV16;
    hlptim5.Init.Trigger.Source = LPTIM_TRIGSOURCE_SOFTWARE;
    hlptim5.Init.Trigger.ActiveEdge = LPTIM_ACTIVEEDGE_RISING;
    hlptim5.Init.Trigger.SampleTime = LPTIM_TRIGSAMPLETIME_DIRECTTRANSITION;
    hlptim5.Init.OutputPolarity = LPTIM_OUTPUTPOLARITY_HIGH;
    hlptim5.Init.UpdateMode = LPTIM_UPDATE_IMMEDIATE;
    hlptim5.Init.CounterSource = LPTIM_COUNTERSOURCE_INTERNAL;

    if (HAL_LPTIM_Init(&hlptim5) != HAL_OK) {
        MDS_LOG_E("[BOARD_LPTIM5_Init] init failed");
        return;
    }

    HAL_NVIC_EnableIRQ(LPTIM5_IRQn);

    HAL_LPTIM_TimeOut_Stop_IT(&hlptim5);

    MDS_Tick_t sleepcnt = 1000 * (LSI_VALUE / 16) / CONFIG_MDS_CLOCK_TICK_FREQ_HZ;
    HAL_LPTIM_TimeOut_Start_IT(&hlptim5, 0, sleepcnt);
}

static MDS_Tick_t BPARD_LPC_SleepDeep(MDS_Tick_t ticksleep)
{
    if (ticksleep < BOARD_LPC_SLEEP_MIN.ticks) {
        HAL_PWR_EnterSLEEPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFI);
        return (0);
    }

    // RTC_TimeTypeDef sTime;

    // HAL_RTC_GetTime(&hRTC_Handle, &sTime, RTC_FORMAT_BIN);
    // uint32_t ts = MDS_TIME_MSEC_OF_SEC * sTime.Seconds +
    //     MDS_TIME_MSEC_OF_SEC * (sTime.SecondFraction - sTime.SubSeconds) /
    //         (sTime.SecondFraction + 1);

    MDS_Tick_t sleepcnt = ticksleep * (LSI_VALUE / 16) / CONFIG_MDS_CLOCK_TICK_FREQ_HZ;
    HAL_LPTIM_TimeOut_Start_IT(&hlptim5, 0, sleepcnt);
    // HAL_RTCEx_SetWakeUpTimer_IT(&hRTC_Handle, sleepcnt, RTC_WAKEUPCLOCK_RTCCLK_DIV16);

    HAL_NVIC_DisableIRQ(SysTick_IRQn);
    // SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
    // SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    HAL_NVIC_EnableIRQ(SysTick_IRQn);

    // HAL_RTCEx_DeactivateWakeUpTimer(&hRTC_Handle);

    // HAL_RTC_GetTime(&hRTC_Handle, &sTime, RTC_FORMAT_BIN);
    // uint32_t te = MDS_TIME_MSEC_OF_SEC * sTime.Seconds +
    //     MDS_TIME_MSEC_OF_SEC * (sTime.SecondFraction - sTime.SubSeconds) /
    //         (sTime.SecondFraction + 1);

    // return ((te >= ts) ? (te - ts) : (MDS_TIME_MSEC_OF_MIN + te - ts));

    sleepcnt = HAL_LPTIM_ReadCounter(&hlptim5);
    HAL_LPTIM_TimeOut_Stop_IT(&hlptim5);

    return (sleepcnt * CONFIG_MDS_CLOCK_TICK_FREQ_HZ / (LSE_VALUE / 16));
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

static const MDS_LPC_ManagerOps_t G_LPC_OPS = {
    .sleep = BOARD_LPC_Sleep,
    .run = BOARD_LPC_Run,
};

static void BOARD_PeriphClockInit(void)
{
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    PeriphClkInitStruct.PLL2.PLL2M = 25;
    PeriphClkInitStruct.PLL2.PLL2N = 480;
    PeriphClkInitStruct.PLL2.PLL2P = 4; // 120MHz
    PeriphClkInitStruct.PLL2.PLL2Q = 4; // 120MHz
    PeriphClkInitStruct.PLL2.PLL2R = 2; // 240MHz
    PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_0;
    PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;
    PeriphClkInitStruct.PLL2.PLL2FRACN = 0;

    PeriphClkInitStruct.PLL3.PLL3M = 25;
    PeriphClkInitStruct.PLL3.PLL3N = 288;
    PeriphClkInitStruct.PLL3.PLL3P = 2; // sai 144MHz
    PeriphClkInitStruct.PLL3.PLL3Q = 6; // usb 48MHz
    PeriphClkInitStruct.PLL3.PLL3R = 2; // ltdc 144MHz
    PeriphClkInitStruct.PLL3.PLL3RGE = RCC_PLL3VCIRANGE_0;
    PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOWIDE;
    PeriphClkInitStruct.PLL3.PLL3FRACN = 0;

    PeriphClkInitStruct.PeriphClockSelection =
        RCC_PERIPHCLK_RTC | RCC_PERIPHCLK_LPTIM5 | RCC_PERIPHCLK_QSPI;
    PeriphClkInitStruct.CkperClockSelection = RCC_CLKPSOURCE_HSI; // 64MHz

    // D1 (LTDC ppl3r)
    PeriphClkInitStruct.FmcClockSelection = RCC_FMCCLKSOURCE_PLL2;     // hclk3 / pll2r
    PeriphClkInitStruct.QspiClockSelection = RCC_QSPICLKSOURCE_PLL2;   // hclk3 / pll2r
    PeriphClkInitStruct.SdmmcClockSelection = RCC_SDMMCCLKSOURCE_PLL2; // pll2r

    // D2 (SDMMC2 - D2) (SDMMC1 - D1)
    PeriphClkInitStruct.Dfsdm1ClockSelection = RCC_DFSDM1CLKSOURCE_D2PCLK1; // pclk2
    PeriphClkInitStruct.FdcanClockSelection = RCC_FDCANCLKSOURCE_PLL2;      // pll2q / pll1q
    PeriphClkInitStruct.CecClockSelection = RCC_CECCLKSOURCE_LSE;           // lse / lsi
    PeriphClkInitStruct.I2c1235ClockSelection = RCC_I2C1235CLKSOURCE_PLL3;  // pclk1 / pll3r
    PeriphClkInitStruct.Lptim1ClockSelection = RCC_LPTIM1CLKSOURCE_LSI;     // pclk1 / pll2p
    PeriphClkInitStruct.Hrtim1ClockSelection = RCC_HRTIM1CLK_CPUCLK;        // cpuclk
    PeriphClkInitStruct.RngClockSelection = RCC_RNGCLKSOURCE_PLL;           // pll1q / hsi48
    PeriphClkInitStruct.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLL3;        // pll3p
    PeriphClkInitStruct.Sai23ClockSelection = RCC_SAI23CLKSOURCE_PLL3;      // pll3p
    PeriphClkInitStruct.SpdifrxClockSelection = RCC_SPDIFRXCLKSOURCE_PLL;   // pll1q / pll2r / pll3r
    PeriphClkInitStruct.Spi123ClockSelection = RCC_SPI123CLKSOURCE_PLL;     // pll1q / pll3p (I2S)
    PeriphClkInitStruct.Spi45ClockSelection = RCC_SPI45CLKSOURCE_PLL2;      // pclk2 / pll2q
    PeriphClkInitStruct.Swpmi1ClockSelection = RCC_SWPMI1CLKSOURCE_D2PCLK1; // pclk1 / hsi
    PeriphClkInitStruct.Usart16ClockSelection = RCC_USART16CLKSOURCE_PLL2;  // pclk2 / pll2q
    PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_PCLK1; // pclk1 / pll2q
    PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_PLL3;                  // pll3q / hsi48

    // D3 (ADC1,2 - D2) (ADC3 - D3)
    PeriphClkInitStruct.AdcClockSelection = RCC_ADCCLKSOURCE_CLKP;          // ck_per
    PeriphClkInitStruct.I2c4ClockSelection = RCC_I2C4CLKSOURCE_D3PCLK1;     // pclk1
    PeriphClkInitStruct.Lptim2ClockSelection = RCC_LPTIM2CLKSOURCE_LSI;     // pclk4 / pll2p
    PeriphClkInitStruct.Lptim345ClockSelection = RCC_LPTIM345CLKSOURCE_LSI; // pclk4 / pll2p
    PeriphClkInitStruct.Lpuart1ClockSelection = RCC_LPUART1CLKSOURCE_PLL2;  // pclk4 / pll2q / hsi
    PeriphClkInitStruct.Sai4AClockSelection = RCC_SAI4ACLKSOURCE_PLL3;      // pll3p
    PeriphClkInitStruct.Sai4BClockSelection = RCC_SAI4BCLKSOURCE_PLL3;      // pll3p
    PeriphClkInitStruct.Spi6ClockSelection = RCC_SPI6CLKSOURCE_PLL2;        // pclk1 / pll2q

    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI; // lse / lsi
    // PeriphClkInitStruct.TIMPresSelection;

    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
        MDS_LOG_E("[BOARD_PeriphClockInit] HAL_RCCEx_PeriphCLKConfig failed");
        return;
    }
}

void BOARD_CLOCK_Init(void)
{
    HAL_Init();

    SysTick_Config(SystemCoreClock / CONFIG_MDS_CLOCK_TICK_FREQ_HZ);

    MDS_LPC_Init(&G_LPC_OPS, BOARD_LPC_SLEEP_THRESHOLD.ticks, MDS_LPC_SLEEP_IDLE, MDS_LPC_RUN_LOW);

    BOARD_PeriphClockInit();

    __HAL_RCC_WAKEUPSTOP_CLK_CONFIG(RCC_STOP_WAKEUPCLOCK_HSI);

    BOARD_LPTIM5_Init();
}

void SysTick_Handler(void)
{
    HAL_IncTick();
    MDS_SysTickHandler();
}
