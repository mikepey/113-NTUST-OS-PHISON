#ifndef SSD_FUNCTION_H
#define SSD_FUNCTION_H

#define FUSE_USE_VERSION 35
#define DEBUG 0

#include <fuse.h>
#include <errno.h>
#include <string.h>

#include "../functions/ftl_function.h"

#include "../definitions/NAND.h"
#include "../definitions/SSD.h"

#define min(X, Y) ((X) < (Y) ? (X) : (Y))
#define max(X, Y) ((X) > (Y) ? (X) : (Y))

void print_ssd();

int ssd_resize(size_t);
int ssd_expand(size_t);
int ssd_read(char*, size_t, off_t);
int ssd_write(const char*, size_t, off_t);

#endif // SSD_FUCNTION_H