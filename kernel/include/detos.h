#ifndef DITOS_H
#define DITOS_H

#include <stdint.h>
#include "init/include/sys_types.h"
#include "init/include/sys_init.h"
#include "init/include/sys_call.h"
#include "init/include/sys_io.h"
#include "init/include/sys_proc.h"


#define DITOS_VERSION_MAJOR 0
#define DITOS_VERSION_MINOR 1
#define DITOS_MAGIC         0x4449544F // "DITO" in ASCII


#define DITOS_KERNEL_LOAD_ADDR ((void*)0x00100000) // LOAD

extern const uint8_t  input_data[];
extern const uint8_t  input_data_end[];
extern const uint32_t input_len;

typedef struct __attribute__((packed)) {
    uint32_t magic;           // verify
    uint32_t comp_size;       // compressed size in bytes
    uint32_t uncomp_size;     // the uncompressed size
    uint32_t entry_point;     // header
} ditos_header_t;

/* --- Core Kernel & Boot Prototypes --- */
void ditos_boot_entry(void);
int  decompress(const void *src, uint32_t src_len, void *dst);

#endif // DITOS_H
