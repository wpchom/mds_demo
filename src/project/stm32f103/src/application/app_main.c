#include "mds_sys.h"
#include "stm32f1xx.h"
#include "SEGGER_SYSVIEW.h"

void InitThread(MDS_Arg_t *arg)
{
    UNUSED(arg);

    MDS_LOOP {
        MDS_ThreadDelay(1000);
    }
}

void BOARD_CLOCK_Init(void)
{
}

int main(void)
{
    BOARD_CLOCK_Init();

    SysTick_Config(SystemCoreClock / MDS_SYSTICK_FREQ_HZ);

    MDS_KernelInit();

    SEGGER_SYSVIEW_Conf();

    MDS_Thread_t *thread = MDS_ThreadCreate("init", InitThread, NULL, 1024, 10, 10);
    if (thread != NULL) {
        MDS_ThreadStartup(thread);
    }

    MDS_KernelStartup();
}

void SysTick_Handler(void)
{
    MDS_SysTickIncCount();
}