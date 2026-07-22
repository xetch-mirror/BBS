#include "net_d.h"
#include "kernel/include/kio_ports.h"
#include "kernel/include/ktime.h"
#include "init/include/sys_io.h"
#include "syslogd.h"

#define INTEL_VEND 0x8086
#define E1000_DEV  0x100E

// Hardware Registers
#define REG_CTRL     0x0000
#define REG_RCTL     0x0100
#define REG_TCTL     0x0400
#define REG_RDBAL    0x2800
#define REG_RDBAH    0x2804
#define REG_RDLEN    0x2808
#define REG_RDH      0x2810
#define REG_RDT      0x2818
#define REG_TDBAL    0x3800
#define REG_TDBAH    0x3804
#define REG_TDLEN    0x3808
#define REG_TDH      0x3810
#define REG_TDT      0x3818

#define NUM_RX_DESC  16
#define NUM_TX_DESC  16
#define BUF_SIZE     2048

#define mmio_write32(addr, val) (*(volatile uint32_t *)(addr) = (val))
#define mmio_read32(addr)       (*(volatile uint32_t *)(addr))

typedef struct {
    uint64_t addr;
    uint16_t length;
    uint16_t checksum;
    uint8_t  status;
    uint8_t  errors;
    uint16_t special;
} __attribute__((packed)) e1000_rx_desc_t;

typedef struct {
    uint64_t addr;
    uint16_t length;
    uint8_t  cso;
    uint8_t  cmd;
    uint8_t  status;
    uint8_t  css;
    uint16_t special;
} __attribute__((packed)) e1000_tx_desc_t;

static uint8_t *e1000_mmio_base = 0;
static e1000_rx_desc_t rx_descs[NUM_RX_DESC] __attribute__((aligned(16)));
static e1000_tx_desc_t tx_descs[NUM_TX_DESC] __attribute__((aligned(16)));
static uint8_t rx_buffers[NUM_RX_DESC][BUF_SIZE] __attribute__((aligned(16)));
static uint8_t tx_buffers[NUM_TX_DESC][BUF_SIZE] __attribute__((aligned(16)));

static uint16_t rx_cur = 0;
static uint16_t tx_cur = 0;

static inline void e1000_write(uint32_t reg, uint32_t val) {
    mmio_write32(e1000_mmio_base + reg, val);
}

static inline uint32_t e1000_read(uint32_t reg) {
    return mmio_read32(e1000_mmio_base + reg);
}

void net_init(uint16_t io_base) {
    (void)io_base;
    e1000_mmio_base = (uint8_t *)pci_get_bar0(INTEL_VEND, E1000_DEV);

    if (!e1000_mmio_base) {
        syslogd_log(LOG_ERR, "net_d", "E1000 MMIO lookup failed");
        io_print("[net_d] error: E1000 NIC not found on PCI bus\n");
        return;
    }

    // restart device
    uint32_t ctrl = e1000_read(REG_CTRL);
    e1000_write(REG_CTRL, ctrl | (1 << 5));

    // setup 
    for (int i = 0; i < NUM_RX_DESC; i++) {
        rx_descs[i].addr = (uint64_t)(uintptr_t)rx_buffers[i];
        rx_descs[i].status = 0;
    }
    e1000_write(REG_RDBAL, (uint32_t)(uintptr_t)rx_descs);
    e1000_write(REG_RDBAH, 0);
    e1000_write(REG_RDLEN, NUM_RX_DESC * sizeof(e1000_rx_desc_t));
    e1000_write(REG_RDH, 0);
    e1000_write(REG_RDT, NUM_RX_DESC - 1);

    uint32_t rctl = e1000_read(REG_RCTL);
    e1000_write(REG_RCTL, rctl | (1 << 1) | (1 << 15) | (1 << 26));

    // ring setup
    for (int i = 0; i < NUM_TX_DESC; i++) {
        tx_descs[i].addr = (uint64_t)(uintptr_t)tx_buffers[i];
        tx_descs[i].cmd = 0;
        tx_descs[i].status = 1;
    }
    e1000_write(REG_TDBAL, (uint32_t)(uintptr_t)tx_descs);
    e1000_write(REG_TDBAH, 0);
    e1000_write(REG_TDLEN, NUM_TX_DESC * sizeof(e1000_tx_desc_t));
    e1000_write(REG_TDH, 0);
    e1000_write(REG_TDT, 0);

    uint32_t tctl = e1000_read(REG_TCTL);
    e1000_write(REG_TCTL, tctl | (1 << 1) | (1 << 3));

    syslogd_log(LOG_INFO, "net_d", "initialized");
    io_print("[net_d] network daemon ready\n");
}

void net_send_packet(uint8_t *data, uint32_t len) {
    if (len > BUF_SIZE) return;

    uint64_t start_ticks = ktime_get_ticks();

    for (uint32_t i = 0; i < len; i++) {
        tx_buffers[tx_cur][i] = data[i];
    }

    tx_descs[tx_cur].length = len;
    tx_descs[tx_cur].cmd = (1 << 0) | (1 << 1); // EOP | IFCS
    tx_descs[tx_cur].status = 0;

    tx_cur = (tx_cur + 1) % NUM_TX_DESC;
    e1000_write(REG_TDT, tx_cur);

    uint64_t duration = ktime_get_ticks() - start_ticks;
    (void)duration; // Available for kernel latency profiling via ktime

    syslogd_log(LOG_DEBUG, "net_d", "packet transmitted");
}

void net_handle_receive(void) {
    while (rx_descs[rx_cur].status & 0x01) {
        uint16_t len = rx_descs[rx_cur].length;
        uint8_t *pkt = rx_buffers[rx_cur];
        (void)pkt;
        (void)len;

        syslogd_log(LOG_INFO, "net_d", "packet received");

        rx_descs[rx_cur].status = 0;
        uint16_t old_cur = rx_cur;
        rx_cur = (rx_cur + 1) % NUM_RX_DESC;
        e1000_write(REG_RDT, old_cur);
    }
}
