#include "board.h"
#include "drv_pmu.h"
#include "drv_rcu.h"
#include "drv_rtc.h"

#define BOARD_LPC_SLEEP_THRESHOLD 300
#define BOARD_LPC_SLEEP_MIN       10

#define BOARD_LPC_RUN_HIGH_CLKFREQ 96000000

static DRV_RTC_Handle_t hrtc;

static MDS_LPC_Run_t BOARD_LPC_GetRunMode(void)
{
    MDS_LPC_Run_t run;
    uint32_t sysClockFreq = DRV_RCU_GetCoreClockFreq();

    if (sysClockFreq == BOARD_LPC_RUN_HIGH_CLKFREQ) {
        run = MDS_LPC_RUN_HIGH;
    } else {  // HSI_VALUE
        run = MDS_LPC_RUN_LOW;
    }

    return (run);
}

static MDS_Err_t BOARD_LPC_HighRun(void)
{
    DRV_RCU_OscInit_t oscInitStruct = {0};

    oscInitStruct.OscillatorType = RCU_OSCILLATORTYPE_IRC8M | RCU_OSCILLATORTYPE_IRC40K;

    oscInitStruct.IRC8MState = RCU_IRC8M_ON;
    oscInitStruct.IRC8MCalibValue = RCU_IRC8MCALIB_DEFAULT;

    oscInitStruct.IRC40KState = RCU_IRC40K_ON;

    oscInitStruct.PLL.PLLState = RCU_PLL_ON;
    oscInitStruct.PLL.PLLSource = RCU_PLLSRC_IRC8M_DIV2;
    oscInitStruct.PLL.PLLMUL = RCU_PLL_MUL24;  // 8 / 2 * 24 = 96M

    MDS_Err_t err = DRV_RCU_OscConfig(&oscInitStruct);
    if (err != MDS_EOK) {
        MDS_LOG_E("[BOARD_LPC_HighRun] osc config err:%d", err);
        return (err);
    }

    DRV_RCU_ClockInit_t clkInitStruct = {
        .ClockType = RCU_CLOCKTYPE_SYSCLK | RCU_CLOCKTYPE_HCLK | RCU_CLOCKTYPE_PCLK1 | RCU_CLOCKTYPE_PCLK2,
        .SYSCLKSource = RCU_CKSYSSRC_PLL,
        .AHBCLKDivider = RCU_AHB_CKSYS_DIV1,
        .APB1CLKDivider = RCU_APB1_CKAHB_DIV1,
        .APB2CLKDivider = RCU_APB2_CKAHB_DIV1,
    };

    err = DRV_RCU_ClockConfig(&clkInitStruct);
    if (err != MDS_EOK) {
        MDS_LOG_E("[BOARD_LPG_HighRun] clock config err:%d", err);
    }

    return (err);
}

static MDS_Err_t BOARD_LPC_LowRun(void)
{
    DRV_RCU_ClockInit_t clkInitStruct = {
        .ClockType = RCU_CLOCKTYPE_SYSCLK | RCU_CLOCKTYPE_HCLK | RCU_CLOCKTYPE_PCLK1 | RCU_CLOCKTYPE_PCLK2,
        .SYSCLKSource = RCU_CKSYSSRC_IRC8M,
        .AHBCLKDivider = RCU_AHB_CKSYS_DIV1,
        .APB1CLKDivider = RCU_APB1_CKAHB_DIV1,
        .APB2CLKDivider = RCU_APB2_CKAHB_DIV1,
    };

    MDS_Err_t err = DRV_RCU_ClockConfig(&clkInitStruct);
    if (err != MDS_EOK) {
        MDS_LOG_E("[BOARD_LPD_LowRun] clock config err:%d", err);
        return (err);
    }

    DRV_RCU_PLLInit_t pllConfig = {
        .PLLState = RCU_PLL_OFF,
    };

    err = DRV_RCU_PLLConfig(&pllConfig);
    if (err != MDS_EOK) {
        MDS_LOG_E("[BOARD_LPD_LowRun] pll config err:%d", err);
    }

    return (err);
}

static MDS_LPC_Run_t BOARD_LPC_Run(MDS_LPC_Run_t run)
{
    if (run == MDS_LPC_RUN_HIGH) {
        BOARD_LPC_HighRun();
    } else {
        BOARD_LPC_LowRun();
    }

    return (BOARD_LPC_GetRunMode());
}

static MDS_Tick_t BOARD_LPC_SleepDeep(MDS_Tick_t ticksleep)
{
    MDS_Tick_t sleeptick = 0;
    MDS_Err_t err = MDS_ERANGE;

    do {
        if (ticksleep < BOARD_LPC_SLEEP_MIN) {
            break;
        }

        err = DRV_RTC_TimerAlarmStartINT(&hrtc, 0, ticksleep);
        if (err != MDS_EOK) {
            break;
        }

        err = BOARD_LPC_LowRun();
        if (err == MDS_EOK) {
            SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
            DRV_PMU_EnterDeepSleepMode(PMU_LDO_LOWPOWER, WFI_CMD);
            SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
        }

        DRV_RTC_AlarmTimerStop(&hrtc);
    } while (0);

    if (err == MDS_EOK) {
        sleeptick = DRV_RTC_GetTimerCount(&hrtc);
    } else {
        DRV_PMU_EnterSleepMode(WFI_CMD);
    }

    return (sleeptick);
}

static MDS_Tick_t BOARD_LPC_Sleep(MDS_LPC_Sleep_t sleep, MDS_Tick_t ticksleep)
{
    MDS_Tick_t sleeptick = 0;

    switch (sleep) {
        case MDS_LPC_SLEEP_LIGHT:
        case MDS_LPC_SLEEP_DEEP:
            sleeptick = BOARD_LPC_SleepDeep(ticksleep);
            break;
        case MDS_LPC_SLEEP_RESET:
        case MDS_LPC_SLEEP_SHUTDOWN:
            NVIC_SystemReset();
            break;
        default:
            DRV_PMU_EnterSleepMode(WFI_CMD);
            break;
    }

    return (sleeptick);
}

static const MDS_LPC_ManagerOps_t G_LPC_OPS = {
    .sleep = BOARD_LPC_Sleep,
    .run = BOARD_LPC_Run,
};

void BOARD_CLOCK_Init(void)
{
    BOARD_LPC_HighRun();

    DRV_RCU_RTCClockSelection(RCU_RTCSRC_IRC40K);
    DRV_RCU_RTCClockEnable();

    static const DRV_RTC_InitStruct_t init = {
        .prescale = (IRC40K_VALUE / MDS_CLOCK_TICK_FREQ_HZ) - 1,
    };

    DRV_RTC_Init(&hrtc, &init);

    // run default will be low in idle thread
    MDS_LPC_Init(&G_LPC_OPS, BOARD_LPC_SLEEP_THRESHOLD, MDS_LPC_SLEEP_IDLE, MDS_LPC_RUN_LOW);
}
