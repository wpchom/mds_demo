#ifndef __PUBLIC_FILE_H__
#define __PUBLIC_FILE_H__

#include "mds_fs.h"
#include "mds_emfs.h"

#ifdef __cplusplus
extern "C" {
#endif

const MDS_FileSystem_t *FILE_SYSTEM_DATA(void);

#define PUBLIC_FILE_LIST                                                                                               \
    PUBLIC_FILE_DEF(FILE_ID_BOOTINFO, FILE_SYSTEM_DATA, 0x00, MDS_EMFS_FileDesc_t)                                     \
    PUBLIC_FILE_DEF(FILE_ID_DEVINFO, FILE_SYSTEM_DATA, 0x01, MDS_EMFS_FileDesc_t)

#ifdef __cplusplus
}
#endif

#endif /* __PUBLIC_FILE_H__ */
