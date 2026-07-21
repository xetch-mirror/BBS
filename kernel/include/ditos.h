#ifndef DITOS_H
#define DITOS_H

#include <stdint.h>

#define DITOS_MAGIC 0x4449544F

typedef struct __attribute__((packed)) {
    uint32_t magic;
    uint32_t uncomp_size;
    uint32_t comp_size;
    uint32_t entry_point;
} ditos_header_t;

#endif
