#include "miniphone.h"

// External references to your core console output engine
extern void serial_write(const char *str);
extern int print(int value, char *buf, int base); // Custom xstring integer converter

/* 
 * Safely parses an open file buffer line-by-line and feeds it directly 
 * back into the structural interpreter environment context.
 */
static void mph_interpret_buffer(MphContext *ctx, const char *buffer, size_t size, void* (*ramdisk_lookup)(const char*)) {
    char line_buf[128];
    size_t line_idx = 0;

    for (size_t i = 0; i < size; i++) {
        char c = buffer[i];

        // If we hit a newline or null terminator, execute the structural statement line
        if (c == '\n' || c == '\r' || c == '\0') {
            if (line_idx > 0) {
                line_buf[line_idx] = '\0';
                mph_interpret_line(ctx, line_buf, ramdisk_lookup);
                line_idx = 0; // Reset line buffer offset index
            }
            if (c == '\0') break;
        } else {
            // Safe copy bounds checking to ensure long script lines don't smash kernel memory arrays
            if (line_idx < sizeof(line_buf) - 1) {
                line_buf[line_idx++] = c;
            }
        }
    }

    // Capture any final trailing line hanging at the end of the file descriptor stream
    if (line_idx > 0) {
        line_buf[line_idx] = '\0';
        mph_interpret_line(ctx, line_buf, ramdisk_lookup);
    }
}

/*
 * The external entry point called by your VFS shell loop when a user runs an inline string 
 * statement or invokes an internal file script configuration.
 */
void mph_execute_script(MphContext *ctx, const char *filename, void* (*ramdisk_lookup)(const char*)) {
    if (!ramdisk_lookup || !filename) return;

    // Utilize the driver lookup pointer passed by the environment configuration
    const char *file_data = (const char *)ramdisk_lookup(filename);

    if (file_data != NULL) {
        // Safe look ahead: assume an upper limit of file read data or parse until the termination character
        size_t file_size = 0;
        while (file_data[file_size] != '\0' && file_size < 4096) {
            file_size++;
        }

        serial_write("[MINIPHONE] Executing header stream target: ");
        serial_write(filename);
        serial_write("\n");

        // Parse and process file segments
        mph_interpret_buffer(ctx, file_data, file_size, ramdisk_lookup);
    } else {
        serial_write("[MINIPHONE ERROR] Script target target resource not found.\n");
    }
}

/*
 * Practical utility to output all currently registered script tokens to your serial TTY console
 */
void mph_dump_symbols(MphContext *ctx) {
    char num_buf[32];
    serial_write("--- MP BULID FOR BBS ---\n");
    
    for (int i = 0; i < ctx->symbol_count; i++) {
        if (ctx->symbols[i].is_constant) {
            serial_write("  const ");
        } else {
            serial_write("  let   ");
        }
        
        serial_write(ctx->symbols[i].name);
        serial_write(" = ");
        
        print(ctx->symbols[i].value, num_buf, 10);
        serial_write(num_buf);
        serial_write("\n");
    }
    serial_write("--------------------------------------------------\n");
}
