#include "decompressor.h"
#include "init/sys_io.h"

void decompress_init(decompress_ctx_t *ctx, const uint8_t *src, size_t src_size, uint8_t *dest, size_t dest_capacity) {
    if (!ctx) return;
    ctx->src = src;
    ctx->dest = dest;
    ctx->src_size = src_size;
    ctx->dest_capacity = dest_capacity;
    ctx->bytes_read = 0;
    ctx->bytes_written = 0;
    ctx->state = DECOMPRESS_STATE_INIT;
}

int32_t decompress_bbs_block(decompress_ctx_t *ctx, size_t block_size) {
    if (!ctx || !ctx->src || !ctx->dest) return DECOMPRESS_ERR_NULL_PTR;
    if (ctx->state == DECOMPRESS_STATE_FAILED) return DECOMPRESS_ERR_CORRUPT;

    ctx->state = DECOMPRESS_STATE_RUNNING;
    size_t target_read = ctx->bytes_read + block_size;
    if (target_read > ctx->src_size) target_read = ctx->src_size;

    while (ctx->bytes_read < target_read) {
        uint8_t count = ctx->src[ctx->bytes_read++];
        if (ctx->bytes_read >= ctx->src_size) {
            ctx->state = DECOMPRESS_STATE_FAILED;
            return DECOMPRESS_ERR_BOUNDS;
        }
        uint8_t val = ctx->src[ctx->bytes_read++];
        if (ctx->bytes_written + count > ctx->dest_capacity) {
            ctx->state = DECOMPRESS_STATE_FAILED;
            return DECOMPRESS_ERR_OVERFLOW;
        }
        for (uint16_t i = 0; i < count; i++) {
            ctx->dest[ctx->bytes_written++] = val;
        }
    }
    if (ctx->bytes_read >= ctx->src_size) ctx->state = DECOMPRESS_STATE_COMPLETED;
    return DECOMPRESS_SUCCESS;
}

int32_t decompress_bbs(const uint8_t *src, size_t src_len, uint8_t *dest, size_t dest_max_len, size_t *out_written) {
    if (!src || !dest) return DECOMPRESS_ERR_NULL_PTR;

    io_print("decompressing BBS...");

    decompress_ctx_t ctx;
    decompress_init(&ctx, src, src_len, dest, dest_max_len);

    int32_t status = decompress_bbs_block(&ctx, src_len);
    if (status != DECOMPRESS_SUCCESS) {
        io_print(" Failed!\n");
        if (out_written) *out_written = 0;
        return status;
    }

    if (out_written) *out_written = ctx.bytes_written;
    io_print(" Ok starting kernel\n");
    return DECOMPRESS_SUCCESS;
}

size_t decompress_get_bytes_read(const decompress_ctx_t *ctx) {
    return ctx ? ctx->bytes_read : 0;
}

size_t decompress_get_bytes_written(const decompress_ctx_t *ctx) {
    return ctx ? ctx->bytes_written : 0;
}

void decompress_reset(decompress_ctx_t *ctx) {
    if (!ctx) return;
    ctx->bytes_read = 0;
    ctx->bytes_written = 0;
    ctx->state = DECOMPRESS_STATE_INIT;
}
