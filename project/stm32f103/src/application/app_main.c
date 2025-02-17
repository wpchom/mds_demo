#include "board.h"
#include "mds_boot.h"

void InitThread(MDS_Arg_t *arg)
{
    UNUSED(arg);

    // request

    MDS_InitExport();

    // release
}

int main(void)
{
    BOARD_CLOCK_Init();

    MDS_KernelInit();

    MDS_BOOT_SwapInfo_t *swapInfo = MDS_BOOT_GetSwapInfo();
    MDS_LOG_I("[boot] resetReaon:%x", swapInfo->reset);

    MDS_Thread_t *thread = MDS_ThreadCreate("init", InitThread, NULL, 1024, 10, 10);
    if (thread != NULL) {
        MDS_ThreadStartup(thread);
    }

    MDS_KernelStartup();
}
