#include <stdbool.h>

#include "nand_function.h"
#include "PCA.h"

unsigned int* L2P_address_table;

static unsigned int get_next_PCA();
static int ftl_read(char*, size_t);
static int ftl_write(const char* , size_t, size_t);