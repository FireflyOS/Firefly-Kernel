#include <copy_unreal.hpp>
#include <drive.hpp>
#include <err.hpp>
#include <fat32.hpp>
#include <stdio.hpp>
#include <string.hpp>

using namespace fat32;

static unsigned long fat_start, cluster_start;
#define cluster_size (boot_vbr->cluster_size)

unsigned char cluster_buffer[4096];
static unsigned char fat_buffer[512];

/**
 *                          Searches a directory for a file.
 * @param  cluster          The first cluster of the directory.
 * @param  name             The name of the file to search for.
 */
static dir_entry search_dir(unsigned long cluster, const char *name) {
    unsigned long read_lba = cluster_start + (cluster * cluster_size);
    unsigned long read_fat = 0;
    int namelen = strlen(name);

    // read each cluster of the directory
    do {
        // split the cluster into parts if the size is greater than the buffer
        for (int clust_part = 0; clust_part < cluster_size; clust_part += 8) {
            int num_sects = (cluster_size - clust_part) >= 8 ? 8 : (cluster_size - clust_part) % 8;
            if (!drive::read(cluster_buffer, 0, read_lba + clust_part, num_sects))
                err("failed to read cluster");

            // read buffer as directory
            dir_entry *dir = reinterpret_cast<dir_entry *>(cluster_buffer);
            for (int entry_idx = 0; dir[entry_idx].name[0] != '\x00' && entry_idx < num_sects * 16; entry_idx++) {
                dir_entry *file = dir + entry_idx;
                // skip deleted
                if (file->name[0] == '\xe5' || file->name[0] == '\x05')
                    continue;
                if (file->readonly && file->hidden && file->system && file->vol_label) {
                    // vfat long file name entry
                    vfat_dir_entry *vfat_file = reinterpret_cast<vfat_dir_entry *>(file);
                    // idx into name
                    int ch = 0, i_n0 = 0, i_n1 = 0, i_n2 = 0;
                    // compare chars 0-4
                    for (; i_n0 < 5 && (vfat_file->name0[i_n0] & 0xff) == name[ch] && ch < namelen; i_n0++, ch++)
                        ;
                    if (ch < namelen && i_n0 != 5) {
                        // skip to next entry
                        for (; !vfat_file->last_part; vfat_file++, entry_idx++) {
                            if (entry_idx == (num_sects * 16) - 1) {
                                clust_part += 8;
                                drive::read(cluster_buffer, 0, read_lba + clust_part, 8);
                                entry_idx = -1;
                                vfat_dir_entry *vfat_file = reinterpret_cast<vfat_dir_entry *>(dir - 1);
                            }
                        }
                        entry_idx++;  // also skip data entry
                        continue;
                    }

                    // then chars 5-10
                    for (; i_n1 < 6 && (vfat_file->name1[i_n1] & 0xff) == name[ch] && ch < namelen; i_n1++, ch++)
                        ;
                    if (ch < namelen && i_n0 != 5) {
                        // skip to next entry
                        for (; !vfat_file->last_part; vfat_file++, entry_idx++) {
                            if (entry_idx == (num_sects * 16) - 1) {
                                clust_part += 8;
                                drive::read(cluster_buffer, 0, read_lba + clust_part, 8);
                                entry_idx = -1;
                                vfat_dir_entry *vfat_file = reinterpret_cast<vfat_dir_entry *>(dir - 1);
                            }
                        }
                        entry_idx++;  // also skip data entry
                        continue;
                    }
                    // then chars 11-12
                    for (; i_n2 < 2 && (vfat_file->name2[i_n2] & 0xff) == name[ch] && ch < namelen; i_n2++, ch++)
                        ;
                    if (ch < namelen && i_n0 != 5) {
                        // skip to next entry
                        for (; !vfat_file->last_part; vfat_file++, entry_idx++) {
                            if (entry_idx == (num_sects * 16) - 1) {
                                clust_part += 8;
                                drive::read(cluster_buffer, 0, read_lba + clust_part, 8);
                                entry_idx = -1;
                                vfat_dir_entry *vfat_file = reinterpret_cast<vfat_dir_entry *>(dir - 1);
                            }
                        }
                        entry_idx++;  // also skip data entry
                        continue;
                    }

                    if (ch == namelen && vfat_file->last_part)
                        if (entry_idx == (num_sects * 16) - 1) {
                            // last entry in buffer
                            drive::read(cluster_buffer, 0, read_lba + clust_part + 8, 1);
                            return *dir;
                        } else
                            return file[1];
                    printf("\r\n");
                    // since we know the only files we're searching for have fewer than 13 characters,
                    // only read one lfn entry then get data from next entry
                } else {
                }
            }
        }

        // read new fat sector if next cluster isn't in the already loaded one
        if (read_fat != fat_start + (cluster / 128)) {
            read_fat = fat_start + (cluster / 128);
            if (!drive::read(fat_buffer, 0, read_fat, 1))
                err("failed to read cluster");
        }

        fat_entry *fat_table = reinterpret_cast<fat_entry *>(fat_buffer);
        cluster = fat_table[cluster % 128].link;
    } while (cluster < 0xffffff8);

    return {};
}

/**
 *                          Reads a file into memory.
 * @param  file             The file to read.
 * @param  dest             Address to read the file into.
 * @return                  The total number of bytes read, aligned to 4KiB..
 */
static unsigned long load_file(dir_entry file, unsigned long dest) {
    unsigned long dest_start = dest;
    unsigned long cluster = (file.cluster_high << 16) | file.cluster_low;
    unsigned long read_lba = cluster_start + (cluster * cluster_size);
    unsigned long read_fat = 0;

    // read each cluster of the file
    do {
        // split the cluster into parts if the size is greater than the buffer
        for (int clust_part = 0; clust_part < cluster_size; clust_part += 8) {
            int num_sects = (cluster_size - clust_part) >= 8 ? 8 : (cluster_size - clust_part) % 8;
            if (!drive::read(cluster_buffer, 0, read_lba + clust_part, num_sects))
                err("failed to read cluster");

            copy_unreal(static_cast<unsigned short>(reinterpret_cast<unsigned int>(cluster_buffer)), dest, num_sects * 512);
            dest += num_sects * 512;
        }

        if (read_fat != fat_start + (cluster / 128)) {
            read_fat = fat_start + (cluster / 128);
            if (!drive::read(fat_buffer, 0, read_fat, 1))
                err("failed to read cluster");
        }

        fat_entry *fat_table = reinterpret_cast<fat_entry *>(fat_buffer);
        cluster = fat_table[cluster % 128].link;
    } while (cluster < 0xffffff8);

    return dest - dest_start;
}

/**
 *                          Loads the kernel into 1M.
 * @param  file             The kernel's directory entry.
 * @return                  The total number of bytes read.
 */
static unsigned long load_kernel(dir_entry file) {
    if (!load_file(file, 0x00100000ul))
        err("failed to load kernel");
    else
        return file.size;
}

/**
 *                          Loads all the modules from /boot/modules after the kernel.
 * @param  file             The modules directory entry.
 * @param  start            Where to begin loading the modules.
 * @return                  The total number of bytes read.
 */
static unsigned long load_modules(dir_entry file, unsigned long dest) {
    unsigned long cluster = (file.cluster_high << 16) | file.cluster_low;
    unsigned long read_lba = cluster_start + (cluster * cluster_size);
    unsigned long read_fat = 0;
    unsigned long desc_dest = 0x00030000;

    // read each cluster of the directory
    do {
        // split the cluster into parts if the size is greater than the buffer
        for (int clust_part = 0; clust_part < cluster_size; clust_part += 8) {
            int num_sects = (cluster_size - clust_part) >= 8 ? 8 : (cluster_size - clust_part) % 8;
            if (!drive::read(cluster_buffer, 0, read_lba + clust_part, num_sects))
                err("failed to read cluster");

            // read buffer as directory
            dir_entry *dir = reinterpret_cast<dir_entry *>(cluster_buffer);
            for (int entry_idx = 0; dir[entry_idx].name[0] != '\x00' && entry_idx < num_sects * 16; entry_idx++) {
                dir_entry *file = dir + entry_idx;
                // skip deleted
                if (file->name[0] == '\xe5' || file->name[0] == '\x05')
                    continue;
                if (file->readonly && file->hidden && file->system && file->vol_label) {
                    vfat_dir_entry *vfat_file = reinterpret_cast<vfat_dir_entry *>(file);
                    // just skip to the data
                    for (; !vfat_file->last_part; vfat_file++, entry_idx++) {
                        if (entry_idx == (num_sects * 16) - 1) {
                            clust_part += 8;
                            drive::read(cluster_buffer, 0, read_lba + clust_part, 8);
                            entry_idx = -1;
                            vfat_dir_entry *vfat_file = reinterpret_cast<vfat_dir_entry *>(dir - 1);
                        }
                    }

                    unsigned long fsize = load_file(dir[++entry_idx], dest);

                    module_description desc = { dest, fsize };
                    copy_unreal(static_cast<unsigned short>(reinterpret_cast<unsigned int>(&desc)), desc_dest, 16);
                    desc_dest += 16;

                    dest += fsize;
                } else if (file->name[0] != '\x2e') {
                    unsigned long fsize = load_file(*file, dest);

                    module_description desc = { dest, fsize };
                    copy_unreal(static_cast<unsigned short>(reinterpret_cast<unsigned int>(&desc)), desc_dest, 16);
                    desc_dest += 16;

                    dest += fsize;
                }
            }
        }

        // read new fat sector if next cluster isn't in the already loaded one
        if (read_fat != fat_start + (cluster / 128)) {
            read_fat = fat_start + (cluster / 128);
            if (!drive::read(fat_buffer, 0, read_fat, 1))
                err("failed to read cluster");
        }

        fat_entry *fat_table = reinterpret_cast<fat_entry *>(fat_buffer);
        cluster = fat_table[cluster % 128].link;
    } while (cluster < 0xffffff8);

    // end list with empty description
    module_description desc = {};
    copy_unreal(static_cast<unsigned short>(reinterpret_cast<unsigned int>(&desc)), desc_dest, 16);

    return dest - 0x00100000;
}

unsigned long fat32::loadfs() {
    // LBA of first FAT
    fat_start = boot_vbr->hidden_sectors + boot_vbr->reserved_sectors;
    // LBA of first actual cluster.
    // Cluster 2 starts at offset 0 from the end of the FATs.
    cluster_start = fat_start + (boot_vbr->num_fats * boot_vbr->sectors_per_fat) - (2 * cluster_size);

    dir_entry boot_dir = search_dir(boot_vbr->root_cluster, "boot");
    if (!boot_dir.name[0])
        err("/boot directory does not exist\r\n");
    dir_entry kernel_file = search_dir((boot_dir.cluster_high << 16) | boot_dir.cluster_low, "kernel.bin");
    if (!kernel_file.name[0])
        err("/boot/kernel.bin does not exist\r\n");
    dir_entry modules_dir = search_dir((boot_dir.cluster_high << 16) | boot_dir.cluster_low, "modules");
    if (!modules_dir.name[0])
        err("/boot/modules directory does not exist\r\n");

    return load_modules(modules_dir, 0x00100000 + load_file(kernel_file, 0x00100000));
}