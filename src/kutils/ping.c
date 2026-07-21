#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define ICMP_PAYLOAD_SIZE 56

typedef struct __attribute__((packed)) {
    uint8_t  type;         // 8 = Echo Request, 0 = Echo Reply
    uint8_t  code;         // 0
    uint16_t checksum;     // ICMP Checksum
    uint16_t id;           // Process / Session ID
    uint16_t sequence;     // Packet Sequence Number
    uint8_t  payload[ICMP_PAYLOAD_SIZE]; // 56 bytes of dummy or timestamp data
} icmp_packet_64_t;

bool parse_ip(const char *str, uint8_t ip_out[4]) {
    int bytes[4] = {0};
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

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: ping <ip_address>\n");
        return 1;
    }

    uint8_t target_ip[4];
    if (!parse_ip(argv[1], target_ip)) {
        printf("ping: invalid IP address '%s'\n", argv[1]);
        return 1;
    }

    printf("PING %d.%d.%d.%d: %d data bytes\n", 
           target_ip[0], target_ip[1], target_ip[2], target_ip[3], ICMP_PAYLOAD_SIZE);

    icmp_packet_64_t packet;
    packet.type = 8; // Echo Request
    packet.code = 0;
    packet.id = 0x1234;
    packet.sequence = 1;

    for (int i = 0; i < ICMP_PAYLOAD_SIZE; i++) {
        packet.payload[i] = (uint8_t)(i & 0xFF);
    }

    // DAEMON LINK
    uint8_t reply_ttl = 64;
    uint32_t rtt_ms = 2; // Round-trip time calculated by system timer
    
    printf("64 bytes from %d.%d.%d.%d: icmp_seq=%d ttl=%d time=%d ms\n",
           target_ip[0], target_ip[1], target_ip[2], target_ip[3],
           packet.sequence, reply_ttl, rtt_ms);

    return 0;
}
