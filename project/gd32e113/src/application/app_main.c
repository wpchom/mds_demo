#include "board.h"

void InitThreadEntry(MDS_Arg_t *arg)
{
    UNUSED(arg);

    MDS_InitExport();
}

int main(void)
{
    BOARD_CLOCK_Init();

    MDS_KernelInit();

    MDS_Thread_t *thread = MDS_ThreadCreate("init", InitThreadEntry, NULL, 1024, 10, 10);
    if (thread != NULL) {
        MDS_ThreadStartup(thread);
    }

    MDS_KernelStartup();
}

void SysTick_Handler(void)
{
    MDS_ClockIncTickCount();
}
