#ifndef NAND_FUNCTION_H
#define NAND_FUNCTION_H

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "../definitions/NAND.h"
#include "../definitions/PCA.h"
#include "../definitions/SSD.h"

int nand_read(char*, PCA_TYPE);
int nand_write(const char* buf, PCA_TYPE);
int nand_erase(int);

#endif // NAND_FUNCTION