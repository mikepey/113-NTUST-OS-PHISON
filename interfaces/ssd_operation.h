#ifndef SSD_OPERATION_H
#define SSD_OPERATION_H

#include "../functions/ssd_function.h"

#ifndef S_IFDIR
#define S_IFDIR 0040000
#endif // S_IFDIR

#ifndef S_IFREG
#define S_IFREG 0100000
#endif // S_IFREG

extern const struct fuse_operations ssd_operations;

int ssd_file_type(const char*);
int ssd_operation_ioctl(const char*, unsigned int, void*, struct fuse_file_info*, unsigned int, void*);
int ssd_operation_getattr(const char*, struct stat*, struct fuse_file_info*);
int ssd_operation_readdir(const char*, void*, fuse_fill_dir_t, off_t, struct fuse_file_info*, enum fuse_readdir_flags);
int ssd_operation_truncate(const char*, off_t, struct fuse_file_info*);
int ssd_operation_open(const char*, struct fuse_file_info*);
int ssd_operation_read(const char*, char*, size_t, off_t, struct fuse_file_info*);
int ssd_operation_write(const char*, const char*, size_t, off_t, struct fuse_file_info*);

#endif // SSD_OPERATION_H