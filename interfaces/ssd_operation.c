#include "ssd_operation.h"

const struct fuse_operations ssd_operations = {
    .ioctl = ssd_operation_ioctl,
    .getattr = ssd_operation_getattr,
    .readdir = ssd_operation_readdir,
    .truncate = ssd_operation_truncate,
    .open = ssd_operation_open,
    .read = ssd_operation_read,
    .write = ssd_operation_write,
};

int ssd_file_type(const char* ssd_file_path) {
    if (strcmp(ssd_file_path, "/") == 0) return SSD_ROOT;
    if (strcmp(ssd_file_path, "/" SSD_FILE_NAME) == 0) return SSD_FILE;
    return SSD_NONE;
}

int ssd_operation_ioctl(const char* ssd_file_path, unsigned int command, void* arg, struct fuse_file_info* fuse_file_info, unsigned int flags, void* data) {
    if (ssd_file_type(ssd_file_path) != SSD_FILE) return -EINVAL;
    if (flags & FUSE_IOCTL_COMPAT) return -ENOSYS;

    switch (command) {
        case SSD_GET_LOGIC_SIZE:
            *(size_t*)data = ssd_size.logic;
            printf(" --> logic size: %ld\n", ssd_size.logic);
            return 0;

        case SSD_GET_PHYSIC_SIZE:
            *(size_t*)data = ssd_size.physic;
            printf(" --> physic size: %ld\n", ssd_size.physic);
            return 0;

        case SSD_GET_WA:
            *(double*)data = (double)ssd_size.nand_write / (double)ssd_size.host_write;
            return 0;
    }

    return -EINVAL;
}

int ssd_operation_getattr(const char* ssd_file_path, struct stat* stbuf, struct fuse_file_info* fuse_file_info) {
    (void)fuse_file_info;

    stbuf->st_uid = getuid();
    stbuf->st_gid = getgid();
    stbuf->st_atime = stbuf->st_mtime = time(NULL);

    switch (ssd_file_type(ssd_file_path)) {
        case SSD_ROOT:
            stbuf->st_mode = S_IFDIR | 0755;
            stbuf->st_nlink = 2;
            break;

        case SSD_FILE:
            stbuf->st_mode = S_IFREG | 0644;
            stbuf->st_nlink = 1;
            stbuf->st_size = ssd_size.logic;
            break;

        case SSD_NONE:
            return -ENOENT;
    }

    return 0;
}

int ssd_operation_readdir(const char* ssd_file_path, void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fuse_file_info, enum fuse_readdir_flags flags) {
    (void)fuse_file_info;
    (void)offset;
    (void)flags;

    if (ssd_file_type(ssd_file_path) != SSD_ROOT) return -ENOENT;

    filler(buf, ".", NULL, 0, 0);
    filler(buf, "..", NULL, 0, 0);
    filler(buf, SSD_FILE_NAME, NULL, 0, 0);

    return 0;
}

int ssd_operation_truncate(const char* ssd_file_path, off_t size, struct fuse_file_info* fuse_file_info) {
    (void)fuse_file_info;

    if (ssd_file_type(ssd_file_path) != SSD_FILE) return -EINVAL;

    return ssd_resize(size);
}

int ssd_operation_open(const char* ssd_file_path, struct fuse_file_info* fuse_file_info) {
    (void)fuse_file_info;

    if (ssd_file_type(ssd_file_path) != SSD_NONE) return 0;

    return -ENOENT;
}

int ssd_operation_read(const char* ssd_file_path, char* buf, size_t size, off_t offset, struct fuse_file_info* fuse_file_info) {
    (void)fuse_file_info;

    if (ssd_file_type(ssd_file_path) != SSD_FILE) return -EINVAL;

    printf("ssd_operation_read size: %d, offset: %d\n", size, offset);

    return ssd_read(buf, size, offset);
}

int ssd_operation_write(const char* ssd_file_path, const char* buf, size_t size, off_t offset, struct fuse_file_info* fuse_file_info) {
    (void)fuse_file_info;

    if (ssd_file_type(ssd_file_path) != SSD_FILE) return -EINVAL;

    return ssd_write(buf, size, offset);
}