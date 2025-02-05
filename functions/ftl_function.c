#include "ftl_function.h"

unsigned int* L2P_table;
PCA_STATEMENT* PCA_statement_table;

PCA_STATEMENT get_PCA_statement(PCA* PCA) {
    return PCA_statement_table[20 * PCA->info.block_number + PCA->info.page_number];
}

unsigned int get_next_PCA() {
    /* TODO: seq A, need to change to seq B */
    // [v] Done
    // [v] Test

    while (get_PCA_statement(&current_PCA) != PCA_EMPTY) {
        const bool PCA_BLOCK_IS_FULL = current_PCA.info.block_number + 1 >= NAND_PHYSICAL_COUNT;
        const bool PCA_PAGE_IS_FULL = current_PCA.info.page_number + 1 >= NAND_PAGE_COUNT;

        if (PCA_BLOCK_IS_FULL && PCA_PAGE_IS_FULL) {
            if (ftl_garbage_collection()) return current_PCA.value;
            
            printf("--> GC FAIL!!!");
            print_ssd();

            abort();
        }

        if (PCA_PAGE_IS_FULL) {
            current_PCA.info.block_number++;
            current_PCA.info.page_number = 0;

            ftl_garbage_collection();
            continue;
        }

        current_PCA.info.page_number++;
    }

    return current_PCA.value;
}

unsigned int get_min_valid_PCA(unsigned int* valid_PCA_count) {
    PCA PCA;

    unsigned int block_number_result = 0, min_valid_PCA_count = 512;

    for (int i = 0; i < NAND_PHYSICAL_COUNT; i++) {
        PCA.info.block_number = i;

        unsigned int valid_count = 0;
        for (int j = 0; j < NAND_PAGE_COUNT; j++) {
            PCA.info.page_number = j;

            if (get_PCA_statement(&PCA) == PCA_HAVE_DATA) valid_count++;
        }

        if (valid_count > 0 && min_valid_PCA_count > valid_count) {
            block_number_result = i;
            min_valid_PCA_count = valid_count;
        }
    }

    *valid_PCA_count = min_valid_PCA_count;
    return block_number_result;
}

unsigned int get_max_empty_PCA(unsigned int* empty_PCA_count) {
    PCA PCA;  
    unsigned int block_number_result = 0, max_empty_PCA_count = 0;

    for (int i = 0; i < NAND_PHYSICAL_COUNT; i++) {
        PCA.info.block_number = i;

        unsigned int empty_count = 0;
        for (int j = 0; j < NAND_PAGE_COUNT; j++) {
            PCA.info.page_number = j;

            if (get_PCA_statement(&PCA) == PCA_EMPTY) empty_count++;
        }

        if (empty_count > max_empty_PCA_count) {
            block_number_result = i;
            max_empty_PCA_count = empty_count;
        }
    }

    *empty_PCA_count = max_empty_PCA_count;
    return block_number_result;
}

bool is_disable_block(unsigned int block_number) { 
    PCA PCA;  
    PCA.info.block_number = block_number;

    for (int i = 0; i < NAND_PAGE_COUNT; i++) {
        PCA.info.page_number = i;
        
        if (get_PCA_statement(&PCA) != PCA_DISABLE) return false;
    }

    return true;
}

int ftl_read(char* buf, size_t logical_block_address) {
    /* TODO: 1. Check L2P to get PCA 2. Send read data into nand_read */
    // [v] Done
    // [v]  Test

    PCA PCA;
    PCA.value = L2P_table[logical_block_address];

    if (PCA.value == L2P_EMPTY_PCA) return 512;
    if (get_PCA_statement(&PCA) != PCA_HAVE_DATA) return 512;

    return nand_read(buf, PCA.value);
}

int ftl_write(const char* buf, size_t logical_block_address) {
    /* TODO: only basic write case, need to consider other cases */
    // [v] Done
    // [v] Test

    PCA PCA;
    PCA.value = L2P_table[logical_block_address];

    char* buf_temp = calloc(512, sizeof(char));

    if (PCA.value == L2P_EMPTY_PCA) PCA.value = get_next_PCA();

    size_t last_space = 511, remaining_nand_size = 0;

    if (get_PCA_statement(&PCA) == PCA_HAVE_DATA) {
        nand_read(buf_temp, PCA.value);

        PCA_statement_table[NAND_PAGE_COUNT * PCA.info.block_number + PCA.info.page_number] = PCA_DISABLE;
        PCA.value = get_next_PCA();
    }

    while (buf_temp[last_space] == '\0') {
        last_space--;
        remaining_nand_size++;
    }
    
    for (int i = 0; i < 512; i++) {
        if (buf[i] != '\0') buf_temp[i] = buf[i];
    }

    if ((remaining_nand_size >= strlen(buf) && nand_write(buf_temp, PCA.value)) || (remaining_nand_size < strlen(buf) && nand_write(buf, PCA.value))) {
        L2P_table[logical_block_address] = PCA.value;
        PCA_statement_table[NAND_PAGE_COUNT * PCA.info.block_number + PCA.info.page_number] = PCA_HAVE_DATA;

        return 512;
    }

    return -EINVAL;
}

int ftl_garbage_collection() {
    size_t success_collect_size = 0;

    unsigned int valid_PCA_count, empty_PCA_count;
    unsigned int min_valid_PCA_block_number = get_min_valid_PCA(&valid_PCA_count), max_empty_PCA_block_number = get_max_empty_PCA(&empty_PCA_count);

    for (int i = 0; i < NAND_PHYSICAL_COUNT; i++) {
        if (!is_disable_block(i)) continue;

        ftl_erase(i);

        success_collect_size += 512;
    }
    
    if (min_valid_PCA_block_number != max_empty_PCA_block_number && valid_PCA_count <= empty_PCA_count) success_collect_size += ftl_merge(min_valid_PCA_block_number, max_empty_PCA_block_number, valid_PCA_count);
    return success_collect_size;
}

int ftl_merge(unsigned int from_block_number, unsigned int to_block_number, unsigned int valid_PCA_count) {
    PCA from_PCA, to_PCA;
    int success_merge_size = 0;

    from_PCA.info.block_number = from_block_number;
    from_PCA.info.page_number = 0;

    to_PCA.info.block_number = to_block_number;
    to_PCA.info.page_number = 0;

    char* buf_temp = calloc(512, sizeof(char));

    while (valid_PCA_count > 0) {
        while (get_PCA_statement(&from_PCA) != PCA_HAVE_DATA) from_PCA.info.page_number++;
        while (get_PCA_statement(&to_PCA) != PCA_EMPTY) to_PCA.info.page_number++;

        for (int i = 0; i < NAND_LOGICAL_COUNT * NAND_PAGE_COUNT; i++) {
            PCA PCA;
            PCA.value = L2P_table[i];

            if (PCA.value == from_PCA.value) L2P_table[i] = to_PCA.value;
        }

        nand_read(buf_temp, from_PCA.value);
        PCA_statement_table[NAND_PAGE_COUNT * from_PCA.info.block_number + from_PCA.info.page_number] = PCA_DISABLE;

        nand_write(buf_temp, to_PCA.value);
        PCA_statement_table[NAND_PAGE_COUNT * to_PCA.info.block_number + to_PCA.info.page_number] = PCA_HAVE_DATA;

        valid_PCA_count--;
        success_merge_size++;
    }

    ftl_erase(from_block_number);

    return success_merge_size;
}

void ftl_erase(unsigned int block_number) {
    nand_erase(block_number);

    for (int i = 0; i < NAND_PAGE_COUNT; i++) {
        PCA_statement_table[NAND_PAGE_COUNT * block_number + i] = PCA_EMPTY;
    }

    current_PCA.info.block_number = block_number;
    current_PCA.info.page_number = 0;
}