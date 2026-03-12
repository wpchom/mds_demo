#include "mds_sys.h"
#include "stm32h7xx.h"
// #include "SEGGER_SYSVIEW.h"

void InitThread(MDS_Arg_t arg)
{
    UNUSED(arg);

    for(;;) {
        MDS_ThreadDelay(MDS_TIMEOUT_MS(1000));
    }
}

int main(void)
{
    MDS_KernelInit();

    // SEGGER_SYSVIEW_Conf();

    // MDS_CoreInterruptRequestRegister(SysTick_IRQn, (MDS_IsrHandler_t)MDS_SysTickIncCount, NULL);
    // MDS_CoreInterruptRequestEnable(SysTick_IRQn);
    SysTick_Config(SystemCoreClock / CONFIG_MDS_CLOCK_TICK_FREQ_HZ);

    MDS_Thread_t *thread = MDS_ThreadCreate("init", InitThread, MDS_ARG_WITH(NULL), 1024,
                                            MDS_THREAD_PRIORITY(10), MDS_TIMEOUT_MS(10));
    if (thread != NULL) {
        MDS_ThreadStartup(thread);
    }

    MDS_KernelStartup();
}
