#include "sys_ipc.h"
#include "sys_io.h"

#define DISK_CMD_CREATE 0x01
#define DISK_CMD_DELETE 0x02

void disk_daemon_loop(void) {
    message_t msg;

    while (1) {
        // Block/Listen for incoming IPC commands from any process
        if (kipc_recv(IPC_ANY_SENDER, &msg) == 0) {
            switch (msg.command) {
                case DISK_CMD_CREATE:
                    io_print("[disk_d] received command: creating file..\n");
                    break;

                case DISK_CMD_DELETE:
                    io_print("[disk_d] received command: deleting file..\n");
                    break;

                default:
                    break;
            }
        }
    }
}
