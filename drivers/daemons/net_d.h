#ifndef NET_D_H
#define NET_D_H

#include "sys_types.h"

void net_init(uint16_t io_base);
void net_send_packet(uint8_t *data, uint32_t len);
void net_handle_receive(void);

#endif
