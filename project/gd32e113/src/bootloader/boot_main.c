#include "mds_boot.h"
#include "drv_chip.h"
#include "drv_flash.h"

#define BOOT_FIRMWARE_VERSION 0x00000001U

typedef struct BOOT_FlashDevice {
    uint32_t baseAddr;
    uint32_t pageNums;
} BOOT_FlashDevice_t;

static BOOT_FlashDevice_t g_flashApp = {
    .baseAddr = 0x08004000,
    .pageNums = 68 * 1024 / DRV_FLASH_PAGE_SIZE,
};

static BOOT_FlashDevice_t g_flashDft = {
    .baseAddr = 0x08015000,
    .pageNums = 44 * 1024 / DRV_FLASH_PAGE_SIZE,
};

uint32_t BOOT_GetResetReason(void)
{
    uint32_t resetReason = RCU_RSTSCK;

    RCU_RSTSCK |= RCU_RSTSCK_RSTFC;

    return (resetReason);
}

static int BOOT_FlashRead(MDS_BOOT_Device_t *dev, uintptr_t ofs, uint8_t *data, size_t len)
{
    BOOT_FlashDevice_t *flashDev = (BOOT_FlashDevice_t *)dev;

    MDS_MemBuffCopy(data, len, (void *)(flashDev->baseAddr + ofs), len);

    return (0);
}

static int BOOT_FlashWrite(MDS_BOOT_Device_t *dev, uintptr_t ofs, const uint8_t *data, size_t len)
{
    BOOT_FlashDevice_t *flashDev = (BOOT_FlashDevice_t *)dev;

    MDS_Err_t err = DRV_FLASH_Program(flashDev->baseAddr + ofs, data, len, NULL);

    return (err);
}

static int BOOT_FlashErase(MDS_BOOT_Device_t *dev)
{
    BOOT_FlashDevice_t *flashDev = (BOOT_FlashDevice_t *)dev;

    MDS_Err_t err = DRV_FLASH_Erase(flashDev->baseAddr, flashDev->pageNums, NULL);

    return (err);
}

const MDS_BOOT_UpgradeOps_t G_BOOT_UPGRADE_OPS = {
    .read = BOOT_FlashRead,
    .write = BOOT_FlashWrite,
    .erase = BOOT_FlashErase,
};

int main(void)
{
    extern void JUMP_APP_ADDRESS(void);

    SysTick_Config(SystemCoreClock / MDS_CLOCK_TICK_FREQ_HZ);
    NVIC_SetPriority(SysTick_IRQn, 0x00U);

    MDS_BOOT_SwapInfo_t *swapInfo = MDS_BOOT_GetSwapInfo();
    if (swapInfo != NULL) {
        swapInfo->version = BOOT_FIRMWARE_VERSION;
        swapInfo->reset = BOOT_GetResetReason();
    }

    MDS_BOOT_Result_t result = MDS_BOOT_UpgradeCheck(swapInfo, (MDS_BOOT_Device_t *)(&g_flashApp),
                                                     (MDS_BOOT_Device_t *)(&g_flashDft), &G_BOOT_UPGRADE_OPS);
    switch (result) {
        case MDS_BOOT_RESULT_NONE:
        case MDS_BOOT_RESULT_SUCCESS:
        case MDS_BOOT_RESULT_ECHECK:
            DRV_CHIP_JumpIntoVectorAddress((uintptr_t)JUMP_APP_ADDRESS);
            break;
        case MDS_BOOT_RESULT_ERETRY:
            DRV_CHIP_JumpIntoDFU();
            break;
        default:
            DRV_CHIP_SystemReset();
            break;
    }
}

void SysTick_Handler(void)
{
    MDS_ClockIncTickCount();
}
