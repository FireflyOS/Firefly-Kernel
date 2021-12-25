#include <x86_64/drivers/pci/ide.hpp>
#include <x86_64/drivers/serial_legacy.hpp>
#include <stl/cstdlib/stdio.h>

#define outb firefly::kernel::io::outb
#define inb firefly::kernel::io::inb
#define inl firefly::kernel::io::inl

#define IDE_SDEBUG

namespace firefly::drivers::pci::ide {
    uint16_t es;
    IDEChannelRegisters channels[2];
    uint8_t buffer[2048];
    const char *ataatapi[2] = {"ATA", "ATAPI"};
    uint8_t irq_invoked = 0;
    uint8_t atapi_packet[12] = {0xA8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    ide_device ide_devices[4];
    uint8_t package[16];

    //why this is not working
    void ide_init(uint_t BAR0, uint_t BAR1, uint_t BAR2, uint_t BAR3, uint_t BAR4) {
        #ifdef IDE_SDEBUG
        firefly::kernel::io::legacy::writeTextSerial(": ide_init()\n");
        #endif
        int j = 0, k = 0, count = 0;

        // 1- Detect I/O Ports which interface IDE Controller:
        channels[etc::ATA_PRIMARY  ].base  = (BAR0 & 0xFFFFFFFC) + 0x1F0 * (!BAR0);
        channels[etc::ATA_PRIMARY  ].ctrl  = (BAR1 & 0xFFFFFFFC) + 0x3F6 * (!BAR1);
        channels[etc::ATA_SECONDARY].base  = (BAR2 & 0xFFFFFFFC) + 0x170 * (!BAR2);
        channels[etc::ATA_SECONDARY].ctrl  = (BAR3 & 0xFFFFFFFC) + 0x376 * (!BAR3);
        channels[etc::ATA_PRIMARY  ].bmide = (BAR4 & 0xFFFFFFFC) + 0; // Bus Master IDE
        channels[etc::ATA_SECONDARY].bmide = (BAR4 & 0xFFFFFFFC) + 8; // Bus Master IDE

        // 2- Disable IRQs:
        ide_write(etc::ATA_PRIMARY  , registers::ATA_REG_CONTROL, 2);
        ide_write(etc::ATA_SECONDARY, registers::ATA_REG_CONTROL, 2);

        // 3- Detect ATA-ATAPI Devices:
        for (k = 0; k < 2; k++) {
            #ifdef IDE_SDEBUG
            firefly::kernel::io::legacy::writeTextSerial(": ide_init l0.0\n");
            #endif
            for (j = 0; j < 2; j++) {
                #ifdef IDE_SDEBUG
                firefly::kernel::io::legacy::writeTextSerial(": ide_init l0.1\n");
                #endif
                unsigned char err = 0, type = etc::IDE_ATA, status;
                ide_devices[count].Reserved = 0; // Assuming that no drive here.
        
                // (I) Select Drive:
                ide_write(k, registers::ATA_REG_HDDEVSEL, 0xA0 | (j << 4)); // Select Drive.
                firefly::kernel::sleep::sleep(256); // Wait for drive select to work.
        
                // (II) Send ATA Identify Command:
                ide_write(k, registers::ATA_REG_COMMAND, commands::ATA_CMD_IDENTIFY);
                firefly::kernel::sleep::sleep(256);
        
                // (III) Polling:
                if (ide_read(k, registers::ATA_REG_STATUS) == 0) continue; // If Status = 0, No Device.
                bool akaks = false;
                while(akaks == false) {
                    #ifdef IDE_SDEBUG
                    firefly::kernel::io::legacy::writeTextSerial(": ide_init l0.2\n");
                    #endif
                    status = ide_read(k, registers::ATA_REG_STATUS);
                    if ((status & status::ATA_SR_ERR)) {
                        #ifdef IDE_SDEBUG
                        firefly::kernel::io::legacy::writeTextSerial(": ide_init ERROR\n");
                        #endif
                        err = 1; 
                        akaks = true;
                    } // If Err, Device is not ATA.
                    else if (!(status & status::ATA_SR_BSY) && (status & status::ATA_SR_DRQ)) {
                        #ifdef IDE_SDEBUG
                        firefly::kernel::io::legacy::writeTextSerial(": ide_init OK\n");
                        #endif
                        akaks = true; // Everything is right.
                    }
                    else {
                        #ifdef IDE_SDEBUG
                        firefly::kernel::io::legacy::writeTextSerial(": ide_read() WARNING: something got wrong\n");
                        #endif
                        akaks = true;
                    }
                }
                akaks = false;
        
                // (IV) Probe for ATAPI Devices:
        
                if (err != 0) {
                    #ifdef IDE_SDEBUG
                    firefly::kernel::io::legacy::writeTextSerial(": ide_init err / id: %d\n", err);
                    #endif
                    unsigned char cl = ide_read(k, registers::ATA_REG_LBA1);
                    unsigned char ch = ide_read(k, registers::ATA_REG_LBA2);
        
                    if (cl == 0x14 && ch ==0xEB) type = etc::IDE_ATAPI;
                    else if (cl == 0x69 && ch == 0x96) type = etc::IDE_ATAPI;
                    else continue; // Unknown Type (may not be a device).
        
                    ide_write(k, registers::ATA_REG_COMMAND, commands::ATA_CMD_IDENTIFY_PACKET);
                    firefly::kernel::sleep::sleep(255);
                }
        
                // (V) Read Identification Space of the Device:
                ide_read_buffer(k, registers::ATA_REG_DATA, (unsigned int *)buffer, 128);
        
                // (VI) Read Device Parameters:
                ide_devices[count].Reserved     = 1;
                ide_devices[count].Type         = type;
                ide_devices[count].Channel      = k;
                ide_devices[count].Drive        = j;
                ide_devices[count].Signature    = *((unsigned short *)(buffer + id::ATA_IDENT_DEVICETYPE));
                ide_devices[count].Capabilities = *((unsigned short *)(buffer + id::ATA_IDENT_CAPABILITIES));
                ide_devices[count].CommandSets  = *((unsigned int *)(buffer + id::ATA_IDENT_COMMANDSETS));
        
                // (VII) Get Size:
                if (ide_devices[count].CommandSets & (1 << 26)) ide_devices[count].Size   = *((unsigned int*)buffer + id::ATA_IDENT_MAX_LBA_EXT);
                else ide_devices[count].Size   = *((unsigned int*)buffer + id::ATA_IDENT_MAX_LBA);
        
                // (VIII) String indicates model of device (like Western Digital HDD and SONY DVD-RW...):
                for(k = 0; k < 40; k += 2) {
                    firefly::kernel::io::legacy::writeTextSerial(": ide_init l0.3\n");
                    ide_devices[count].Model[k] = buffer[id::ATA_IDENT_MODEL + k + 1];
                    ide_devices[count].Model[k + 1] = buffer[id::ATA_IDENT_MODEL + k];
                } 
                ide_devices[count].Model[40] = 0; // Terminate String.
                count++;
            }
        }
        // 4- Print Summary:
        for (k = 0; k < 4; k++) {
            #ifdef IDE_SDEBUG
            firefly::kernel::io::legacy::writeTextSerial(": ide_init l1\n");
            #endif
            if (ide_devices[k].Reserved == 1) {
                printf(" Found %s Drive %dGB - %s\n", ataatapi[ide_devices[k].Type], ide_devices[k].Size / 1024 / 1024 / 2, ide_devices[k].Model);
                firefly::kernel::io::legacy::writeTextSerial(" Found %s Drive %dGB - %s\n", ataatapi[ide_devices[k].Type], ide_devices[k].Size / 1024 / 1024 / 2, ide_devices[k].Model);
            }
        }
            
    }
    uint8_t ide_read(uint8_t channel, uint8_t reg) {
        #ifdef IDE_SDEBUG
        firefly::kernel::io::legacy::writeTextSerial(": ide_read()\n");
        #endif
        uint8_t ret;

        if(reg > 7 && reg < 12) ide_write(channel, registers::ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);
        if(reg < 8) ret = inb(channels[channel].base + reg);
        if(reg < 0x0C) ret = inb(channels[channel].base + reg - 6);
        if(reg < 0x0E) ret = inb(channels[channel].ctrl + reg - 0x0A);
        if(reg < 0x16) ret = inb(channels[channel].bmide + reg - 0x0E);
        if(reg > 7 && reg < 0x0C) ide_write(channel, registers::ATA_REG_CONTROL, channels[channel].nIEN);

        return ret;
    }
    void ide_write(uint8_t channel, uint8_t reg, uint8_t data) {
        #ifdef IDE_SDEBUG
        firefly::kernel::io::legacy::writeTextSerial(": ide_write()\n");
        #endif
        if (reg > 0x07 && reg < 0x0C) ide_write(channel, registers::ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);
        if (reg < 0x08) outb(channels[channel].base  + reg - 0x00, data);
        if (reg < 0x0C) outb(channels[channel].base  + reg - 0x06, data);
        if (reg < 0x0E) outb(channels[channel].ctrl  + reg - 0x0A, data);
        if (reg < 0x16) outb(channels[channel].bmide + reg - 0x0E, data);
        if (reg > 0x07 && reg < 0x0C) ide_write(channel, registers::ATA_REG_CONTROL, channels[channel].nIEN);
    }
    void ide_read_buffer(uint8_t channel, uint8_t reg, uint_t *buffer, uint_t quads) {
        #ifdef IDE_SDEBUG
        firefly::kernel::io::legacy::writeTextSerial(": ide_read_buffer()\n");
        #endif
        if (reg > 0x07 && reg < 0x0C)
        ide_write(channel, registers::ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);
        //push_es();
        #ifdef IDE_SDEBUG
        firefly::kernel::io::legacy::writeTextSerial(": ide_read_buffer() insl part\n");
        #endif
        if (reg < 0x08) insl(channels[channel].base  + reg - 0x00, buffer, quads);
        if (reg < 0x0C) insl(channels[channel].base  + reg - 0x06, buffer, quads);
        if (reg < 0x0E) insl(channels[channel].ctrl  + reg - 0x0A, buffer, quads);
        if (reg < 0x16) insl(channels[channel].bmide + reg - 0x0E, buffer, quads);
        #ifdef IDE_SDEBUG
        firefly::kernel::io::legacy::writeTextSerial(": ide_read_buffer() asm\n");
        #endif
        //pop_es();
        asm volatile("movw %0, %%es;"
            :"=r"(es)
        );
        if (reg > 0x07 && reg < 0x0C) ide_write(channel, registers::ATA_REG_CONTROL, channels[channel].nIEN);
    }
    uint8_t ide_polling(uint8_t channel, uint_t check) {
        // (I) Delay 400 nanosecond for BSY to be set:
        // -------------------------------------------------
        uint8_t i = 0;
        while(i < 4){
            ide_read(channel, registers::ATA_REG_ALTSTATUS);
            i++;
        }
        
        // (II) Wait for BSY to be cleared:
        // -------------------------------------------------
        while (ide_read(channel, registers::ATA_REG_STATUS) & status::ATA_SR_BSY);
        
        if (check == 1) {
            uint8_t state = ide_read(channel, registers::ATA_REG_STATUS); // Read Status Register.
        
            // (III) Check For Errors:
            // -------------------------------------------------
            if (state & status::ATA_SR_ERR) return 2; // Error.
        
            // (IV) Check If Device fault:
            // -------------------------------------------------
            if (state & status::ATA_SR_DF) return 1; // Device Fault.
        
            // (V) Check DRQ:
            // -------------------------------------------------
            // BSY = 0; DF = 0; ERR = 0 so we should check for DRQ now.
            if ((state & status::ATA_SR_DRQ) == 0) return 3; // DRQ should be set
        }
        
        return 0; // No Error.
    }

    //sends error message to serial port
    uint8_t ide_error(uint_t drive, uint8_t err) {
        if (err == 0)
        return err;
 
        firefly::kernel::io::legacy::writeTextSerial("\n\n ------- IDE ERROR -------\n");
        switch(err){
            case 1: {
                firefly::kernel::io::legacy::writeTextSerial("- Device Fault\n");
                err = 19;
                break;
            }
            case 2: {
                unsigned char st = ide_read(ide_devices[drive].Channel, registers::ATA_REG_ERROR);
                if (st & error::ATA_ER_AMNF)   {firefly::kernel::io::legacy::writeTextSerial("- No Address Mark Found\n"); err = 7;}
                if (st & error::ATA_ER_TK0NF)   {firefly::kernel::io::legacy::writeTextSerial("- No Media or Media Error\n"); err = 3;}
                if (st & error::ATA_ER_ABRT)   {firefly::kernel::io::legacy::writeTextSerial("- Command Aborted\n"); err = 20;}
                if (st & error::ATA_ER_MCR)   {firefly::kernel::io::legacy::writeTextSerial("- No Media or Media Error\n"); err = 3;}
                if (st & error::ATA_ER_IDNF)   {firefly::kernel::io::legacy::writeTextSerial("- ID mark not Found\n"); err = 21;}
                if (st & error::ATA_ER_MC)   {firefly::kernel::io::legacy::writeTextSerial("- No Media or Media Error\n"); err = 3;}
                if (st & error::ATA_ER_UNC)   {firefly::kernel::io::legacy::writeTextSerial("- Uncorrectable Data Error\n"); err = 22;}
                if (st & error::ATA_ER_BBK)   {firefly::kernel::io::legacy::writeTextSerial("- Bad Sectors\n"); err = 13;}
                break;
            }
            case 3: {
                firefly::kernel::io::legacy::writeTextSerial("- Reads Nothing\n"); 
                err = 23;
                break;
            }
            case 4: {
                firefly::kernel::io::legacy::writeTextSerial("- Write Protected\n     "); 
                err = 8;
                break;
            }
        }

        firefly::kernel::io::legacy::writeTextSerial("- [%s %s] %s\n", (const char *[]){"Primary", "Secondary"}[ide_devices[drive].Channel], (const char *[]){"Master", "Slave"}[ide_devices[drive].Drive], ide_devices[drive].Model);  
        return err;
    }
    uint8_t ide_ata_access(uint8_t direction, uint8_t drive, uint8_t lba, uint8_t sectors, uint16_t selector, uint_t edi) {
        uint8_t lba_mode, dma, cmd;
        uint8_t lba_io[6];
        uint_t channel = ide_devices[drive].Channel; // Read the Channel.
        uint_t slavebit = ide_devices[drive].Drive; // Read the Drive [Master/Slave]
        uint_t bus = channels[channel].base; // Bus Base, like 0x1F0 which is also data port.
        uint_t words = 256; // Almost every ATA drive has a sector-size of 512-byte.
        uint16_t cyl, i;
        uint8_t head, sect, err = 0;

        ide_write(channel, registers::ATA_REG_CONTROL, channels[channel].nIEN = (irq_invoked = 0) + 2);

        // (I) Select one from LBA28, LBA48 or CHS;
        if (lba >= 0b10000000) { // Sure Drive should support LBA in this case, or you are
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
        } else if (ide_devices[drive].Capabilities & 0x200)  { // Drive supports LBA?
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
        while (ide_read(channel, registers::ATA_REG_STATUS) & status::ATA_SR_BSY);

        // (IV) Select Drive from the controller;
        switch(lba_mode){
            case 0: {
                ide_write(channel, registers::ATA_REG_HDDEVSEL, 0xA0 | (slavebit << 4) | head); // Drive & CHS.
                break;
            }
            // (V) Write Parameters
            case 2: {
                ide_write(channel, registers::ATA_REG_SECCOUNT1, 0);
                ide_write(channel, registers::ATA_REG_LBA3, lba_io[3]);
                ide_write(channel, registers::ATA_REG_LBA4, lba_io[4]);
                ide_write(channel, registers::ATA_REG_LBA5, lba_io[5]);
            }
            default: {
                ide_write(channel, registers::ATA_REG_HDDEVSEL, 0xE0 | (slavebit << 4) | head); // Drive & LBA
                break;
            }
        }
        ide_write(channel, registers::ATA_REG_SECCOUNT0, sectors);
        ide_write(channel, registers::ATA_REG_LBA0, lba_io[0]);
        ide_write(channel, registers::ATA_REG_LBA1, lba_io[1]);
        ide_write(channel, registers::ATA_REG_LBA2, lba_io[2]);

        switch (lba_mode){
            case 0: {
                switch (dma){
                    case 0: {
                        switch (direction) {
                            case 0: {
                                cmd = commands::ATA_CMD_READ_PIO;
                                break;
                            }
                            case 1: {
                                cmd = commands::ATA_CMD_WRITE_PIO;
                                break;
                            }
                        }
                    }
                    case 1: {
                        switch (direction) {
                            case 0: {
                                cmd = commands::ATA_CMD_READ_DMA;
                                break;
                            }
                            case 1: {
                                cmd = commands::ATA_CMD_WRITE_DMA;
                                break;
                            }
                        }
                    }
                }
            }
            case 1: {
                switch (dma){
                    case 0: {
                        switch (direction) {
                            case 0: {
                                cmd = commands::ATA_CMD_READ_PIO;
                                break;
                            }
                            case 1: {
                                cmd = commands::ATA_CMD_WRITE_PIO;
                                break;
                            }
                        }
                    }
                    case 1: {
                        switch (direction) {
                            case 0: {
                                cmd = commands::ATA_CMD_READ_DMA;
                                break;
                            }
                            case 1: {
                                cmd = commands::ATA_CMD_WRITE_DMA;
                                break;
                            }
                        }
                    }
                }
            }
            case 2: {
                switch (dma){
                    case 0: {
                        switch (direction) {
                            case 0: {
                                cmd = commands::ATA_CMD_READ_PIO_EXT;
                                break;
                            }
                            case 1: {
                                cmd = commands::ATA_CMD_WRITE_PIO_EXT;
                                break;
                            }
                        }
                    }
                    case 1: {
                        switch(direction) {
                            case 0: {
                                cmd = commands::ATA_CMD_READ_PIO_EXT;
                                break;
                            }
                            case 1: {
                                cmd = commands::ATA_CMD_WRITE_DMA_EXT;
                                break;
                            }
                        }
                    }
                }
            }
        }  
        ide_write(channel, registers::ATA_REG_COMMAND, cmd); // Send the Command.

        switch (dma){
            case 1: {
                break;
            }
            case 0: {
                switch(direction) {
                    case 0: {
                        for (i = 0; i < sectors; i++) {
                            if (err == ide_polling(channel, 1)) return err; // Polling, set error and exit if there is.
                            //asm volatile("pushw %es");
                            asm volatile("mov %%ax, %%es" : : "a"(selector));
                            asm volatile("rep insw" : : "c"(words), "d"(bus), "D"(edi)); // Receive Data.
                            //asm volatile("popw %es");
                            edi += (words * 2);
                        }
                    }
                    default: {
                        for (i = 0; i < sectors; i++) {
                            ide_polling(channel, 0); // Polling.
                            //asm volatile("pushw %ds");
                            asm volatile("mov %%ax, %%ds"::"a"(selector));
                            asm volatile("rep outsw"::"c"(words), "d"(bus), "S"(edi)); // Send Data
                            //asm volatile("popw %ds");
                            edi += (words * 2);
                        }
                        ide_write(channel, registers::ATA_REG_COMMAND, (char []) {   commands::ATA_CMD_CACHE_FLUSH,
                            commands::ATA_CMD_CACHE_FLUSH,
                            commands::ATA_CMD_CACHE_FLUSH_EXT}[lba_mode]);
                        ide_polling(channel, 0); // Polling.
                    }
                }
            }
        }
        return 0;
    }
    void ide_wait_irq() {
        while (!irq_invoked);

        irq_invoked = 0;
    }
    uint8_t ide_atapi_read(uint8_t drive, uint_t lba, uint8_t sectors, uint16_t selector, uint_t edi) {
        uint_t channel  = ide_devices[drive].Channel;
        uint_t slavebit = ide_devices[drive].Drive;
        uint_t bus      = channels[channel].base;
        uint_t words    = 1024; // Sector Size. ATAPI drives have a sector size of 2048 bytes.
        uint8_t err = 0;
        int i;

        ide_write(channel, registers::ATA_REG_CONTROL, channels[channel].nIEN = irq_invoked = 0);

        atapi_packet[ 0] = commands::ATAPI_CMD_READ;
        atapi_packet[ 1] = 0;
        atapi_packet[ 2] = (lba >> 24) & 0xFF;
        atapi_packet[ 3] = (lba >> 16) & 0xFF;
        atapi_packet[ 4] = (lba >> 8) & 0xFF;
        atapi_packet[ 5] = (lba >> 0) & 0xFF;
        atapi_packet[ 6] = 0;
        atapi_packet[ 7] = 0;
        atapi_packet[ 8] = 0;
        atapi_packet[ 9] = sectors;
        atapi_packet[10] = 0;
        atapi_packet[11] = 0;

        ide_write(channel, registers::ATA_REG_HDDEVSEL, slavebit << 4);

        for(int i = 0; i < 4; i++) ide_read(channel, registers::ATA_REG_ALTSTATUS); // Reading the Alternate Status port wastes 100ns.

        ide_write(channel, registers::ATA_REG_FEATURES, 0);

        ide_write(channel, registers::ATA_REG_LBA1, (words * 2) & 0xFF);   // Lower Byte of Sector Size.
        ide_write(channel, registers::ATA_REG_LBA2, (words * 2) >> 8);   // Upper Byte of Sector Size.

        ide_write(channel, registers::ATA_REG_COMMAND, commands::ATA_CMD_PACKET); // Send the Command.
        if (err == ide_polling(channel, 1)) return err; // Polling and return if error.
        asm volatile("rep outsw" : : "c"(6), "d"(bus), "S"(atapi_packet)); // Send Packet Data

        for (i = 0; i < sectors; i++) {
            ide_wait_irq();                  // Wait for an IRQ.
            if (err == ide_polling(channel, 1))
            return err;      // Polling and return if error.
            //asm volatile("pushw %es");
            asm volatile("mov %%ax, %%es"::"a"(selector));
            asm volatile("rep insw"::"c"(words), "d"(bus), "D"(edi));// Receive Data.
            //asm volatile("popw %es");
            edi += (words * 2);
        }
        
        ide_wait_irq();

        while (ide_read(channel, registers::ATA_REG_STATUS) & (status::ATA_SR_BSY | status::ATA_SR_DRQ));

        return 0;
    }
    void ide_read_sectors(uint8_t drive, uint8_t sectors, uint_t lba, uint16_t es, uint_t edi) {
        uint8_t i;
        // 1: Check if the drive presents:
        // ==================================
        if (drive > 3 || ide_devices[drive].Reserved == 0) package[0] = 1;      // Drive Not Found!
        
        // 2: Check if inputs are valid:
        // ==================================
        else if (((lba + sectors) > ide_devices[drive].Size) && (ide_devices[drive].Type == etc::IDE_ATA)) package[0] = 2; // Seeking to invalid position.
        
        // 3: Read in PIO Mode through Polling & IRQs:
        // ============================================
        else {
            uint8_t err = 0;
            switch (ide_devices[drive].Type){
                case etc::IDE_ATA: {
                    err = ide_ata_access(etc::ATA_READ, drive, lba, sectors, es, edi);
                    break;
                }
                case etc::IDE_ATAPI: {
                    for (i = 0; i < sectors; i++) err = ide_atapi_read(drive, lba + i, 1, es, edi + (i*2048));
                }
            }
            package[0] = ide_error(drive, err);
        }
    }

    void ide_write_sectors(uint8_t drive, uint8_t sectors, uint_t lba, uint16_t es, uint_t edi) {
        // 1: Check if the drive presents:
        // ==================================
        if (drive > 3 || ide_devices[drive].Reserved == 0)
            package[0] = 1;      // Drive Not Found!
        // 2: Check if inputs are valid:
        // ==================================
        else if (((lba + sectors) > ide_devices[drive].Size) && (ide_devices[drive].Type == etc::IDE_ATA))
            package[0] = 2;                     // Seeking to invalid position.
        // 3: Read in PIO Mode through Polling & IRQs:
        // ============================================
        else {
            unsigned char err = 0;
            switch(ide_devices[drive].Type){
                case etc::IDE_ATA: {
                    err = ide_ata_access(etc::ATA_WRITE, drive, lba, sectors, es, edi);
                    break;
                }
                case etc::IDE_ATAPI: {
                    err = 4; // Write-Protected.
                    break;
                }
            }
            package[0] = ide_error(drive, err);
        }
    }

    void ide_irq() { irq_invoked = 1;}
    void ide_init(){ firefly::kernel::io::legacy::writeTextSerial(": ide_init()"); ide_init(0x1F0, 0x3F6, 0x170, 0x376, 0); }
}