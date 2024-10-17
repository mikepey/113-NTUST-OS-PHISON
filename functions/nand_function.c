#include "nand_function.h"

int nand_read(char* buf, PCA_TYPE read_PCA) {    
    FILE* fptr;

    PCA PCA;
    PCA.value = read_PCA;

    printf("nand_read PCA value: %d\n", PCA.value);
    printf("nand_read PCA block: %d, page: %d\n", PCA.info.block_number, PCA.info.page_number);

    char NAND_path[100] = {};
    snprintf(NAND_path, 100, "%s/nand_%d", NAND_LOCATION, PCA.info.block_number);

    if (!(fptr = fopen(NAND_path, "r"))) {
        printf("open file fail at nand read pca = %d\n", read_PCA);
        return -EINVAL;
    }

    printf("nand_read path: %s\n", NAND_path);

    fseek(fptr, PCA.info.page_number * 512, SEEK_SET);
    printf("fread: %lu\n", fread(buf, 1, 512, fptr));

    printf("buf: %s\n", buf);
    fclose(fptr);

    return 512;
}

int nand_write(const char* buf, PCA_TYPE write_PCA) {
    FILE* fptr;

    PCA PCA;
    PCA.value = write_PCA;

    char NAND_path[100];
    snprintf(NAND_path, 100, "%s/nand_%d", NAND_LOCATION, PCA.info.block_number);

    // write to nand
    if (!(fptr = fopen(NAND_path, "r+"))) {
        printf("open file fail at nand (%s) write pca = %d, return %d\n", NAND_path, write_PCA, -EINVAL);
        return -EINVAL;
    }

    printf("write buf: %s\n", buf);
    printf("write nand path: %s\n", NAND_path);

    printf("write PCA value: %d\n", PCA.value);
    printf("write PCA block: %d, page: %d\n", PCA.info.block_number, PCA.info.page_number);

    fseek(fptr, PCA.info.page_number * 512, SEEK_SET);
    printf("fwrite: %lu\n", fwrite(buf, 1, 512, fptr));
    fclose(fptr);

    ssd_size.physic++;
    ssd_size.nand_write += 512;

    return 512;
}

int nand_erase(int block_number) {
    FILE* fptr;
    // int found = 0; WTF is this?

    char NAND_path[100];
    snprintf(NAND_path, 100, "%s/nand_%d", NAND_LOCATION, block_number);

    // erase nand
    if (!(fptr = fopen(NAND_path, "w"))) {
        printf("open file fail at nand (%s) erase nand = %d, return %d\n", NAND_path, block_number, -EINVAL);
        return -EINVAL;
    }

    fclose(fptr);
    printf("nand erase %d pass\n", block_number);

    /*
        if (found == 0)
        {
            printf("nand erase not found\n");
            return -EINVAL;
        }
        WTF is this?
    */

    return 1;
}