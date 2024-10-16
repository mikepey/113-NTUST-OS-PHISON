#include "ftl_function.h"

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
    PCA.value = L2P_table[logical_block_address];

    if (PCA.value == PCA_INVALID) return -EINVAL;

    return nand_read(buf, PCA.value);
}

static int ftl_write(const char* buf, size_t logic_block_address_range, size_t logical_block_address) {
    /* TODO: only basic write case, need to consider other cases */
    PCA PCA;
    PCA.value = get_next_PCA();

    if (nand_write(buf, PCA.value)) {
        L2P_table[logical_block_address] = PCA.value;
        return 512;
    }

    printf(" --> Write fail !!!");

    return -EINVAL;
}