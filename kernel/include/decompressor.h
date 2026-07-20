#ifndef DECOMPRESSOR_H
#define DECOMPRESSOR_H

#include "init/sys_types.h"

#define DECOMPRESS_SUCCESS          0
#define DECOMPRESS_ERR_NULL_PTR    -1
#define DECOMPRESS_ERR_BOUNDS      -2
#define DECOMPRESS_ERR_CORRUPT     -3
#define DECOMPRESS_ERR_OVERFLOW    -4

#define RLE_MARKER_BYTE          0xAA
#define MAX_RUN_LENGTH            255
#define DECOMPRESS_BUFFER_SIZE   4096

typedef enum {
    DECOMPRESS_STATE_INIT = 0,
    DECOMPRESS_STATE_RUNNING,
    DECOMPRESS_STATE_COMPLETED,
    DECOMPRESS_STATE_FAILED
} decompress_state_t;

typedef struct {
    const uint8_t *src;
    uint8_t *dest;
    size_t src_size;
    size_t dest_capacity;
    size_t bytes_read;
    size_t bytes_written;
    decompress_state_t state;
} decompress_ctx_t;

void decompress_init(
    decompress_ctx_t *ctx,
    const uint8_t *src,
    size_t src_size,
    uint8_t *dest,
    size_t dest_capacity
);

int32_t decompress_bbs_block(
    decompress_ctx_t *ctx,
    size_t block_size
);

int32_t decompress_bbs(
    const uint8_t *src,
    size_t src_len,
    uint8_t *dest,
    size_t dest_max_len,
    size_t *out_written
);

size_t decompress_get_bytes_read(const decompress_ctx_t *ctx);

size_t decompress_get_bytes_written(const decompress_ctx_t *ctx);

const char *decompress_strerror(int32_t err_code);

void decompress_reset(decompress_ctx_t *ctx);

#endif
