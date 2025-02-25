#include "board.h"
#include "drv_flash.h"
#include "public_file.h"

static DEV_STORAGE_Adaptr_t g_flashAdaptr;
static DEV_STORAGE_Periph_t g_flashNV;
static DEV_STORAGE_Periph_t g_flashUV;
static DEV_STORAGE_Periph_t g_flashDFT;

static void BOARD_FLASH_Init(void)
{
    MDS_Err_t err = DEV_STORAGE_AdaptrInit(&g_flashAdaptr, FLASH_MODULE, &G_DRV_GD32E11X_FLASH, NULL, NULL);
    if (err != MDS_EOK) {
        MDS_LOG_E("[flash] adaptr init failed");
        return;
    }

    err = DEV_STORAGE_PeriphInit(&g_flashNV, FLASH_NV, &g_flashAdaptr);
    if (err == MDS_EOK) {
        g_flashNV.object.baseAddr = 0x08002800;
        g_flashNV.object.blockNums = 10 * 1024 / DRV_FLASH_PAGE_SIZE;
    }

    err = DEV_STORAGE_PeriphInit(&g_flashUV, FLASH_UV, &g_flashAdaptr);
    if (err == MDS_EOK) {
        g_flashUV.object.baseAddr = 0x08003000;
        g_flashUV.object.blockNums = 4 * 1024 / DRV_FLASH_PAGE_SIZE;
    }

    err = DEV_STORAGE_PeriphInit(&g_flashDFT, FLASH_DFT, &g_flashAdaptr);
    if (err == MDS_EOK) {
        g_flashDFT.object.baseAddr = 0x08015000;
        g_flashDFT.object.blockNums = 44 * 1024 / DRV_FLASH_PAGE_SIZE;
    }
}
MDS_INIT_IMPORT(MDS_INIT_PRIORITY_1, BOARD_FLASH_Init);

const MDS_FileSystem_t *FILE_SYSTEM_DATA(void)
{
    MDS_Err_t err = MDS_EOK;

    static uint8_t buff[512];
    static const MDS_EMFS_FsInitStruct_t init = {
        .device = &g_flashUV,
        .buff = buff,
        .size = sizeof(buff),
    };

    static MDS_EMFS_FileSystem_t emfs = {0};

    if (emfs.init.device == NULL) {
        err = MDS_EMFS_Mount(&emfs, &init);
        if (err != MDS_EOK) {
            MDS_LOG_W("[FILE_SYSTEM_DATA] emfs mount fail to mkfs");
            err = MDS_EMFS_Mkfs(init.device, init.size);
            if (err == MDS_EOK) {
                err = MDS_EMFS_Mount(&emfs, &init);
            }
        }
    }

    static const MDS_FileSystem_t fs = {
        .ops = &G_FILESYSTEM_EMFS_OPS,
        .data = (MDS_Arg_t *)(&emfs),
    };

    return ((err == MDS_EOK) ? (&fs) : (NULL));
}

#define PUBLIC_FILE_DEF(fileId, fileSystem, filePath, fileDesc_t)                                                      \
    MDS_FileNode_t *fileId(void)                                                                                       \
    {                                                                                                                  \
        static fileDesc_t fd;                                                                                          \
        static MDS_FileNode_t fn;                                                                                      \
                                                                                                                       \
        if (fn.fs == NULL) {                                                                                           \
            fn.fs = (MDS_FileSystem_t *)fileSystem;                                                                    \
            fn.path = (char *)filePath;                                                                                \
            fn.data = (MDS_Arg_t *)(&fd);                                                                              \
        }                                                                                                              \
                                                                                                                       \
        return (&fn);                                                                                                  \
    }

PUBLIC_FILE_LIST

#undef PUBLIC_FILE_DEF
