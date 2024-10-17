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

    printf("reading... size: %d, offset: %d\n", size, offset);

    // out of limit
    if ((offset) >= ssd_size.logic) return 0;
    if (size > ssd_size.logic - offset) size = ssd_size.logic - offset;

    int logical_block_address = offset / 512;
    int logical_block_address_range = (offset + size - 1) / 512 - (logical_block_address) + 1;

    printf("reading... logical_block_address: %d, logical_block_address_range: %d\n", logical_block_address, logical_block_address_range);

    int result = 1;

    char* buf_temp = calloc(logical_block_address_range * 512, sizeof(char));
    
    printf("reloading... logic: %d, offset: %d\n", ssd_size.logic, offset);
    printf("reloading... size: %d, offset: %d\n", size, offset);

    for (int i = 0; i < logical_block_address_range; i++) {
        printf("reading... i: %d, lba: %d\n", i, logical_block_address);

        result = ftl_read(buf_temp + i * 512, logical_block_address + i);

        if (result == 0) return -ENOMEM;
        if (result < 0) return result;
    }


    printf("end read size: %d\n", size);

    memcpy(buf, buf_temp + offset % 512, size);
    free(buf_temp);

    print_ssd();

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

    printf("ssd_write in, offset: %d, size: %d\n", offset, size);

    char* write_buf = calloc(512, sizeof(char));

    if (ssd_expand(offset + size) != 0) return -ENOMEM;

    printf("ssd_write expand done\n");

    ssd_size.host_write += size;

    for (int i = 0; i < logical_block_address_range; i++) {
        printf("for 1\n");

        if (result == 0) return -ENOMEM;
        if (result < 0) return result;

        printf("for 2: current_size: %d, remain_size: %d, process_size: %d, offset: %d\n", current_size, remain_size, process_size, offset);

        offset %= 512;

        if (offset > 0 || (remain_size > 0 && remain_size < 512)) {
            size_t fill_size = 512 - offset;
            size_t write_size = fill_size ? min(remain_size, fill_size) : remain_size;

            printf("write size: %d, remain_size: %d, fill_size: %d, offset: %d\n", write_size, remain_size, fill_size, offset);
            printf("memset\n");
            memset(write_buf, 0, 512);

            printf("memcpy, write_size: %d\n", write_size);
            result = ftl_read(write_buf, logical_block_address + i);
            memcpy(write_buf + offset, buf + process_size, write_size);

            printf("writing %s, write buf: %s\n", buf + process_size, write_buf);

            printf("ftl_write in, LBA: %d\n", logical_block_address + i);
            result = ftl_write(write_buf, logical_block_address + i);

            current_size += write_size;
            remain_size -= write_size;
            process_size += write_size;
            offset += write_size;
            continue;
        }

        printf("for 3: current_size: %d, remain_size: %d, process_size: %d, offset: %d\n", current_size, remain_size, process_size, offset);

        printf("for 3-1\n");
        if (remain_size >= 512) {
            printf("big remain size\n");
            printf("logical_block_address: %d, i: %d, process_size: %d\n", logical_block_address, i, process_size);
            
            memset(write_buf, 0, 512);
            memcpy(write_buf, buf + process_size, 512);

            result = ftl_write(write_buf, logical_block_address + i);

            current_size += 512;
            remain_size -= 512;
            process_size += 512;
            offset += 512;
            continue;
        }
        printf("for 4-1\n");
    }

    printf("for done\n");
    print_ssd();

    return size;
}