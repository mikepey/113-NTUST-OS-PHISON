#include "nand_function.h"

static int nand_read(char* buf, PCA_TYPE read_PCA) {    
    FILE* fptr;

    PCA PCA;
    PCA.value = read_PCA;

    char NAND_path[100] = {};
    snprintf(NAND_path, 100, "%s/nand_%d", NAND_LOCATION, PCA.info.block_number);

    if (!(fptr = fopen(NAND_path, "r"))) {
        printf("open file fail at nand read pca = %d\n", read_PCA);
        return -EINVAL;
    }

    fseek(fptr, PCA.info.page_number * 512, SEEK_SET);
    fread(buf, 1, 512, fptr);
    fclose(fptr);

    return 512;
}

static int nand_write(const char* buf, PCA_TYPE write_PCA) {
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

    fseek(fptr, PCA.info.page_number * 512, SEEK_SET);
    fwrite(buf, 1, 512, fptr);
    fclose(fptr);

    ssd_size.physic++;
    ssd_size.nand_write += 512;

    return 512;
}

static int nand_erase(int block_number) {
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