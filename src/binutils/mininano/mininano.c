#include "carry.h"

extern void serial_write(const char *s);
extern void print(int val, char *buf, int base);

static uint8_t physical_ram[TOTAL_RAM_SIZE];
static uint8_t page_bitmap[TOTAL_PAGES / 8];

static CarryEngine carry;

static void set_page_used(uint32_t p) { page_bitmap[p / 8] |= (1 << (p % 8)); }
static void set_page_free(uint32_t p) { page_bitmap[p / 8] &= ~(1 << (p % 8)); }
static int  is_page_used(uint32_t p)  { return (page_bitmap[p / 8] & (1 << (p % 8))) != 0; }

static int32_t alloc_page(void) {
    for (uint32_t i = 0; i < TOTAL_PAGES; i++) {
        if (!is_page_used(i)) {
            set_page_used(i);
            uint8_t *ptr = &physical_ram[i * PAGE_SIZE];
            for (int b = 0; b < PAGE_SIZE; b++) ptr[b] = 0;
            return (int32_t)i;
        }
    }
    return -1;
}

void carry_init(void) {
    carry.active_idx = -1;
    carry.active_count = 0;

    for (size_t i = 0; i < sizeof(page_bitmap); i++) {
        page_bitmap[i] = 0;
    }

    carry_create_file("main.mp");
    carry_create_file("scratch.mp");
    carry_switch_file(0);
}

int carry_create_file(const char *filename) {
    if (carry.active_count >= MAX_FILES) return -1;

    int idx = carry.active_count++;
    CarryFile *f = &carry.files[idx];

    int i = 0;
    while (filename[i] != '\0' && i < 31) {
        f->name[i] = filename[i];
        i++;
    }
    f->name[i] = '\0';
    f->page_count = 0;
    f->file_size = 0;
    f->cursor = 0;

    return idx;
}

void carry_switch_file(int idx) {
    if (idx >= 0 && idx < (int)carry.active_count) {
        carry.active_idx = idx;
    }
}

void carry_write_char(char c) {
    if (carry.active_idx < 0) return;

    CarryFile *f = &carry.files[carry.active_idx];
    size_t target_page = f->file_size / PAGE_SIZE;
    size_t offset      = f->file_size % PAGE_SIZE;

    if (target_page >= f->page_count) {
        if (f->page_count >= MAX_PAGES_PER_FILE) return;
        
        int32_t new_page = alloc_page();
        if (new_page < 0) {
            serial_write("\n[CARRY] Out of Memory!\n");
            return;
        }
        f->page_table[f->page_count++] = (uint32_t)new_page;
    }

    uint32_t phys_page = f->page_table[target_page];
    physical_ram[phys_page * PAGE_SIZE + offset] = (uint8_t)c;

    f->file_size++;
    f->cursor = f->file_size;
}

void carry_backspace(void) {
    if (carry.active_idx < 0) return;
    CarryFile *f = &carry.files[carry.active_idx];

    if (f->file_size > 0) {
        f->file_size--;
        f->cursor = f->file_size;

        if (f->file_size % PAGE_SIZE == 0 && f->page_count > 0) {
            uint32_t last_page = f->page_table[--f->page_count];
            set_page_free(last_page);
        }
    }
}

char carry_read_char(int file_idx, size_t offset) {
    if (file_idx < 0 || file_idx >= (int)carry.active_count) return '\0';
    CarryFile *f = &carry.files[file_idx];
    if (offset >= f->file_size) return '\0';

    size_t p_idx = offset / PAGE_SIZE;
    size_t p_off = offset % PAGE_SIZE;

    return (char)physical_ram[f->page_table[p_idx] * PAGE_SIZE + p_off];
}

void carry_status(void) {
    char buf[16];
    serial_write("\n--- CARRY 50MB VFS BUFFER STATUS ---\n");
    for (size_t i = 0; i < carry.active_count; i++) {
        CarryFile *f = &carry.files[i];
        if ((int)i == carry.active_idx) {
            serial_write(" > [ACTIVE] ");
        } else {
            serial_write("   [SLOT]   ");
        }
        serial_write(f->name);
        serial_write(" | Size: ");
        print(f->file_size, buf, 10);
        serial_write(buf);
        serial_write(" B | Pages: ");
        print(f->page_count, buf, 10);
        serial_write(buf);
        serial_write("\n");
    }
    serial_write("-------------------------------------\n");
}

void mininano_render(void) {
    serial_write("\033[2J\033[H");

    if (carry.active_idx < 0) return;
    CarryFile *f = &carry.files[carry.active_idx];

    serial_write("=== MININANO v0.1 | File: ");
    serial_write(f->name);
    serial_write(" ===\n\r");

    for (size_t i = 0; i < f->file_size; i++) {
        char ch[2] = { carry_read_char(carry.active_idx, i), '\0' };
        serial_write(ch);
    }

    serial_write("\n\r-----------------------------------------------\n\r");
    serial_write("[Ctrl+T: Switch Slot | Ctrl+Q: Exit Mininano]\n\r");
}

void mininano_handle_input(char ch) {
    switch (ch) {
        case 0x14:
            carry_switch_file((carry.active_idx + 1) % carry.active_count);
            break;

        case 0x7F:
        case 0x08:
            carry_backspace();
            break;

        case '\r':
        case '\n':
            carry_write_char('\n');
            break;

        default:
            if (ch >= 32 && ch <= 126) {
                carry_write_char(ch);
            }
            break;
    }

    mininano_render();
}
