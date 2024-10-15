/*
	FUSE-ioctl: ioctl support for FUSE
	Copyright (C) 2008       SUSE Linux Products GmbH
	Copyright (C) 2008       Tejun Heo <teheo@suse.de>
	This program can be distributed under the terms of the GNU GPLv2.
	See the file COPYING.
*/

#include <sys/types.h>
#include <sys/uio.h>
#include <sys/ioctl.h>

#define SSD_FILE_NAME "ssd_file"
#define NAND_LOCATION  "/home/mikepey/OS/SSD"

#define NAND_SIZE (10) // 10 KB (10240 Bytes)
#define NAND_LOGICAL_COUNT (5)
#define NAND_PHYSICAL_COUNT (8)

/* 擴展宏 */
#define NAND_PAGE_COUNT (NAND_SIZE * 1024 / 512)

enum PCA_STATEMENT {
    PCA_FULL = 0xFFFFFFFE, // 4294967294 (unsigned int的最大值 - 1)
    PCA_INVALID = 0xFFFFFFFF, // 4294967295 (unsigned int的最大值)
};

enum SSD_IOCTL_COMMAND_TYPE {
	SSD_GET_LOGIC_SIZE = _IOR('E', 0, size_t),
	SSD_GET_PHYSIC_SIZE = _IOR('E', 1, size_t),
	SSD_GET_WA = _IOR('E', 2, size_t),
};

enum SSD_FILE_PATH_TYPE {
	SSD_NONE,
	SSD_ROOT,
	SSD_FILE,
};

typedef union PCA {
    unsigned int value;

    struct {
        unsigned int page_count : 16;
        unsigned int block_count : 16;
    } info;
} PCA;