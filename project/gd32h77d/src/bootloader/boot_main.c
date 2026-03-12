#include "mds_sys.h"

MDS_LOG_MODULE_DEFINE(boot);

void InitThread(MDS_Arg_t arg)
{
    UNUSED(arg);

    for (;;) {
    }
}

int main(void)
{
    MDS_KernelInit();

    // SysTick_Config(SystemCoreClock / CONFIG_MDS_CLOCK_TICK_FREQ_HZ);

    MDS_LOG_D("startup");

    MDS_Thread_t *thread = MDS_ThreadCreate("init", InitThread, MDS_ARG_WITH(NULL), 1024,
                                            MDS_THREAD_PRIORITY(10), MDS_TIMEOUT_MS(10));
    if (thread != NULL) {
        MDS_ThreadStartup(thread);
    }

    MDS_KernelStartup();
}

void SysTick_Handler(void)
{
    MDS_SysTickHandler();
}
