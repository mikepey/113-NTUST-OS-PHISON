/*
  FUSE ssd: FUSE ioctl example
  Copyright (C) 2008       SUSE Linux Products GmbH
  Copyright (C) 2008       Tejun Heo <teheo@suse.de>
  This program can be distributed under the terms of the GNU GPLv2.
  See the file COPYING.
*/

#define FUSE_USE_VERSION 35

#include <errno.h>
#include <fuse.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "ssd_fuse_header.h"

static size_t physic_size;
static size_t logic_size;
static size_t host_write_size;
static size_t nand_write_size;

PCA current_PCA;

unsigned int* L2P_address_table;

static int ssd_resize(size_t new_size) {
    if (new_size > NAND_LOGICAL_COUNT * NAND_BYTE_SIZE) return -ENOMEM;

    logic_size = new_size;

    return 0;
}

static int ssd_expand(size_t new_size) {
    if (new_size > logic_size) return ssd_resize(new_size);

    return 0;
}

static int nand_read(char* buf, int read_PCA) {
    FILE* fptr;

    PCA PCA;
    PCA.value = read_PCA;

    char NAND_path[100] = {};
    snprintf(NAND_path, 100, "%s/nand_%d", NAND_LOCATION, PCA.info.block_number);

    if (!(fptr = fopen(NAND_path, "r"))) {
        printf("open file fail at nand read pca = %d\n", read_PCA);
        return -EINVAL;
    }

    fseek(fptr, PCA.info.page_number * 512, SEEK_SET);
    fread(buf, 1, 512, fptr);
    fclose(fptr);

    return 512;
}

static int nand_write(const char* buf, int write_PCA) {
    FILE* fptr;

    PCA PCA;
    PCA.value = write_PCA;

    char NAND_path[100];
    snprintf(NAND_path, 100, "%s/nand_%d", NAND_LOCATION, PCA.info.block_number);

    // write to nand
    if (!(fptr = fopen(NAND_path, "r+"))) {
        printf("open file fail at nand (%s) write pca = %d, return %d\n", NAND_path, write_PCA, -EINVAL);
        return -EINVAL;
    }

    fseek(fptr, PCA.info.page_number * 512, SEEK_SET);
    fwrite(buf, 1, 512, fptr);
    fclose(fptr);

    physic_size++;
    nand_write_size += 512;

    return 512;
}

static int nand_erase(int block_number) {
    FILE* fptr;
    // int found = 0; WTF is this?

    char NAND_path[100];
    snprintf(NAND_path, 100, "%s/nand_%d", NAND_LOCATION, block_number);

    // erase nand
    if (!(fptr = fopen(NAND_path, "w"))) {
        printf("open file fail at nand (%s) erase nand = %d, return %d\n", NAND_path, block_number, -EINVAL);
        return -EINVAL;
    }

    fclose(fptr);
    printf("nand erase %d pass\n", block_number);

    /*
        if (found == 0)
        {
            printf("nand erase not found\n");
            return -EINVAL;
        }
        WTF is this?
    */

    return 1;
}

static unsigned int get_next_PCA() {
    /* TODO: seq A, need to change to seq B */
    // [v] Done
    // [] Test

    if (current_PCA.value == PCA_INVALID) {
        // init
        current_PCA.value = 0;

        return current_PCA.value;
    }

    if (current_PCA.value == PCA_FULL) {
        // ssd is full, no pca can be allocated
        printf("No new PCA\n");

        return PCA_FULL;
    }

    const bool PCA_BLOCK_IS_FULL = current_PCA.info.block_number + 1 >= NAND_PHYSICAL_COUNT;
    const bool PCA_PAGE_IS_FULL = current_PCA.info.page_number + 1 >= NAND_PAGE_COUNT;

    /*
        1. page沒滿 -> 換下一個page
        2. page滿了 -> 換下一個block
        3. block滿了 -> PCA_FULL
    */
    if (PCA_BLOCK_IS_FULL && PCA_PAGE_IS_FULL) {
        current_PCA.value = PCA_FULL;

        printf("No new PCA\n");

        return PCA_FULL;
    }

    if (PCA_PAGE_IS_FULL) {
        current_PCA.info.block_number++;
        current_PCA.info.page_number = 0;
    } else
        current_PCA.info.page_number++;

    printf("PCA = page %d, nand %d\n", current_PCA.info.page_number, current_PCA.info.block_number);

    return current_PCA.value;
}

static int ftl_read(char* buf, size_t logical_block_address) {
    /* TODO: 1. Check L2P to get PCA 2. Send read data into nand_read */
    // [v] Done
    // []  Test

    PCA PCA;
    PCA.value = L2P_address_table[logical_block_address];

    if (PCA.value == PCA_INVALID) return -EINVAL;

    return nand_read(buf, PCA.value);
}

static int ftl_write(const char* buf, size_t logic_block_address_range, size_t logical_block_address) {
    /* TODO: only basic write case, need to consider other cases */
    PCA PCA;
    PCA.value = get_next_PCA();

    if (nand_write(buf, PCA.value)) {
        L2P_address_table[logical_block_address] = PCA.value;
        return 512;
    }

    printf(" --> Write fail !!!");

    return -EINVAL;
}

static int ssd_file_type(const char* file_path) {
    if (strcmp(file_path, "/") == 0) return SSD_ROOT;
    if (strcmp(file_path, "/" SSD_FILE_NAME) == 0) return SSD_FILE;
    return SSD_NONE;
}

static int ssd_getattr(const char* file_path, struct stat* stbuf, struct fuse_file_info* fi) {
    (void)fi;

    stbuf->st_uid = getuid();
    stbuf->st_gid = getgid();
    stbuf->st_atime = stbuf->st_mtime = time(NULL);

    switch (ssd_file_type(file_path)) {
        case SSD_ROOT:
            stbuf->st_mode = S_IFDIR | 0755;
            stbuf->st_nlink = 2;
            break;

        case SSD_FILE:
            stbuf->st_mode = S_IFREG | 0644;
            stbuf->st_nlink = 1;
            stbuf->st_size = logic_size;
            break;

        case SSD_NONE:
            return -ENOENT;
    }

    return 0;
}

static int ssd_open(const char* file_path, struct fuse_file_info* fi) {
    (void)fi;

    if (ssd_file_type(file_path) != SSD_NONE) return 0;

    return -ENOENT;
}

static int ssd_do_read(char* buf, size_t size, off_t offset) {
    /* TODO: call ftl_read function and handle result */
    int logical_block_address_temp = offset / 512;
    int logical_block_address_temp_range = (offset + size - 1) / 512 - (logical_block_address_temp) + 1;
    int result;

    char* buf_temp;

    // out of limit
    if ((offset) >= logic_size) return 0;
    if (size > logic_size - offset) size = logic_size - offset;

    buf_temp = calloc(logical_block_address_temp_range * 512, sizeof(char));

    for (int i = 0; i < logical_block_address_temp_range; i++) {
        // TODO
        result = ftl_read(buf_temp + i * 512, logical_block_address_temp++);
        if (result < 0) {
            free(buf_temp);
            return result;
        } else if (result == 0) {
            memset(buf_temp + i * 512, 0, 512);
        }
    }

    memcpy(buf, buf_temp + offset % 512, size);
    free(buf_temp);

    return size;
}

static int ssd_read(const char* file_path, char* buf, size_t size, off_t offset, struct fuse_file_info* fi) {
    (void)fi;

    if (ssd_file_type(file_path) != SSD_FILE) return -EINVAL;

    return ssd_do_read(buf, size, offset);
}

static int ssd_do_write(const char* buf, size_t size, off_t offset) {
    /* TODO: only basic write case, need to consider other cases */

    int logical_block_address_temp = offset / 512;
    int logical_block_address_range_temp = (offset + size - 1) / 512 - (logical_block_address_temp) + 1;
    int process_size = 0, current_size = 0, remain_size = size;
    int result;

    if (ssd_expand(offset + size) != 0) return -ENOMEM;

    host_write_size += size;

    for (int i = 0; i < logical_block_address_range_temp; i++) {
        /* example only align 512, need to implement other cases */
        if (offset % 512 == 0 && size % 512 == 0) {
            result = ftl_write(buf + process_size, 1, logical_block_address_temp + i);
            // write full return -enomem;
            if (result == 0) return -ENOMEM;
            // error
            else if (result < 0)
                return result;

            current_size += 512;
            remain_size -= 512;
            process_size += 512;
            offset += 512;
        } else {
            printf(" --> Not align 512 !!!");
            return -EINVAL;
        }
    }

    return size;
}

static int ssd_write(const char* file_path, const char* buf, size_t size, off_t offset, struct fuse_file_info* fi) {
    (void)fi;

    if (ssd_file_type(file_path) != SSD_FILE) return -EINVAL;

    return ssd_do_write(buf, size, offset);
}

static int ssd_truncate(const char* file_path, off_t size, struct fuse_file_info* fi) {
    (void)fi;

    if (ssd_file_type(file_path) != SSD_FILE) return -EINVAL;

    return ssd_resize(size);
}

static int ssd_readdir(const char* file_path, void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi, enum fuse_readdir_flags flags) {
    (void)fi;
    (void)offset;
    (void)flags;

    if (ssd_file_type(file_path) != SSD_ROOT) return -ENOENT;

    filler(buf, ".", NULL, 0, 0);
    filler(buf, "..", NULL, 0, 0);
    filler(buf, SSD_FILE_NAME, NULL, 0, 0);

    return 0;
}

static int ssd_ioctl(const char* file_path, unsigned int cmd, void* arg, struct fuse_file_info* fi, unsigned int flags, void* data) {
    if (ssd_file_type(file_path) != SSD_FILE) return -EINVAL;
    if (flags & FUSE_IOCTL_COMPAT) return -ENOSYS;

    switch (cmd) {
        case SSD_GET_LOGIC_SIZE:
            *(size_t*)data = logic_size;
            printf(" --> logic size: %ld\n", logic_size);
            return 0;

        case SSD_GET_PHYSIC_SIZE:
            *(size_t*)data = physic_size;
            printf(" --> physic size: %ld\n", physic_size);
            return 0;

        case SSD_GET_WA:
            *(double*)data = (double)nand_write_size / (double)host_write_size;
            return 0;
    }

    return -EINVAL;
}

static const struct fuse_operations ssd_operation = {
    .getattr = ssd_getattr,
    .readdir = ssd_readdir,
    .truncate = ssd_truncate,
    .open = ssd_open,
    .read = ssd_read,
    .write = ssd_write,
    .ioctl = ssd_ioctl,
};

int main(int argc, char* argv[]) {
    char NAND_path[100];

    physic_size = 0;
    logic_size = 0;
    nand_write_size = 0;
    host_write_size = 0;

    current_PCA.value = PCA_INVALID;

    L2P_address_table = malloc(NAND_LOGICAL_COUNT * NAND_PAGE_COUNT * sizeof(int));
    memset(L2P_address_table, PCA_INVALID, NAND_LOGICAL_COUNT * NAND_PAGE_COUNT * sizeof(int));

    for (int i = 0; i < NAND_PHYSICAL_COUNT; i++) {
        snprintf(NAND_path, 100, "%s/nand_%d", NAND_LOCATION, i);

        FILE* fptr = fopen(NAND_path, "w");
        if (fptr == NULL) printf("open fail");

        fclose(fptr);
    }

    return fuse_main(argc, argv, &ssd_operation, NULL);
}