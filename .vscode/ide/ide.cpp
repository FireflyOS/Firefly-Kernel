/* ide.cpp
 *   by SergeyMC9730
 *
 * Created:
 *   9/26/2021, 8:28:20 PM
 * Last edited:
 *   9/27/2021, 6:01:04 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   PCI IDE Controller Driver
**/

#include <stl/cstdlib/stdio.h>

#include <x86_64/drivers/ports.hpp>
#include <x86_64/sleep.hpp>

extern "C" void ide_read_buffer0(void);
extern "C" void ide_read_buffer1(void);

namespace firefiy::kernel::ide {

namespace ata {
enum ata_status : short {
    ATA_SR_BSY = 0x80,   // Busy
    ATA_SR_DRDY = 0x40,  // Drive ready
    ATA_SR_DF = 0x20,    // Drive write fault
    ATA_SR_DSC = 0x10,   // Drive seek complete
    ATA_SR_DRQ = 0x08,   // Data request ready
    ATA_SR_CORR = 0x04,  // Corrected data
    ATA_SR_IDX = 0x02,   // Index
    ATA_SR_ERR = 0x01    // Error
};

enum ata_errors : short {
    ATA_ER_BBK = 0x80,    // Bad block
    ATA_ER_UNC = 0x40,    // Uncorrectable data
    ATA_ER_MC = 0x20,     // Media changed
    ATA_ER_IDNF = 0x10,   // ID mark not found
    ATA_ER_MCR = 0x08,    // Media change request
    ATA_ER_ABRT = 0x04,   // Command aborted
    ATA_ER_TK0NF = 0x02,  // Track 0 not found
    ATA_ER_AMNF = 0x01    // No address mark
};

enum ata_commands : short {
    ATA_CMD_READ_PIO = 0x20,
    ATA_CMD_READ_PIO_EXT = 0x24,
    ATA_CMD_READ_DMA = 0xC8,
    ATA_CMD_READ_DMA_EXT = 0x25,
    ATA_CMD_WRITE_PIO = 0x30,
    ATA_CMD_WRITE_PIO_EXT = 0x34,
    ATA_CMD_WRITE_DMA = 0xCA,
    ATA_CMD_WRITE_DMA_EXT = 0x35,
    ATA_CMD_CACHE_FLUSH = 0xE7,
    ATA_CMD_CACHE_FLUSH_EXT = 0xEA,
    ATA_CMD_PACKET = 0xA0,
    ATA_CMD_IDENTIFY_PACKET = 0xA1,
    ATA_CMD_IDENTIFY = 0xEC
};

enum ata_ident : short {
    ATA_IDENT_DEVICETYPE = 0,
    ATA_IDENT_CYLINDERS = 2,
    ATA_IDENT_HEADS = 6,
    ATA_IDENT_SECTORS = 12,
    ATA_IDENT_SERIAL = 20,
    ATA_IDENT_MODEL = 54,
    ATA_IDENT_CAPABILITIES = 98,
    ATA_IDENT_FIELDVALID = 106,
    ATA_IDENT_MAX_LBA = 120,
    ATA_IDENT_COMMANDSETS = 164,
    ATA_IDENT_MAX_LBA_EXT = 200
};

enum ata_regs : short {
    ATA_REG_DATA = 0x00,
    ATA_REG_ERROR = 0x01,
    ATA_REG_FEATURES = 0x01,
    ATA_REG_SECCOUNT0 = 0x02,
    ATA_REG_LBA0 = 0x03,
    ATA_REG_LBA1 = 0x04,
    ATA_REG_LBA2 = 0x05,
    ATA_REG_HDDEVSEL = 0x06,
    ATA_REG_COMMAND = 0x07,
    ATA_REG_STATUS = 0x07,
    ATA_REG_SECCOUNT1 = 0x08,
    ATA_REG_LBA3 = 0x09,
    ATA_REG_LBA4 = 0x0A,
    ATA_REG_LBA5 = 0x0B,
    ATA_REG_CONTROL = 0x0C,
    ATA_REG_ALTSTATUS = 0x0C,
    ATA_REG_DEVADDRESS = 0x0D
};

enum ata_interface_type : short {
    IDE_ATA = 0x00,
    IDE_ATAPI = 0x01,
    ATA_MASTER = 0x00,
    ATA_SLAVE = 0x01,
};

enum ata_channels_dirs : short {
    // Channels:
    ATA_PRIMARY = 0x00,
    ATA_SECONDARY = 0x01,
    // Directions:
    ATA_READ = 0x00,
    ATA_WRITE = 0x01
};
}  // namespace ata

namespace atapi {
enum atapi_commands : short {
    ATAPI_CMD_READ = 0xA8,
    ATAPI_CMD_EJECT = 0x1B
};
}

unsigned char ide_buf[2048] = { 0 };
unsigned static char ide_irq_invoked = 0;
unsigned static char atapi_packet[12] = { 0xA8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
unsigned char package[64];

struct ide_channels_registers {
    unsigned short base;   // I/O Base.
    unsigned short ctrl;   // Control Base
    unsigned short bmide;  // Bus Master IDE
    unsigned char nIEN;    // nIEN (No Interrupt)
} ide_channels[2];

struct ide_device {
    unsigned char reserved;       // 0 (Empty) or 1 (This Drive really exists).
    unsigned char channel;        // 0 (Primary Channel) or 1 (Secondary Channel).
    unsigned char drive;          // 0 (Master Drive) or 1 (Slave Drive).
    unsigned short type;          // 0: ATA, 1:ATAPI.
    unsigned short signature;     // Drive Signature
    unsigned short capabilities;  // Features.
    unsigned int command_sets;    // Command Sets Supported.
    unsigned int size;            // Size in Sectors.
    unsigned char model[41];      // Model in string.
} ide_devices[4];

void ide_write(unsigned char channel, unsigned char reg, unsigned char data) {
    if (reg > 0x07 && reg < 0x0C) ide_write(channel, ata::ata_regs::ATA_REG_CONTROL, 0x80 | ide_channels[channel].nIEN);
    if (reg < 0x08)
        firefly::kernel::io::outb(ide_channels[channel].base + reg - 0x00, data);
    else if (reg < 0x0C)
        firefly::kernel::io::outb(ide_channels[channel].base + reg - 0x06, data);
    else if (reg < 0x0E)
        firefly::kernel::io::outb(ide_channels[channel].ctrl + reg - 0x0A, data);
    else if (reg < 0x16)
        firefly::kernel::io::outb(ide_channels[channel].bmide + reg - 0x0E, data);
    if (reg > 0x07 && reg < 0x0C) ide_write(channel, ata::ata_regs::ATA_REG_CONTROL, ide_channels[channel].nIEN);
}

unsigned char ide_read(unsigned char channel, unsigned char reg) {
    unsigned char result;
    if (reg > 0x07 && reg < 0x0C) ide_write(channel, ata::ata_regs::ATA_REG_CONTROL, 0x80 | ide_channels[channel].nIEN);
    if (reg < 0x08)
        result = firefly::kernel::io::inb(ide_channels[channel].base + reg - 0x00);
    else if (reg < 0x0C)
        result = firefly::kernel::io::inb(ide_channels[channel].base + reg - 0x06);
    else if (reg < 0x0E)
        result = firefly::kernel::io::inb(ide_channels[channel].ctrl + reg - 0x0A);
    else if (reg < 0x16)
        result = firefly::kernel::io::inb(ide_channels[channel].bmide + reg - 0x0E);
    if (reg > 0x07 && reg < 0x0C) ide_write(channel, ata::ata_regs::ATA_REG_CONTROL, ide_channels[channel].nIEN);
    return result;
}

void ide_read_buffer(unsigned char channel, unsigned char reg, unsigned int *buffer, unsigned int quads) {
    if (reg > 0x07 && reg < 0x0C) ide_write(channel, ata::ata_regs::ATA_REG_CONTROL, 0x80 | ide_channels[channel].nIEN);
    ide_read_buffer0();
    if (reg < 0x08)
        insl(ide_channels[channel].base + reg - 0x00, buffer, quads);
    else if (reg < 0x0C)
        insl(ide_channels[channel].base + reg - 0x06, buffer, quads);
    else if (reg < 0x0E)
        insl(ide_channels[channel].ctrl + reg - 0x0A, buffer, quads);
    else if (reg < 0x16)
        insl(ide_channels[channel].bmide + reg - 0x0E, buffer, quads);
    ide_read_buffer1();
    if (reg > 0x07 && reg < 0x0C)
        ide_write(channel, ata::ata_regs::ATA_REG_CONTROL, ide_channels[channel].nIEN);
}

void insl(unsigned reg, unsigned int *buffer, int quads) {
    int index;
    for (index = 0; index < quads; index++) {
        buffer[index] = firefly::kernel::io::inl(reg);
    }
}

unsigned char ide_polling(unsigned char channel, unsigned int advanced_check) {
    // (I) Delay 400 nanosecond for BSY to be set:
    // -------------------------------------------------
    for (int i = 0; i < 4; i++)
        ide_read(channel, ata::ata_regs::ATA_REG_ALTSTATUS);  // Reading the Alternate Status port wastes 100ns; loop four times.
    // (II) Wait for BSY to be cleared:
    // -------------------------------------------------
    while (ide_read(channel, ata::ata_regs::ATA_REG_STATUS) & ata::ata_status::ATA_SR_BSY)
        ;  // Wait for BSY to be zero.
    if (advanced_check) {
        unsigned char state = ide_read(channel, ata::ata_regs::ATA_REG_STATUS);  // Read Status Register.
        // (III) Check For Errors:
        // -------------------------------------------------
        if (state & ata::ata_status::ATA_SR_ERR)
            return 2;  // Error.
        // (IV) Check If Device fault:
        // -------------------------------------------------
        if (state & ata::ata_status::ATA_SR_DF)
            return 1;  // Device Fault.
        // (V) Check DRQ:
        // -------------------------------------------------
        // BSY = 0; DF = 0; ERR = 0 so we should check for DRQ now.
        if ((state & ata::ata_status::ATA_SR_DRQ) == 0)
            return 3;  // DRQ should be set
    }

    return 0;  // No Error.
}

unsigned char ide_print_error(unsigned int drive, unsigned char err) {
    if (err == 0)
        return err;

    printf("IDE Error:");
    if (err == 1) {
        printf("- Device Fault\n     ");
        err = 19;
    } else if (err == 2) {
        unsigned char st = ide_read(ide_devices[drive].channel, ata::ata_regs::ATA_REG_ERROR);
        if (st & ata::ata_errors::ATA_ER_AMNF) {
            printf("- No Address Mark Found\n     ");
            err = 7;
        }
        if (st & ata::ata_errors::ATA_ER_TK0NF) {
            printf("- No Media or Media Error\n     ");
            err = 3;
        }
        if (st & ata::ata_errors::ATA_ER_ABRT) {
            printf("- Command Aborted\n     ");
            err = 20;
        }
        if (st & ata::ata_errors::ATA_ER_MCR) {
            printf("- No Media or Media Error\n     ");
            err = 3;
        }
        if (st & ata::ata_errors::ATA_ER_IDNF) {
            printf("- ID mark not Found\n     ");
            err = 21;
        }
        if (st & ata::ata_errors::ATA_ER_MC) {
            printf("- No Media or Media Error\n     ");
            err = 3;
        }
        if (st & ata::ata_errors::ATA_ER_UNC) {
            printf("- Uncorrectable Data Error\n     ");
            err = 22;
        }
        if (st & ata::ata_errors::ATA_ER_BBK) {
            printf("- Bad Sectors\n     ");
            err = 13;
        }
    } else if (err == 3) {
        printf("- Reads Nothing\n     ");
        err = 23;
    } else if (err == 4) {
        printf("- Write Protected\n     ");
        err = 8;
    }
    printf("- [%s %s] %s\n",
           (const char *[]){ "Primary", "Secondary" }[ide_devices[drive].channel],  // Use the channel as an index into the array
           (const char *[]){ "Master", "Slave" }[ide_devices[drive].drive],         // Same as above, using the drive
           ide_devices[drive].model);

    return err;
}

void ide_initialize(unsigned int BAR0, unsigned int BAR1, unsigned int BAR2, unsigned int BAR3, unsigned int BAR4) {
    int j, k, count = 0;

    // 1- Detect I/O Ports which interface IDE Controller:
    ide_channels[ata::ata_channels_dirs::ATA_PRIMARY].base = (BAR0 & 0xFFFFFFFC) + 0x1F0 * (!BAR0);
    ide_channels[ata::ata_channels_dirs::ATA_PRIMARY].ctrl = (BAR1 & 0xFFFFFFFC) + 0x3F6 * (!BAR1);
    ide_channels[ata::ata_channels_dirs::ATA_SECONDARY].base = (BAR2 & 0xFFFFFFFC) + 0x170 * (!BAR2);
    ide_channels[ata::ata_channels_dirs::ATA_SECONDARY].ctrl = (BAR3 & 0xFFFFFFFC) + 0x376 * (!BAR3);
    ide_channels[ata::ata_channels_dirs::ATA_PRIMARY].bmide = (BAR4 & 0xFFFFFFFC) + 0;    // Bus Master IDE
    ide_channels[ata::ata_channels_dirs::ATA_SECONDARY].bmide = (BAR4 & 0xFFFFFFFC) + 8;  // Bus Master IDE

    // 2- Disable IRQs:
    ide_write(ata::ata_channels_dirs::ATA_PRIMARY, ata::ata_regs::ATA_REG_CONTROL, 2);
    ide_write(ata::ata_channels_dirs::ATA_SECONDARY, ata::ata_regs::ATA_REG_CONTROL, 2);

    // 3- Detect ATA-ATAPI Devices:
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            unsigned char err = 0, type = ata::ata_interface_type::IDE_ATA, status;
            ide_devices[count].reserved = 0;  // Assuming that no drive here.

            // (I) Select Drive:
            ide_write(i, ata::ata_regs::ATA_REG_HDDEVSEL, 0xA0 | (j << 4));  // Select Drive.
            firefly::kernel::sleep::sleep(1);                                // Wait 1ms for drive select to work.

            // (II) Send ATA Identify Command:
            ide_write(i, ata::ata_regs::ATA_REG_COMMAND, ata::ata_commands::ATA_CMD_IDENTIFY);
            firefly::kernel::sleep::sleep(1);

            // (III) Polling:
            if (ide_read(i, ata::ata_regs::ATA_REG_STATUS) == 0) continue;  // If Status = 0, No Device.

            while (1) {
                status = ide_read(i, ata::ata_regs::ATA_REG_STATUS);
                if ((status & ata::ata_status::ATA_SR_ERR)) {
                    err = 1;
                    break;
                }                                                                                              // If Err, Device is not ATA.
                if (!(status & ata::ata_status::ATA_SR_BSY) && (status & ata::ata_status::ATA_SR_DRQ)) break;  // Everything is right.
            }

            // (IV) Probe for ATAPI Devices:

            if (err != 0) {
                unsigned char cl = ide_read(i, ata::ata_regs::ATA_REG_LBA1);
                unsigned char ch = ide_read(i, ata::ata_regs::ATA_REG_LBA2);

                if (cl == 0x14 && ch == 0xEB)
                    type = ata::ata_interface_type::IDE_ATAPI;
                else if (cl == 0x69 && ch == 0x96)
                    type = ata::ata_interface_type::IDE_ATAPI;
                else
                    continue;  // Unknown Type (may not be a device).

                ide_write(i, ata::ata_regs::ATA_REG_COMMAND, ata::ata_commands::ATA_CMD_IDENTIFY_PACKET);
                firefly::kernel::sleep::sleep(1);
            }

            // (V) Read Identification Space of the Device:
            ide_read_buffer(i, ata::ata_regs::ATA_REG_DATA, (unsigned int *)ide_buf, 128);

            // (VI) Read Device Parameters:
            ide_devices[count].reserved = 1;
            ide_devices[count].type = type;
            ide_devices[count].channel = i;
            ide_devices[count].drive = j;
            ide_devices[count].signature = *((unsigned short *)(ide_buf + ata::ata_ident::ATA_IDENT_DEVICETYPE));
            ide_devices[count].capabilities = *((unsigned short *)(ide_buf + ata::ata_ident::ATA_IDENT_CAPABILITIES));
            ide_devices[count].command_sets = *((unsigned int *)(ide_buf + ata::ata_ident::ATA_IDENT_COMMANDSETS));

            // (VII) Get Size:
            if (ide_devices[count].command_sets & (1 << 26))
                // Device uses 48-Bit Addressing:
                ide_devices[count].size = *((unsigned int *)(ide_buf + ata::ata_ident::ATA_IDENT_MAX_LBA_EXT));
            else
                // Device uses CHS or 28-bit Addressing:
                ide_devices[count].size = *((unsigned int *)(ide_buf + ata::ata_ident::ATA_IDENT_MAX_LBA));

            // (VIII) String indicates model of device (like Western Digital HDD and SONY DVD-RW...):
            for (k = 0; k < 40; k += 2) {
                ide_devices[count].model[k] = ide_buf[ata::ata_ident::ATA_IDENT_MODEL + k + 1];
                ide_devices[count].model[k + 1] = ide_buf[ata::ata_ident::ATA_IDENT_MODEL + k];
            }
            ide_devices[count].model[40] = 0;  // Terminate String.

            count++;
        }
    }
    // 4- Print Summary:
    for (i = 0; i < 4; i++) {
        if (ide_devices[i].Reserved == 1) {
            printf(" Found %s Drive %dGB - %s\n",
                   (const char *[]){ "ATA", "ATAPI" }[ide_devices[i].type], /* Type */
                   ide_devices[i].size / 1024 / 1024 / 2,                   /* Size */
                   ide_devices[i].model);
        }
    }
    return;
}

unsigned char ide_ata_access(unsigned char direction, unsigned char drive, unsigned int lba, unsigned char numsects, unsigned short selector, unsigned int edi) {
    unsigned char  lba_mode /* 0: CHS, 1:LBA28, 2: LBA48 */, dma /* 0: No DMA, 1: DMA */, cmd;
    unsigned char  lba_io[6];
    unsigned int   channel    = ide_devices[drive].channel; // Read the Channel.
    unsigned int   slavebit   = ide_devices[drive].drive; // Read the Drive [Master/Slave]
    unsigned int   bus        = ide_channels[channel].Base; // Bus Base, like 0x1F0 which is also data port.
    unsigned int   words      = 256; // Almost every ATA drive has a sector-size of 512-byte.
    unsigned short cyl, i;
    unsigned char  head, sect, err;

    ide_write(channel, ata::ata_regs::ATA_REG_CONTROL, ide_channels[channel].nIEN = (ide_irq_invoked = 0x0) + 0x02);
    
       // (I) Select one from LBA28, LBA48 or CHS;
    if (lba >= 0x10000000) { // Sure Drive should support LBA in this case, or you are
                            // giving a wrong LBA.
      // LBA48:
        lba_mode  = 2;
        lba_io[0] = (lba & 0x000000FF) >> 0;
        lba_io[1] = (lba & 0x0000FF00) >> 8;
        lba_io[2] = (lba & 0x00FF0000) >> 16;
        lba_io[3] = (lba & 0xFF000000) >> 24;
        lba_io[4] = 0; // LBA28 is integer, so 32-bits are enough to access 2TB.
        lba_io[5] = 0; // LBA28 is integer, so 32-bits are enough to access 2TB.
        head      = 0; // Lower 4-bits of HDDEVSEL are not used here.
    } else if (ide_devices[drive].capabilities & 0x200)  { // Drive supports LBA?
        // LBA28:
        lba_mode  = 1;
        lba_io[0] = (lba & 0x00000FF) >> 0;
        lba_io[1] = (lba & 0x000FF00) >> 8;
        lba_io[2] = (lba & 0x0FF0000) >> 16;
        lba_io[3] = 0; // These Registers are not used here.
        lba_io[4] = 0; // These Registers are not used here.
        lba_io[5] = 0; // These Registers are not used here.
        head      = (lba & 0xF000000) >> 24;
    } else {
        // CHS:
        lba_mode  = 0;
        sect      = (lba % 63) + 1;
        cyl       = (lba + 1  - sect) / (16 * 63);
        lba_io[0] = sect;
        lba_io[1] = (cyl >> 0) & 0xFF;
        lba_io[2] = (cyl >> 8) & 0xFF;
        lba_io[3] = 0;
        lba_io[4] = 0;
        lba_io[5] = 0;
        head      = (lba + 1  - sect) % (16 * 63) / (63); // Head number is written to HDDEVSEL lower 4-bits.
    }

    // (II) See if drive supports DMA or not;
    dma = 0; // We don't support DMA

   // (III) Wait if the drive is busy;
    while (ide_read(channel, ata::ata_regs::ATA_REG_STATUS) & ata::ata_status::ATA_SR_BSY)
      ; // Wait if busy.

       // (IV) Select Drive from the controller;
    if (lba_mode == 0) ide_write(channel, ata::ata_regs::ATA_REG_HDDEVSEL, 0xA0 | (slavebit << 4) | head); // Drive & CHS.
    else ide_write(channel, ata::ata_regs::ATA_REG_HDDEVSEL, 0xE0 | (slavebit << 4) | head); // Drive & LBA

       // (V) Write Parameters;
    if (lba_mode == 2) {
        ide_write(channel, ata::ata_regs::ATA_REG_SECCOUNT1,   0);
        ide_write(channel, ata::ata_regs::ATA_REG_LBA3,   lba_io[3]);
        ide_write(channel, ata::ata_regs::ATA_REG_LBA4,   lba_io[4]);
        ide_write(channel, ata::ata_regs::ATA_REG_LBA5,   lba_io[5]);
    }
    ide_write(channel, ata::ata_regs::ATA_REG_SECCOUNT0, numsects);
    ide_write(channel, ata::ata_regs::ATA_REG_LBA0,      lba_io[0]);
    ide_write(channel, ata::ata_regs::ATA_REG_LBA1,      lba_io[1]);
    ide_write(channel, ata::ata_regs::ATA_REG_LBA2,      lba_io[2]);

    if (lba_mode == 0 && dma == 0 && direction == 0) cmd = ata::ata_commands::ATA_CMD_READ_PIO;
    if (lba_mode == 1 && dma == 0 && direction == 0) cmd = ata::ata_commands::ATA_CMD_READ_PIO;   
    if (lba_mode == 2 && dma == 0 && direction == 0) cmd = ata::ata_commands::ATA_CMD_READ_PIO_EXT;   
    if (lba_mode == 0 && dma == 1 && direction == 0) cmd = ata::ata_commands::ATA_CMD_READ_DMA;
    if (lba_mode == 1 && dma == 1 && direction == 0) cmd = ata::ata_commands::ATA_CMD_READ_DMA;
    if (lba_mode == 2 && dma == 1 && direction == 0) cmd = ata::ata_commands::ATA_CMD_READ_DMA_EXT;
    if (lba_mode == 0 && dma == 0 && direction == 1) cmd = ata::ata_commands::ATA_CMD_WRITE_PIO;
    if (lba_mode == 1 && dma == 0 && direction == 1) cmd = ata::ata_commands::ATA_CMD_WRITE_PIO;
    if (lba_mode == 2 && dma == 0 && direction == 1) cmd = ata::ata_commands::ATA_CMD_WRITE_PIO_EXT;
    if (lba_mode == 0 && dma == 1 && direction == 1) cmd = ata::ata_commands::ATA_CMD_WRITE_DMA;
    if (lba_mode == 1 && dma == 1 && direction == 1) cmd = ata::ata_commands::ATA_CMD_WRITE_DMA;
    if (lba_mode == 2 && dma == 1 && direction == 1) cmd = ata::ata_commands::ATA_CMD_WRITE_DMA_EXT;
    ide_write(channel, ata::ata_regs::ATA_REG_COMMAND, cmd); // Send the Command.

    if (dma) {
        if (direction == 0) {
         // PIO Read.
            for (i = 0; i < numsects; i++) {
                if (err = ide_polling(channel, 1)) return err; // Polling, set error and exit if there is.
                asm("pushw %es");
                asm("mov %%ax, %%es" : : "a"(selector));
                asm("rep insw" : : "c"(words), "d"(bus), "D"(edi)); // Receive Data.
                asm("popw %es");
                edi += (words*2);
            }
        } else {
        // PIO Write.
            for (i = 0; i < numsects; i++) {
                ide_polling(channel, 0); // Polling.
                asm("pushw %ds");
                asm("mov %%ax, %%ds"::"a"(selector));
                asm("rep outsw"::"c"(words), "d"(bus), "S"(edi)); // Send Data
                asm("popw %ds");
                edi += (words*2);
            }
            ide_write(channel, ata::ata_regs::ATA_REG_COMMAND, (char []){ ata::ata_commands:ATA_CMD_CACHE_FLUSH, ata::ata_commands::ATA_CMD_CACHE_FLUSH, ata::ata_commands::ATA_CMD_CACHE_FLUSH_EXT }[lba_mode]);
            ide_polling(channel, 0); // Polling.
        }
    }
 
    return 0; // Easy, isn't it?
}

void ide_read_sectors(unsigned char drive, unsigned char numsects, unsigned int lba, unsigned short es, unsigned int edi) {
 
    // 1: Check if the drive presents:
    // ==================================
    if (drive > 3 || ide_devices[drive].reserved == 0) package[0] = 0x1;      // Drive Not Found!

    // 2: Check if inputs are valid:
    // ==================================
    else if (((lba + numsects) > ide_devices[drive].size) && (ide_devices[drive].type == ata::ata_interface_type::IDE_ATA))
       package[0] = 0x2;                     // Seeking to invalid position.

    // 3: Read in PIO Mode through Polling & IRQs:
    // ============================================
    else {
        unsigned char err;
        if (ide_devices[drive].type == ata::ata_interface_type::IDE_ATA)
           err = ide_ata_access(ata::ata_channels_dirs::ATA_READ, drive, lba, numsects, es, edi);
        else if (ide_devices[drive].type == IDE_ATAPI) printf("[IDE] Warning: ATAPI Support is not implemented yet!\n");

        package[0] = ide_print_error(drive, err);
    }
}
// package[0] is an entry of an array. It contains the Error Code.

//TODO: Write ATAPI Read-Only Support
//TODO:       Ejecting an ATAPI Drive
}  // namespace firefiy::kernel::ide