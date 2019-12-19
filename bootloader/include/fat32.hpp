#pragma once

/*
 *                          FAT32 file system driver.
 */
namespace fat32 {
    /**
     *                      Sector 0 of the FAT32 boot record.
     */
    struct __attribute__((packed)) vbr {
        /**
         *                  Ignored.
         */
        unsigned char rsv0[3];
        /**
         *                  Space-padded OEM name.
         */
        unsigned char oem[8];
        /**
         *                  Bytes per sector.
         */
        unsigned int sector_size;
        /**
         *                  Sectors per cluster.
         */
        unsigned char cluster_size;
        /**
         *                  Reserved sectors between this sector and the first FAT.
         */
        unsigned int reserved_sectors;
        /**
         *                  Amount of copies of the FAT.
         */
        unsigned char num_fats;
        /**
         *                  Ignored.
         */
        unsigned int rsv1;
        /**
         *                  Ignored.
         */
        unsigned int rsv2;
        /**
         *                  Should be 0xf8 for hard drive.
         */
        unsigned char media_descriptor;
        /**
         *                  Ignored.
         */
        unsigned int rsv3;
        /**
         *                  .
         */
        unsigned int sectors_per_track;
        /**
         *                  .
         */
        unsigned int num_heads;
        /**
         *                  Sectors before this one.
         */
        unsigned long hidden_sectors;
        /**
         *                  The number of sectors in this partition.
         */
        unsigned long total_sectors;
        /**
         *                  The number of sectors in each FAT.
         */
        unsigned long sectors_per_fat;
        /**
         *                  Drive description/mirrorring flags.
         */
        unsigned int drive_desc;
        /**
         *                  FAT32 version.
         */
        unsigned int version;
        /**
         *                  Cluster of start of root directory.
         */
        unsigned long root_cluster;
        /**
         *                  Logical sector of FS information sector.
         */
        unsigned int fs_info_sect;
        /**
         *                  Logical sector of FAT32 backup sectors.
         */
        unsigned int backup_sect;
        /**
         *                  Ignored.
         */
        unsigned char rsv4[12];
        /**
         *                  The BIOS ID given to this drive.
         */
        unsigned char boot_drive_num;
        /**
         *                  Ignored.
         */
        unsigned char rsv5;
        /**
         *                  Extended boot signature.
         */
        unsigned char ext_boot_sig;
        /**
         *                  .
         */
        unsigned long volume_id;
        /**
         *                  .
         */
        char volume_label[11];
        /**
         *                  File system type.
         */
        char fs_type[8];
        /**
         *                  Ignored.
         */
        unsigned char rsv6[420];
        /**
         *                  Should be 0xaa55.
         */
        unsigned int boot_sig;
    };

    static_assert(512 == sizeof(vbr), "sizeof vbr is incorrect");

    /**
     *                      File system information sector.
     */
    struct __attribute__((packed)) fs_info {
        /**
         *                  Should be 0x41615252.
         */
        unsigned long sig0;
        /**
         *                  Ignored.
         */
        unsigned char rsv0[480];
        /**
         *                  Should be 0x61417272.
         */
        unsigned long sig1;
        /**
         *                  Last known number of free clusters.
         */
        unsigned long free_clusters;
        /**
         *                  Number of last allocated cluster.
         */
        unsigned long last_alloc_cluster;
        /**
         *                  Ignored.
         */
        unsigned char rsv1[12];
        /**
         *                  Should be 0x0000aa55.
         */
        unsigned long sig2;
    };

    static_assert(512 == sizeof(fs_info), "sizeof fs_info is incorrect");

    /**
     *                      Third boot sector.
     */
    struct __attribute__((packed)) boot3 {
        /**
         *                  Ignored.
         */
        unsigned char rsv[510];
        /**
         *                  Should be 0xaa55.
         */
        unsigned short sig;
    };

    static_assert(512 == sizeof(boot3), "sizeof boot3 is incorrect");

    /**
     *                      Pointer to VBR saved in vbr.asm.
     */
    extern "C" vbr *const boot_vbr;

}  // namespace fat32
