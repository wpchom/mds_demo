#include "mds_sys.h"
#include "stm32h7xx.h"
#include "SEGGER_SYSVIEW.h"

void InitThread(MDS_Arg_t *arg)
{
    UNUSED(arg);

    MDS_LOOP {
        MDS_ThreadDelay(1000);
    }
}

int main(void)
{
    MDS_KernelInit();

    SEGGER_SYSVIEW_Conf();

    // MDS_CoreInterruptRequestRegister(SysTick_IRQn, (MDS_IsrHandler_t)MDS_SysTickIncCount, NULL);
    // MDS_CoreInterruptRequestEnable(SysTick_IRQn);
    SysTick_Config(SystemCoreClock / MDS_SYSTICK_FREQ_HZ);

    MDS_Thread_t *thread = MDS_ThreadCreate("init", InitThread, NULL, 1024, 10, 10);
    if (thread != NULL) {
        MDS_ThreadStartup(thread);
    }

    MDS_KernelStartup();
}
