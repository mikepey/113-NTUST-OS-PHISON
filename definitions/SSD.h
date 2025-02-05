#ifndef SSD_H
#define SSD_H

#include <sys/ioctl.h>
#include <stddef.h>

#define SSD_FILE_NAME "ssd_file"

extern struct SSD_SIZE {
    size_t physic;
    size_t logic;
    size_t host_write;
    size_t nand_write;
} ssd_size;

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

#endif // SSD_H