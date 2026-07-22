#ifndef CARRY_H
#define CARRY_H

#include <stdint.h>
#include <stddef.h>

#define PAGE_SIZE          4096                   // 4 KB page allocation
#define TOTAL_RAM_SIZE     (50 * 1024 * 1024)     // do not break it's potential
#define TOTAL_PAGES        (TOTAL_RAM_SIZE / PAGE_SIZE) // page

#define MAX_FILES          16
#define MAX_PAGES_PER_FILE 3200                  // 12.8mb for each file, 16 files only

typedef struct {
    char name[32];
    uint32_t page_table[MAX_PAGES_PER_FILE];
    size_t page_count;
    size_t file_size;
    size_t cursor;
} CarryFile;

typedef struct {
    CarryFile files[MAX_FILES];
    int active_idx;
    size_t active_count;
} CarryEngine;

// api
void carry_init(void);
int  carry_create_file(const char *filename);
void carry_switch_file(int idx);
void carry_write_char(char c);
void carry_backspace(void);
char carry_read_char(int file_idx, size_t offset);
void carry_status(void);

void mininano_render(void);
void mininano_handle_input(char ch);

#endif /* CARRY_H */
