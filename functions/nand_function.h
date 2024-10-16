#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "../definitions/NAND.h"
#include "../definitions/PCA.h"
#include "../definitions/SSD.h"

static int nand_read(char*, PCA_TYPE);
static int nand_write(const char* buf, PCA_TYPE);
static int nand_erase(int);