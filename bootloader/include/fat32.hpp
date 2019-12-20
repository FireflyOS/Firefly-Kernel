#pragma once

/*
 *                          FAT32 file system driver.
 */
namespace fat32 {
    /**
     *                      A CHS address
     */
    struct __attribute__((packed)) chs_address {
        /**
         *              
         */
        unsigned head : 8;
        /**
         *              a
         */
        unsigned sect : 6;
        /**
         *              a
         */
        unsigned cyl : 10;
    };

    static_assert(3 == sizeof(chs_address), "sizeof chs_address is incorrect");

    /**
     *                      A partition entry in the MBR.
     */
    struct __attribute__((packed)) part_entry {
        /**
         *                  Whether the partition is bootable.
         *                  Any other values are invalid.
         */
        enum status : unsigned char {
            INACTIVE = 0x00,
            ACTIVE = 0x80
        } status;
        /**
         *                  CHS address of first sector of partition.
         */
        chs_address start_chs;
        /**
         *                  Partition ID.
         */
        unsigned char type;
        /**
         *                  CHS address of first sector of partition.
         */
        chs_address end_chs;
        /**
         *                  LBA of first sector of partition.
         */
        unsigned long start_lba;
        /**
         *                  Number of sectors in partition.
         */
        unsigned long num_sects;
    };

    struct __attribute__((packed)) mbr {
        /**
         *                  Ignored.
         */
        unsigned char rsv[446];
        /**
         *                  Primary partition table.
         */
        part_entry part_table[4];
        /**
         *                  Should be 0xaa55.
         */
        unsigned short boot_sig;
    };

    static_assert(512 == sizeof(mbr), "sizeof mbr is incorrect");

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
        unsigned short sector_size;
        /**
         *                  Sectors per cluster.
         */
        unsigned char cluster_size;
        /**
         *                  Reserved sectors between this sector and the first FAT.
         */
        unsigned short reserved_sectors;
        /**
         *                  Amount of copies of the FAT.
         */
        unsigned char num_fats;
        /**
         *                  Ignored.
         */
        unsigned short rsv1;
        /**
         *                  Ignored.
         */
        unsigned short rsv2;
        /**
         *                  Should be 0xf8 for hard drive.
         */
        unsigned char media_descriptor;
        /**
         *                  Ignored.
         */
        unsigned short rsv3;
        /**
         *                  .
         */
        unsigned short sectors_per_track;
        /**
         *                  .
         */
        unsigned short num_heads;
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
        unsigned short drive_desc;
        /**
         *                  FAT32 version.
         */
        unsigned short version;
        /**
         *                  Cluster of start of root directory.
         */
        unsigned long root_cluster;
        /**
         *                  Logical sector of FS information sector.
         */
        unsigned short fs_info_sect;
        /**
         *                  Logical sector of FAT32 backup sectors.
         */
        unsigned short backup_sect;
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
        unsigned short boot_sig;
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
     *                      Creation time of a file.
     */
    struct __attribute__((packed)) ctime {
        /**
         *                  Seconds / 2.
         *                  0 - 29
         */
        unsigned second : 5;
        /**
         *                  0 - 59.
         */
        unsigned minute : 6;
        /**
         *                  0 - 23
         */
        unsigned hour : 5;
    };

    static_assert(2 == sizeof(ctime), "sizeof ctime is incorrect");

    /**
     *                      Creation time of a file.
     */
    struct __attribute__((packed)) cdate {
        /**
     *                      1 - 31
     */
        unsigned day : 5;
        /**
     *                      1 - 12
     */
        unsigned month : 4;
        /**
     *                      0 - 127
     *                      Offset from 1980
     */
        unsigned year : 7;
    };

    static_assert(2 == sizeof(cdate), "sizeof cdate is incorrect");

    struct __attribute__((packed)) dir_entry {
        /**
         *                  Space-padded name.
         */
        char name[8];
        /**
         *                  Space-padded extension.
         */
        char ext[3];
        struct __attribute__((packed)) {
            bool readonly : 1;
            bool hidden : 1;
            bool system : 1;
            bool vol_label : 1;
            bool subdir : 1;
            bool archive : 1;
            bool device : 1;
            bool rsv : 1;
        } attr;
        unsigned char rsv0[8];
        unsigned short cluster_high;
        unsigned char rsv1[4];
        unsigned short cluster_low;
        unsigned long size;
    };

    static_assert(32 == sizeof(dir_entry), "sizeof dir_entry is incorrect");

    /**
     *                      Poshorter to VBR saved in vbr.asm.
     */
    extern "C" vbr *const boot_vbr;

}  // namespace fat32
