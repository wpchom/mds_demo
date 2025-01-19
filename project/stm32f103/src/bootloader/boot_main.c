#include "mds_boot.h"
#include "drv_chip.h"

#define BOOT_FIRMWARE_VERSION 0x00000001U
#define BOOT_FIRMWARE_TYPE    0xA0A0U

typedef struct BOOT_FlashDevice {
    uint32_t baseAddr;
    uint32_t pageNums;
} BOOT_FlashDevice_t;

static BOOT_FlashDevice_t g_flashApp = {
    .baseAddr = 0x08002800,
    .pageNums = 32,
};

static BOOT_FlashDevice_t g_flashDft = {
    .baseAddr = 0x0800A800,
    .pageNums = 22,
};

extern void __APP_JUMP_ADDRESS(void);
uint32_t BOOT_GetResetReason(void)
{
    uint32_t flag = RCC->CSR & (RCC_FLAG_PINRST | RCC_FLAG_PORRST | RCC_FLAG_SFTRST | RCC_FLAG_IWDGRST |
                                RCC_FLAG_WWDGRST | RCC_FLAG_LPWRRST);
    RCC->CSR |= RCC_CSR_RMVF;

    return (flag);
}

static MDS_Err_t BOOT_FlashRead(MDS_Arg_t *dev, intptr_t ofs, uint8_t *data, size_t len)
{
    BOOT_FlashDevice_t *flashDev = (BOOT_FlashDevice_t *)dev;

    MDS_MemBuffCopy(data, len, (void *)(flashDev->baseAddr + ofs), len);

    return (MDS_EOK);
}

static MDS_Err_t BOOT_FlashWrite(MDS_Arg_t *dev, intptr_t ofs, const uint8_t *data, size_t len)
{
    BOOT_FlashDevice_t *flashDev = (BOOT_FlashDevice_t *)dev;
    HAL_StatusTypeDef status = HAL_OK;

    HAL_FLASH_Unlock();
    for (size_t i = 0; i < len; i += sizeof(uint32_t)) {
        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, flashDev->baseAddr + ofs + i, *(uint32_t *)(data + i));
        if (status != HAL_OK) {
            break;
        }
    }
    HAL_FLASH_Lock();

    return (DRV_HalStatusToMdsErr(status));
}

static MDS_Err_t BOOT_FlashErase(MDS_Arg_t *dev)
{
    BOOT_FlashDevice_t *flashDev = (BOOT_FlashDevice_t *)dev;

    FLASH_EraseInitTypeDef eraseInit = {
        .TypeErase = FLASH_TYPEERASE_PAGES,
        .Banks = FLASH_BANK_1,
        .PageAddress = flashDev->baseAddr,
        .NbPages = flashDev->pageNums,
    };
    uint32_t pageError = 0;

    HAL_FLASH_Unlock();
    HAL_StatusTypeDef status = HAL_FLASHEx_Erase(&eraseInit, &pageError);
    HAL_FLASH_Lock();

    return (DRV_HalStatusToMdsErr(status));
}

static bool BOOT_UpgradeCompare(const MDS_BOOT_UpgradeInfo_t *upgradeInfo)
{
    if (ALGO_GetU16BE(upgradeInfo->type) != BOOT_FIRMWARE_TYPE) {
        return (false);
    }

    return (true);
}

const MDS_BOOT_UpgradeOps_t G_BOOT_UPGRADE_OPS = {
    .read = BOOT_FlashRead,
    .write = BOOT_FlashWrite,
    .erase = BOOT_FlashErase,
    .compare = BOOT_UpgradeCompare,
};

int main(void)
{
    HAL_Init();

#if (MDS_CLOCK_TICK_FREQ_HZ != 1000U)
    SysTick_Config(SystemCoreClock / MDS_CLOCK_TICK_FREQ_HZ);
#endif

    MDS_BOOT_SwapInfo_t *swapInfo = MDS_BOOT_GetSwapInfo();
    if (swapInfo != NULL) {
        swapInfo->version = BOOT_FIRMWARE_VERSION;
        swapInfo->reset = BOOT_GetResetReason();
    }

    MDS_BOOT_Result_t result = MDS_BOOT_UpgradeCheck(swapInfo, (MDS_Arg_t *)(&g_flashApp), (MDS_Arg_t *)(&g_flashDft),
                                                     &G_BOOT_UPGRADE_OPS);
    switch (result) {
        case MDS_BOOT_RESULT_NONE:
        case MDS_BOOT_RESULT_SUCCESS:
        case MDS_BOOT_RESULT_ECHECK:
            STARTUP_JumpIntoVectorTable((uintptr_t)__APP_JUMP_ADDRESS);
            break;
        case MDS_BOOT_RESULT_ERETRY:
            STARTUP_JumpIntoDFU();
            break;
        default:
            STARTUP_SystemReset();
            break;
    }
}

void SysTick_Handler(void)
{
    MDS_ClockIncTickCount();
}
