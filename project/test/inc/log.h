#ifndef __LOG_H__
#define __LOG_H__

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {
    const void *fmt;
    uint8_t enabled;
} TRACE_Node_t;

#define MDS_TRACE_REGISTER(_name, _enable)                                                                             \
    const TRACE_Node_t G_MDS_TRACE_NODE_##_name = {.fmt = "OK", .enabled = _enable};                                                \
    MDS_TRACE_DECLARE(_name)

#define MDS_TRACE_DECLARE(_name)                                                                                       \
    extern const TRACE_Node_t G_MDS_TRACE_NODE_##_name;                                                                \
    __used static const TRACE_Node_t *g_this = &(G_MDS_TRACE_NODE_##_name)

#define MDS_TRACE(...)                                                                                                 \
    do {                                                                                                               \
        if (g_this->enabled) {                                                                                         \
            printf(__VA_ARGS__);                                                                                       \
        }                                                                                                              \
    } while (0)

#endif
