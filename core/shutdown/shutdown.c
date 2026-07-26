
typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;
typedef unsigned long  uint64_t;
typedef unsigned int   uintptr_t;


struct RSDPDescriptor {
    char Signature[8];
    uint8_t Checksum;
    char OEMID[6];
    uint8_t Revision;
    uint32_t RsdtAddress; // 32-bit physical address of RSDT
} __attribute__((packed));

// Standard ACPI Table Header
struct ACPISDTHeader {
    char Signature[4];
    uint32_t Length;
    uint8_t Revision;
    uint8_t Checksum;
    char OEMID[6];
    char OEMTableID[8];
    uint32_t OEMRevision;
    uint32_t CreatorID;
    uint32_t CreatorRevision;
} __attribute__((packed));

// Fixed ACPI Description Table (FADT)
struct FADT {
    struct ACPISDTHeader h;
    uint32_t FirmwareCtrl;
    uint32_t DSDT;               // Physical address of the DSDT table
    uint8_t  Reserved;
    uint8_t  Preferred_PM_Profile;
    uint16_t SCI_Interrupt;
    uint32_t SMI_CmdPort;        // Port to enable ACPI
    uint8_t  AcpiEnable;
    uint8_t  AcpiDisable;
    uint32_t PM1a_EvtBlk;        // Power Management 1a Event Block
    uint32_t PM1b_EvtBlk;        // Power Management 1b Event Block
    uint32_t PM1a_CntBlk;        // Power Management 1a Control Register Block
    uint32_t PM1b_CntBlk;        // Power Management 1b Control Register Block
    // Note: Truncated for compilation safety; real hardware may have more bytes here
} __attribute__((packed));


// --- Bare-Metal Hardware Helpers ---

static inline void outw(uint16_t port, uint16_t val) {
    __asm__ volatile ("outw %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint16_t inw(uint16_t port) {
    uint16_t ret;
    __asm__ volatile ("inw %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

int os_strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) { s1++; s2++; }
    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

// --- ACPI Table Parsing Engine ---

struct FADT* global_fadt = 0;
uint16_t SLP_TYPa = 0;
uint16_t SLP_TYPb = 0;

// Scans memory to find DSDT bytecode and extract Sleep State variables (\_S5 object)
void parse_dsdt(uint32_t dsdt_address) {
    struct ACPISDTHeader *dsdt = (struct ACPISDTHeader*)(uintptr_t)dsdt_address;
    char *S5_ptr = (char*)dsdt + sizeof(struct ACPISDTHeader);
    int dsdt_length = dsdt->Length - sizeof(struct ACPISDTHeader);

    // Look for the AML opcode representing the \_S5 (Shutdown) sleep object
    while (dsdt_length-- > 0) {
        if (os_strcmp(S5_ptr, "_S5_") == 0 || (S5_ptr[0] == '_' && S5_ptr[1] == 'S' && S5_ptr[2] == '5')) {
            break;
        }
        S5_ptr++;
    }

    if (dsdt_length <= 0) return; // \_S5 structure not found

    // Advance past the AML structural opcodes to find packet payload
    S5_ptr += 4; 
    if (*S5_ptr == 0x12) S5_ptr += 3; // Skip AML Package OpCode
    else S5_ptr += 2;

    if (*S5_ptr == 0x0A) S5_ptr++; // Skip BytePrefix

    // Extract hardware-specific sleep configurations
    SLP_TYPa = *(S5_ptr) << 10;
    S5_ptr++;
    if (*S5_ptr == 0x0A) S5_ptr++;
    SLP_TYPb = *(S5_ptr) << 10;
}

// Scans physical memory tables given by the bootloader/GRUB multimap
void init_acpi(struct RSDPDescriptor* rsdp) {
    struct ACPISDTHeader *rsdt = (struct ACPISDTHeader*)(uintptr_t)rsdp->RsdtAddress;
    int entries = (rsdt->Length - sizeof(struct ACPISDTHeader)) / 4;
    uint32_t *pointers = (uint32_t*)((char*)rsdt + sizeof(struct ACPISDTHeader));

    for (int i = 0; i < entries; i++) {
        struct ACPISDTHeader *h = (struct ACPISDTHeader*)(uintptr_t)pointers[i];
        if (h->Signature[0] == 'F' && h->Signature[1] == 'A' && h->Signature[2] == 'C' && h->Signature[3] == 'P') {
            global_fadt = (struct FADT*)h;
            parse_dsdt(global_fadt->DSDT);
            break;
        }
    }
}



void hardware_shutdown(void) {
     
    if (!global_fadt) {
        outw(0xB004, 0x2000); 
        outw(0x604,  0x2000); 
        outw(0x4004, 0x3400); 
        while(1) { __asm__ volatile("hlt"); }
    }

     
    outw(global_fadt->SMI_CmdPort, global_fadt->AcpiEnable);
    
    
    for (volatile int i = 0; i < 5000; i++);

    uint16_t SLP_EN = 1 << 13;
    
    outw(global_fadt->PM1a_CntBlk, SLP_TYPa | SLP_EN);
    if (global_fadt->PM1b_CntBlk != 0) {
        outw(global_fadt->PM1b_CntBlk, SLP_TYPb | SLP_EN);
    }

     
    while(1) { __asm__ volatile("hlt"); }
}

int os_cmd_shutdown(int argc, char *argv[]) {
    int delay = 0;
    int perform_halt = 0;

    for (int i = 1; i < argc; i++) {
        if (os_strcmp(argv[i], "-now") == 0) {
            delay = 0;
        } else if (os_strcmp(argv[i], "-h") == 0) {
            perform_halt = 1;
        } else if (os_strcmp(argv[i], "-t") == 0) {
            if (i + 1 < argc) {
                delay = 0;
                char *p = argv[++i];
                while (*p >= '0' && *p <= '9') {
                    delay = delay * 10 + (*p - '0');
                    p++;
                }
            }
        } else if (os_strcmp(argv[i], "-c") == 0) {
            return 0; // Abort operation sequence immediately
        }
    }

    // Delay wrapper loop (In production OS, replace with PIT timer ticks interrupt)
    if (delay > 0) {
        for (volatile uint64_t i = 0; i < (delay * 100000000ULL); i++) {
            __asm__ volatile("nop");
        }
    }

    if (perform_halt) {
        hardware_shutdown();
    }

    return 0;
}
