#include "init/include/sys_io.h"
#include "k_call.h"
#include "include/sys_shell.h"
#include "include/sys_string.h"

#define CMD_BUF_SIZE 128

extern void outb(uint16_t port, uint8_t data);

// String helpers
int k_strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) { s1++; s2++; }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

int k_strncmp(const char *s1, const char *s2, uint32_t n) {
    while (n && *s1 && (*s1 == *s2)) { s1++; s2++; n--; }
    if (n == 0) return 0;
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

// Shell Commands
static void cmd_help(void) {
    io_print("BBS Core Commands:\n");
    io_print("  help     - Show command list\n");
    io_print("  clear    - Clear console display\n");
    io_print("  sysinfo  - Display system status\n");
    io_print("  reboot   - Hardware reset\n");
}

static void cmd_sysinfo(void) {
    io_print("Kernel: BBS 32-bit Protected Mode\n");
    io_print("API Layer: MGR Core API\n");
    io_print("Gateway: k_call (LVL 0)\n");
}

static void cmd_clear(void) {
    io_print("\033[2J\033[H");
}

static void cmd_reboot(void) {
    io_print("[sys] pulsing CPU reset...\n");
    outb(0x64, 0xFE);
}

// Command Dispatcher
static void execute_command(char *buf) {
    if (buf[0] == '\0') return;

    if (k_strcmp(buf, "help") == 0) {
        cmd_help();
    } else if (k_strcmp(buf, "clear") == 0) {
        cmd_clear();
    } else if (k_strcmp(buf, "sysinfo") == 0) {
        cmd_sysinfo();
    } else if (k_strcmp(buf, "reboot") == 0) {
        cmd_reboot();
    } else if (k_strncmp(buf, "echo ", 5) == 0) {
        io_print(buf + 5);
        io_print("\n");
    } else {
        io_print("Unknown command: ");
        io_print(buf);
        io_print("\nType 'help' for available commands.\n");
    }
}

// Console Subsystem Loop
void console_subsystem(void) {
    char cmd_buf[CMD_BUF_SIZE];
    int idx = 0;

    io_print("\n===================================\n");
    io_print("       BBS Subsystem Shell         \n");
    io_print("===================================\n");
    io_print("BBS> ");

    while (1) {
        char c = kcall_getchar();

        if (c == '\n' || c == '\r') {
            io_print("\n");
            cmd_buf[idx] = '\0';
            execute_command(cmd_buf);
            idx = 0;
            io_print("BBS> ");
        } else if (c == '\b' || c == 0x7F) {
            if (idx > 0) {
                idx--;
                io_print("\b \b");
            }
        } else if (c >= ' ' && c <= '~' && idx < CMD_BUF_SIZE - 1) {
            cmd_buf[idx++] = c;
            char echo[2] = {c, '\0'};
            io_print(echo);
        }

        kcall_yield();
    }
}
