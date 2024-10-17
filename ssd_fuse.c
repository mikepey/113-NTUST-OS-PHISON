/*
    FUSE ssd: FUSE ioctl example
    Copyright (C) 2008       SUSE Linux Products GmbH
    Copyright (C) 2008       Tejun Heo <teheo@suse.de>
    This program can be distributed under the terms of the GNU GPLv2.
    See the file COPYING.
*/
#include "functions/ftl_function.h"
#include "interfaces/ssd_operation.h"

#include "definitions/PCA.h"
#include "definitions/SSD.h"

int main(int argc, char* argv[]) {
    char NAND_path[100] = {};

    ssd_size.physic = 0;
    ssd_size.logic = 0;
    ssd_size.nand_write = 0;
    ssd_size.host_write = 0;

    current_PCA.value = PCA_EMPTY;

    L2P_table = malloc(NAND_LOGICAL_COUNT * NAND_PAGE_COUNT * sizeof(PCA_TYPE));

    for (int i = 0; i < NAND_LOGICAL_COUNT; i++) {
        for (int j = 0; j < NAND_PAGE_COUNT; j++) L2P_table[NAND_PAGE_COUNT * i + j] = L2P_EMPTY_PCA;
    }

    PCA_statement_table = malloc(NAND_PHYSICAL_COUNT * NAND_PAGE_COUNT * sizeof(PCA_TYPE));
    memset(PCA_statement_table, PCA_EMPTY, NAND_PHYSICAL_COUNT * NAND_PAGE_COUNT * sizeof(PCA_TYPE));

    for (int i = 0; i < NAND_PHYSICAL_COUNT; i++) {
        snprintf(NAND_path, 100, "%s/nand_%d", NAND_LOCATION, i);

        FILE* fptr = fopen(NAND_path, "w"); 
        if (fptr == NULL) printf("open fail\n");
        
        fclose(fptr);
    }

    return fuse_main(argc, argv, &ssd_operations, NULL);
}