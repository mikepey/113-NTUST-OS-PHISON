#ifndef FTL_FUNCTION_H
#define FTL_FUNCTION_H

#include <stdbool.h>

#include "nand_function.h"
#include "ssd_function.h"

#include "../definitions/PCA.h"

extern unsigned int* L2P_table;
extern PCA_STATEMENT* PCA_statement_table;

PCA_STATEMENT get_PCA_statement(PCA* PCA);
unsigned int get_next_PCA();
unsigned int get_min_valid_PCA(unsigned int*);
unsigned int get_max_empty_PCA(unsigned int*);

bool is_disable_block(unsigned int);

int ftl_read(char*, size_t);
int ftl_write(const char*, size_t);
int ftl_garbage_collection();

int ftl_merge(unsigned int, unsigned int, unsigned int);
void ftl_erase(unsigned int);

#endif // FTL_FUNCTION