#include "ssd_function.h"

static int ssd_resize(size_t new_size) {
    if (new_size > NAND_LOGICAL_COUNT * NAND_BYTE_SIZE) return -ENOMEM;

    ssd_size.logic = new_size;

    return 0;
}

static int ssd_expand(size_t new_size) {
    if (new_size > ssd_size.logic) return ssd_resize(new_size);

    return 0;
}

static int ssd_do_read(char* buf, size_t size, off_t offset) {
    /* TODO: call ftl_read function and handle result */
    int logical_block_address_temp = offset / 512;
    int logical_block_address_temp_range = (offset + size - 1) / 512 - (logical_block_address_temp) + 1;
    int result;

    char* buf_temp;

    // out of limit
    if ((offset) >= ssd_size.logic) return 0;
    if (size > ssd_size.logic - offset) size = ssd_size.logic - offset;

    logical_block_address_temp = offset / 512;
    logical_block_address_temp_range = (offset + size - 1) / 512 - (logical_block_address_temp) + 1;

    buf_temp = calloc(logical_block_address_temp_range * 512, sizeof(char));

    for (int i = 0; i < logical_block_address_temp_range; i++) {
        // TODO
    }

    memcpy(buf, buf_temp + offset % 512, size);
    free(buf_temp);

    return size;
}

static int ssd_do_write(const char* buf, size_t size, off_t offset) {
    /* TODO: only basic write case, need to consider other cases */

    int logical_block_address_temp = offset / 512;
    int logical_block_address_range_temp = (offset + size - 1) / 512 - (logical_block_address_temp) + 1;
    int process_size = 0, current_size = 0, remain_size = size;
    int result;

    if (ssd_expand(offset + size) != 0) return -ENOMEM;

    ssd_size.host_write += size;

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