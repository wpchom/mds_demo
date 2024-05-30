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

    MDS_CoreInterruptRequestRegister(SysTick_IRQn, (MDS_IsrHandler_t)MDS_SysTickIncCount, NULL);
    MDS_CoreInterruptRequestEnable(SysTick_IRQn);
    SysTick_Config(SystemCoreClock / MDS_SYSTICK_FREQ_HZ);

    SEGGER_SYSVIEW_Conf();

    MDS_KernelInit();

    MDS_Thread_t *thread = MDS_ThreadCreate("init", InitThread, NULL, 1024, 10, 10);
    if (thread != NULL) {
        MDS_ThreadStartup(thread);
    }

    MDS_KernelStartup();
}
