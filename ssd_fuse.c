/*
    FUSE ssd: FUSE ioctl example
    Copyright (C) 2008       SUSE Linux Products GmbH
    Copyright (C) 2008       Tejun Heo <teheo@suse.de>
    This program can be distributed under the terms of the GNU GPLv2.
    See the file COPYING.
*/

#define FUSE_USE_VERSION 35

#include "ftl_function.h"
#include "ssd_operation.h"

#include "PCA.h"
#include "SSD.h"

int main(int argc, char* argv[]) {
    char NAND_path[100];

    ssd_size.physic = 0;
    ssd_size.logic = 0;
    ssd_size.nand_write = 0;
    ssd_size.host_write = 0;

    current_PCA.value = PCA_INVALID;

    L2P_address_table = malloc(NAND_LOGICAL_COUNT * NAND_PAGE_COUNT * sizeof(int));
    memset(L2P_address_table, PCA_INVALID, NAND_LOGICAL_COUNT * NAND_PAGE_COUNT * sizeof(int));

    for (int i = 0; i < NAND_PHYSICAL_COUNT; i++) {
        snprintf(NAND_path, 100, "%s/nand_%d", NAND_LOCATION, i);

        FILE* fptr = fopen(NAND_path, "w");
        if (fptr == NULL) printf("open fail");

        fclose(fptr);
    }

    return fuse_main(argc, argv, &ssd_operations, NULL);
}