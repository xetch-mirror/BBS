#ifndef SYS_IPC_H
#define SYS_IPC_H

#include "sys_types.h"

#define IPC_MAX_PAYLOAD 256
#define IPC_ANY_SENDER  0xFFFFFFFF

// Core IPC message packet
typedef struct {
    uint32_t sender_pid;
    uint32_t target_pid;
    uint32_t command;       // Opcode (e.g., DISK_CREATE, MOUSE_MOVE)
    uint32_t payload_len;
    uint8_t  payload[IPC_MAX_PAYLOAD];
} message_t;

// Public Kernel IPC Interface
int kipc_send(uint32_t target_pid, message_t *msg);
int kipc_recv(uint32_t sender_pid, message_t *msg);

#endif
