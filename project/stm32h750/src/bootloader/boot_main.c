#include "mds_sys.h"
#include "mds_log.h"
#include "stm32h7xx.h"

MDS_LOG_MODULE_DEFINE(boot);

void InitThread(MDS_Arg_t *arg)
{
    UNUSED(arg);

    MDS_LOOP {
        MDS_ThreadDelay(MDS_TIMEOUT_MS(1000));
    }
}

int main(void)
{
    MDS_KernelInit();

    SysTick_Config(SystemCoreClock / CONFIG_MDS_CLOCK_TICK_FREQ_HZ);

    MDS_LOG_D("startup");

    MDS_Thread_t *thread = MDS_ThreadCreate("init", InitThread, NULL, 1024,
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
