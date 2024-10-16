#include <sys/ioctl.h>

#include "ssd_function.h"

#define S_IFDIR 0040000
#define S_IFREG 0100000

enum SSD_FILE_PATH_TYPE {
	SSD_NONE,
	SSD_ROOT,
	SSD_FILE,
};

enum SSD_IOCTL_COMMAND_TYPE {
	SSD_GET_LOGIC_SIZE = _IOR('E', 0, size_t),
	SSD_GET_PHYSIC_SIZE = _IOR('E', 1, size_t),
	SSD_GET_WA = _IOR('E', 2, size_t),
};

static int ssd_file_type(const char*);

static int ssd_operation_ioctl(const char*, unsigned int, void*, struct fuse_file_info*, unsigned int, void*);
static int ssd_operation_getattr(const char*, struct stat*, struct fuse_file_info*);
static int ssd_operation_readdir(const char*, void*, fuse_fill_dir_t, off_t, struct fuse_file_info*, enum fuse_readdir_flags);
static int ssd_operation_truncate(const char*, off_t, struct fuse_file_info*);
static int ssd_operation_open(const char*, struct fuse_file_info*);
static int ssd_operation_read(const char*, char*, size_t, off_t, struct fuse_file_info*);
static int ssd_operation_write(const char*, const char*, size_t, off_t, struct fuse_file_info*);

static const struct fuse_operations ssd_operations = {
    .ioctl = ssd_operation_ioctl,
    .getattr = ssd_operation_getattr,
    .readdir = ssd_operation_readdir,
    .truncate = ssd_operation_truncate,
    .open = ssd_operation_open,
    .read = ssd_operation_read,
    .write = ssd_operation_write,
};