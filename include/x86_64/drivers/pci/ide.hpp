#pragma once

#include <stl/cstdlib/cstdint.h>
#include <x86_64/drivers/ports.hpp>
#include <x86_64/drivers/serial_legacy.hpp>
#include <x86_64/sleep.hpp>

typedef unsigned int uint_t;

namespace firefly::drivers::pci {
    inline void insl(uint8_t reg, uint_t *buffer, int quads){
        int index = 0;
        while(index < quads) buffer[index++] = firefly::kernel::io::inl(reg);
    }

    namespace ide {
        struct IDEChannelRegisters {
            uint16_t base;  // I/O Base.
            uint16_t ctrl;  // Control Base
            uint16_t bmide; // Bus Master IDE
            uint8_t nIEN;  // nIEN (No Interrupt);
        };
        struct ide_device {
            unsigned char  Reserved;    // 0 (Empty) or 1 (This Drive really exists).
            unsigned char  Channel;     // 0 (Primary Channel) or 1 (Secondary Channel).
            unsigned char  Drive;       // 0 (Master Drive) or 1 (Slave Drive).
            unsigned short Type;        // 0: ATA, 1:ATAPI.
            unsigned short Signature;   // Drive Signature
            unsigned short Capabilities;// Features.
            unsigned int   CommandSets; // Command Sets Supported.
            unsigned int   Size;        // Size in Sectors.
            unsigned char  Model[41];   // Model in string.
        };

        extern ide_device ide_devices[4];
        extern IDEChannelRegisters channels[2];
        extern uint8_t buffer[2048];
        extern uint8_t irq_invoked;
        extern uint8_t atapi_packet[12]; 
        extern uint8_t package[16];

        namespace status {
            constexpr uint8_t ATA_SR_BSY = 0x80;   // Busy
            constexpr uint8_t ATA_SR_DRDY = 0x40;  // Drive ready
            constexpr uint8_t ATA_SR_DF = 0x20;    // Drive write fault
            constexpr uint8_t ATA_SR_DSC = 0x10;   // Drive seek complete
            constexpr uint8_t ATA_SR_DRQ = 0x08;   // Data request ready
            constexpr uint8_t ATA_SR_CORR = 0x04;  // Corrected data
            constexpr uint8_t ATA_SR_IDX = 0x02;   // Index
            constexpr uint8_t ATA_SR_ERR = 0x01;   // Error
        }  // namespace status
        namespace error {
            constexpr uint8_t ATA_ER_BBK = 0x80;    // Bad block
            constexpr uint8_t ATA_ER_UNC = 0x40;    // Uncorrectable data
            constexpr uint8_t ATA_ER_MC = 0x20;     // Media changed
            constexpr uint8_t ATA_ER_IDNF = 0x10;   // ID mark not found
            constexpr uint8_t ATA_ER_MCR = 0x08;    // Media change request
            constexpr uint8_t ATA_ER_ABRT = 0x04;   // Command aborted
            constexpr uint8_t ATA_ER_TK0NF = 0x02;  // Track 0 not found
            constexpr uint8_t ATA_ER_AMNF = 0x01;   // No address mark
        }  // namespace error
        namespace commands {
            constexpr uint8_t ATA_CMD_READ_PIO = 0x20;
            constexpr uint8_t ATA_CMD_READ_PIO_EXT = 0x24;
            constexpr uint8_t ATA_CMD_READ_DMA = 0xC8;
            constexpr uint8_t ATA_CMD_READ_DMA_EXT = 0x25;
            constexpr uint8_t ATA_CMD_WRITE_PIO = 0x30;
            constexpr uint8_t ATA_CMD_WRITE_PIO_EXT = 0x34;
            constexpr uint8_t ATA_CMD_WRITE_DMA = 0xCA;
            constexpr uint8_t ATA_CMD_WRITE_DMA_EXT = 0x35;
            constexpr uint8_t ATA_CMD_CACHE_FLUSH = 0xE7;
            constexpr uint8_t ATA_CMD_CACHE_FLUSH_EXT = 0xEA;
            constexpr uint8_t ATA_CMD_PACKET = 0xA0;
            constexpr uint8_t ATA_CMD_IDENTIFY_PACKET = 0xA1;
            constexpr uint8_t ATA_CMD_IDENTIFY = 0xEC;
            constexpr uint8_t ATAPI_CMD_READ = 0xA8;
            constexpr uint8_t ATAPI_CMD_EJECT = 0x1B;
        }  // namespace commands
        namespace id {
            constexpr uint8_t ATA_IDENT_DEVICETYPE = 0;
            constexpr uint8_t ATA_IDENT_CYLINDERS = 2;
            constexpr uint8_t ATA_IDENT_HEADS = 6;
            constexpr uint8_t ATA_IDENT_SECTORS = 12;
            constexpr uint8_t ATA_IDENT_SERIAL = 20;
            constexpr uint8_t ATA_IDENT_MODEL = 54;
            constexpr uint8_t ATA_IDENT_CAPABILITIES = 98;
            constexpr uint8_t ATA_IDENT_FIELDVALID = 106;
            constexpr uint8_t ATA_IDENT_MAX_LBA = 120;
            constexpr uint8_t ATA_IDENT_COMMANDSETS = 164;
            constexpr uint8_t ATA_IDENT_MAX_LBA_EXT = 200;
        }  // namespace id
        namespace registers {
            constexpr uint8_t ATA_REG_DATA = 0x00;
            constexpr uint8_t ATA_REG_ERROR = 0x01;
            constexpr uint8_t ATA_REG_FEATURES = 0x01;
            constexpr uint8_t ATA_REG_SECCOUNT0 = 0x02;
            constexpr uint8_t ATA_REG_LBA0 = 0x03;
            constexpr uint8_t ATA_REG_LBA1 = 0x04;
            constexpr uint8_t ATA_REG_LBA2 = 0x05;
            constexpr uint8_t ATA_REG_HDDEVSEL = 0x06;
            constexpr uint8_t ATA_REG_COMMAND = 0x07;
            constexpr uint8_t ATA_REG_STATUS = 0x07;
            constexpr uint8_t ATA_REG_SECCOUNT1 = 0x08;
            constexpr uint8_t ATA_REG_LBA3 = 0x09;
            constexpr uint8_t ATA_REG_LBA4 = 0x0A;
            constexpr uint8_t ATA_REG_LBA5 = 0x0B;
            constexpr uint8_t ATA_REG_CONTROL = 0x0C;
            constexpr uint8_t ATA_REG_ALTSTATUS = 0x0C;
            constexpr uint8_t ATA_REG_DEVADDRESS = 0x0D;
        }  // namespace registers
        namespace etc {
            constexpr uint8_t IDE_ATA = 0x00;
            constexpr uint8_t IDE_ATAPI = 0x01;
            constexpr uint8_t ATA_MASTER = 0x00;
            constexpr uint8_t ATA_SLAVE = 0x01;

            // Channels:
            constexpr uint8_t ATA_PRIMARY = 0x00;
            constexpr uint8_t ATA_SECONDARY = 0x01;

            // Directions:
            constexpr uint8_t ATA_READ = 0x00;
            constexpr uint8_t ATA_WRITE = 0x01;
        }
        
        //standart IDE init
        void ide_init(uint_t BAR0, uint_t BAR1, uint_t BAR2, uint_t BAR3, uint_t BAR4);
        uint8_t ide_read(uint8_t channel, uint8_t reg);
        void ide_write(uint8_t channel, uint8_t reg, uint8_t data);

        /* WARNING: This code contains a serious bug. The inline assembly trashes ES and
        *           ESP for all of the code the compiler generates between the inline
        *           assembly blocks.
        */
        void ide_read_buffer(uint8_t channel, uint8_t reg, uint_t *buffer, uint_t quads);

        uint8_t ide_polling(uint8_t channel, uint_t check);
        uint8_t ide_error(uint_t drive, uint8_t err);
        uint8_t ide_ata_access(uint8_t direction, uint8_t drive, uint8_t lba, uint8_t sectors, uint16_t selector, uint_t edi);
        void ide_wait_irq();
        void ide_irq();

        //only read mode
        uint8_t ide_atapi_read(uint8_t drive, uint_t lba, uint8_t sectors, uint16_t selector, uint_t edi);

        //ata and atapi
        void ide_read_sectors(uint8_t drive, uint8_t sectors, uint_t lba, uint16_t es, uint_t edi);

        //ata
        void ide_write_sectors(uint8_t drive, uint8_t sectors, uint_t lba, uint16_t es, uint_t edi);

        //atapi
        void ide_atapi_eject(uint8_t drive); //TODO

        //parralel IDE support
        void ide_init();
    }  // namespace ide
}  // namespace firefly::drivers::pci