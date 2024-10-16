#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "NAND.h"
#include "PCA.h"
#include "SSD.h"

static int nand_read(char*, PCA_TYPE);
static int nand_write(const char* buf, PCA_TYPE);
static int nand_erase(int);