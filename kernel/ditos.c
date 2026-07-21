#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "ditos.h"

static size_t compress_rle(const uint8_t *src, size_t src_len, uint8_t *dst) {
    size_t src_idx = 0;
    size_t dst_idx = 0;

    while (src_idx < src_len) {
        uint8_t current_byte = src[src_idx];
        uint32_t run_length = 1;

        while ((src_idx + run_length < src_len) && 
               (src[src_idx + run_length] == current_byte) && 
               (run_length < 255)) {
            run_length++;
        }

        dst[dst_idx++] = current_byte;
        dst[dst_idx++] = (uint8_t)run_length;

        src_idx += run_length;
    }

    return dst_idx;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: ditos <input.bin> <output.ditos> [entry_hex]\n");
        return 1;
    }

    const char *input_path = argv[1];
    const char *output_path = argv[2];
    uint32_t entry_point = (argc >= 4) ? (uint32_t)strtoul(argv[3], NULL, 16) : 0x00100000;

    FILE *fin = fopen(input_path, "rb");
    if (!fin) {
        return 1;
    }

    fseek(fin, 0, SEEK_END);
    size_t uncomp_size = ftell(fin);
    fseek(fin, 0, SEEK_SET);

    uint8_t *uncomp_buf = (uint8_t *)malloc(uncomp_size);
    if (!uncomp_buf) {
        fclose(fin);
        return 1;
    }

    if (fread(uncomp_buf, 1, uncomp_size, fin) != uncomp_size) {
        free(uncomp_buf);
        fclose(fin);
        return 1;
    }
    fclose(fin);

    uint8_t *comp_buf = (uint8_t *)malloc(uncomp_size * 2);
    if (!comp_buf) {
        free(uncomp_buf);
        return 1;
    }

    size_t comp_size = compress_rle(uncomp_buf, uncomp_size, comp_buf);

    ditos_header_t hdr;
    hdr.magic       = DITOS_MAGIC;
    hdr.uncomp_size = (uint32_t)uncomp_size;
    hdr.comp_size   = (uint32_t)comp_size;
    hdr.entry_point = entry_point;

    FILE *fout = fopen(output_path, "wb");
    if (!fout) {
        free(uncomp_buf);
        free(comp_buf);
        return 1;
    }

    fwrite(&hdr, sizeof(ditos_header_t), 1, fout);
    fwrite(comp_buf, 1, comp_size, fout);
    fclose(fout);

    free(uncomp_buf);
    free(comp_buf);
    return 0;
}
