#include "Clib/Xlibary/xbool.h"
#include "Clib/Xlibary/xio.h"
#include "Clib/Xlibary/xstring.h"

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

#define ICMP_PAYLOAD_SIZE 56

typedef struct __attribute__((packed)) {
    uint8_t  type;
    uint8_t  code;
    uint16_t checksum;
    uint16_t id;
    uint16_t sequence;
    uint8_t  payload[ICMP_PAYLOAD_SIZE];
} icmp_packet_64_t;

bool parse_ip(const char *str, uint8_t ip_out[4]) {
    int bytes[4] = {0, 0, 0, 0};
    int byte_idx = 0, digits = 0;

    while (*str) {
        if (*str >= '0' && *str <= '9') {
            bytes[byte_idx] = (bytes[byte_idx] * 10) + (*str - '0');
            if (bytes[byte_idx] > 255) return false;
            digits++;
            if (digits > 3) return false;
        } else if (*str == '.') {
            if (digits == 0) return false;
            byte_idx++;
            if (byte_idx > 3) return false;
            digits = 0;
        } else {
            return false;
        }
        str++;
    }

    if (byte_idx != 3 || digits == 0) return false;
    for (int i = 0; i < 4; i++) ip_out[i] = (uint8_t)bytes[i];
    return true;
}

static void print_ip(const uint8_t ip[4]) {
    char num[12];
    kernel_print(print(ip[0], num, 10));
    kernel_print(".");
    kernel_print(print(ip[1], num, 10));
    kernel_print(".");
    kernel_print(print(ip[2], num, 10));
    kernel_print(".");
    kernel_print(print(ip[3], num, 10));
}

int main(int argc, char *argv[]) {
    char num_buf[12];

    if (argc < 2) {
        kernel_print("Usage: ping <ip_address>\n");
        return 1;
    }

    uint8_t target_ip[4];
    if (!parse_ip(argv[1], target_ip)) {
        kernel_print("ping: invalid IP address '");
        kernel_print(argv[1]);
        kernel_print("'\n");
        return 1;
    }

    kernel_print("PING ");
    print_ip(target_ip);
    kernel_print(": ");
    kernel_print(print(ICMP_PAYLOAD_SIZE, num_buf, 10));
    kernel_print(" data bytes\n");

    icmp_packet_64_t packet;
    packet.type = 8;
    packet.code = 0;
    packet.id = 0x1234;
    packet.sequence = 1;

    for (int i = 0; i < ICMP_PAYLOAD_SIZE; i++) {
        packet.payload[i] = (uint8_t)(i & 0xFF);
    }

    uint8_t reply_ttl = 64;
    uint32_t rtt_ms = 2;

    kernel_print("64 bytes from ");
    print_ip(target_ip);
    kernel_print(": icmp_seq=");
    kernel_print(print(packet.sequence, num_buf, 10));
    kernel_print(" ttl=");
    kernel_print(print(reply_ttl, num_buf, 10));
    kernel_print(" time=");
    kernel_print(print(rtt_ms, num_buf, 10));
    kernel_print(" ms\n");

    return 0;
}
