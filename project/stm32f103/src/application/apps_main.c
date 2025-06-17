#include "board.h"
#include "boot/mds_boot.h"
#include "mds_log.h"

MDS_LOG_MODULE_DEFINE(board, CONFIG_BOARD_LOG_LEVEL);

void InitThread(MDS_Arg_t *arg)
{
    UNUSED(arg);

    MDS_InitExport();
}

int main(void)
{
    BOARD_CLOCK_Init();

    MDS_KernelInit();

    MDS_BOOT_SwapInfo_t *swapInfo = MDS_BOOT_GetSwapInfo();
    MDS_LOG_I("[boot] resetReaon:%lx", swapInfo->reset);

    MDS_Thread_t *thread = MDS_ThreadCreate("init", InitThread, NULL, 1024,
                                            MDS_THREAD_PRIORITY(10), MDS_TIMEOUT_TICKS(10));
    if (thread != NULL) {
        MDS_ThreadStartup(thread);
    }

    MDS_KernelStartup();
}
