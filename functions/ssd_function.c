#include "ssd_function.h"

void print_ssd() {
    printf("=========================================================\n\n");

    printf("L2P Table:\n");

    for (int i = 0; i < NAND_LOGICAL_COUNT; i++) {
        printf("Logical NAND %d: \n", i);

        for (int j = 0; j < NAND_PAGE_COUNT; j++) {
            PCA PCA;
            PCA.value = L2P_table[NAND_PAGE_COUNT * i + j];

            printf("\t%d - Block: %d, Page: %d\n", j, PCA.info.block_number, PCA.info.page_number);
        }

        printf("\n");
    }

    printf("\n");

    printf("PCA Statement Table:\n");

    for (int i = 0; i < NAND_PHYSICAL_COUNT; i++) {
        printf("Physical NAND %d: ", i);

        for (int j = 0; j < NAND_PAGE_COUNT; j++) {
            printf("%d ", PCA_statement_table[NAND_PAGE_COUNT * i + j]);
        }

        printf("\n");
    }

    printf("\n=========================================================\n\n");
}

int ssd_resize(size_t new_size) {
    if (new_size > NAND_LOGICAL_COUNT * NAND_BYTE_SIZE) return -ENOMEM;

    ssd_size.logic = new_size;

    return 0;
}

int ssd_expand(size_t new_size) {
    if (new_size > ssd_size.logic) return ssd_resize(new_size);

    return 0;
}

int ssd_read(char* buf, size_t size, off_t offset) {
    /* TODO: call ftl_read function and handle result */
    // [v] Done
    // [v] Test

    // out of limit
    if ((offset) >= ssd_size.logic) return 0;
    if (size > ssd_size.logic - offset) size = ssd_size.logic - offset;

    int logical_block_address = offset / 512;
    int logical_block_address_range = (offset + size - 1) / 512 - (logical_block_address) + 1;

    int result = 1;

    char* buf_temp = calloc(logical_block_address_range * 512, sizeof(char));

    for (int i = 0; i < logical_block_address_range; i++) {
        result = ftl_read(buf_temp + i * 512, logical_block_address + i);

        if (result == 0) return -ENOMEM;
        if (result < 0) return result;
    }

    memcpy(buf, buf_temp + offset % 512, size);
    free(buf_temp);

    #if DEBUG
    print_ssd();
    #endif // DEBUG

    return size;
}

int ssd_write(const char* buf, size_t size, off_t offset) {
    /* TODO: only basic write case, need to consider other cases */
    // [v] Done
    // [v] Test

    int logical_block_address = offset / 512;
    int logical_block_address_range = (offset + size - 1) / 512 - (logical_block_address) + 1;
    int process_size = 0, current_size = 0, remain_size = size;
    int result = 1;

    char* write_buf = calloc(512, sizeof(char));

    if (ssd_expand(offset + size) != 0) return -ENOMEM;

    ssd_size.host_write += size;

    for (int i = 0; i < logical_block_address_range; i++) {
        if (result == 0) return -ENOMEM;
        if (result < 0) return result;

        offset %= 512;

        if (offset > 0 || (remain_size > 0 && remain_size < 512)) {
            size_t fill_size = 512 - offset;
            size_t write_size = fill_size ? min(remain_size, fill_size) : remain_size;

            memset(write_buf, 0, 512);

            result = ftl_read(write_buf, logical_block_address + i);
            memcpy(write_buf + offset, buf + process_size, write_size);

            result = ftl_write(write_buf, logical_block_address + i);

            current_size += write_size;
            remain_size -= write_size;
            process_size += write_size;
            offset += write_size;
            continue;
        }

        if (remain_size >= 512) {
            memset(write_buf, 0, 512);
            memcpy(write_buf, buf + process_size, 512);

            result = ftl_write(write_buf, logical_block_address + i);

            current_size += 512;
            remain_size -= 512;
            process_size += 512;
            offset += 512;
            continue;
        }
    }

    #if DEBUG
    print_ssd();
    #endif // DEBUG

    return size;
}