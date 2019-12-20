#include <drive.hpp>
#include <err.hpp>
#include <fat32.hpp>
#include <stdio.hpp>
#include <string.hpp>

using namespace fat32;

static unsigned long fat_start, clust_start;
#define cluster_size (boot_vbr->cluster_size)

static unsigned char clust_buffer[2048];
static unsigned char fat_buffer[512];

/**
 *                          Searches a directory for a file.
 * @param cluster           The first cluster of the directory.
 * @param name              The name of the file to search for.
 */
static dir_entry search_dir(unsigned long cluster, const char *name) {
    unsigned long read_lba = clust_start + (cluster * cluster_size);
    unsigned long read_fat = 0;

    // read each cluster of the directory
    do {
        // split the cluster into parts if the size is greater than the buffer
        for (int clust_part = 0; clust_part < cluster_size; clust_part += 4) {
            int num_sects = (cluster_size - clust_part) >= 4 ? 4 : (cluster_size - clust_part) % 4;
            if (!drive::read(clust_buffer, 0, read_lba + clust_part, num_sects))
                err("failed to read cluster");

            dir_entry *dir = reinterpret_cast<dir_entry *>(clust_buffer);
            for (int entry_idx = 0; dir[entry_idx].name[0] != '\x00' && entry_idx < num_sects * 16; entry_idx++) {
                dir_entry *file = dir + entry_idx;
                // skip deleted
                if (file->name[0] == '\xe5' || file->name[0] == '\x05')
                    continue;
                if (file->readonly && file->hidden && file->system && file->vol_label) {
                    // vfat long file name entry
                    printf("lfn ");
                    vfat_dir_entry *vfat_file = reinterpret_cast<vfat_dir_entry *>(file);
                    for (int ch = 0; ch < 5; ch++) {
                        putchar((char)(vfat_file->name0[ch] & 0xff));
                    }
                    printf("\r\n");
                    // since we know the only files we're searching for have fewer than 13 characters,
                    // only read one lfn entry then get data from next entry
                } else {
                    // short file name entry
                    printf("sfn ");
                    putsn(file->name, 8);
                    printf("\r\n");
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
        cluster = fat_table[cluster].link;
    } while (cluster < 0xffffff8);

    return {};
}

/**
 *                          Loads the kernel into 1M.
 * @param file              The kernel's directory entry.
 * @return                  The total number of bytes read.
 */
static unsigned long load_kernel(dir_entry file) {
    return 0;
}

/**
 *                          Loads all the modules from /boot/modules after the kernel.
 * @param file              The modules directory entry.
 * @return                  The total number of bytes read.
 */
static unsigned long load_modules(dir_entry file) {
    // iterate over the
    return 0;
}

unsigned long fat32::loadfs() {
    // LBA of first FAT
    fat_start = boot_vbr->hidden_sectors + boot_vbr->reserved_sectors;
    // LBA of first actual cluster.
    // Cluster 2 starts at offset 0 from the end of the FATs.
    clust_start = fat_start + (boot_vbr->num_fats * boot_vbr->sectors_per_fat) - (2 * cluster_size);

    dir_entry boot_dir = search_dir(boot_vbr->root_cluster, "boot");
    if (!boot_dir.name[0])
        err("/boot directory does not exist\r\n");
    dir_entry kernel_file = search_dir((boot_dir.cluster_high << 16) | boot_dir.cluster_low, "kernel.bin");
    if (!kernel_file.name[0])
        err("/boot/kernel.bin does not exist\r\n");
    dir_entry modules_dir = search_dir((boot_dir.cluster_high << 16) | boot_dir.cluster_low, "modules");
    if (!modules_dir.name[0])
        err("/boot/modules directory does not exist\r\n");

    unsigned long kernel_size = load_kernel(kernel_file);
    unsigned long modules_size = load_modules(modules_dir);

    return kernel_size + modules_size;
}