#include <stddef.h>

#define SSD_FILE_NAME "ssd_file"

struct SSD_SIZE {
    size_t physic;
    size_t logic;
    size_t host_write;
    size_t nand_write;
} ssd_size;