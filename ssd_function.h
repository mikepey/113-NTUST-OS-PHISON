#include <fuse.h>
#include <errno.h>

#include "NAND.h"
#include "SSD.h"

static int ssd_resize(size_t);
static int ssd_expand(size_t);
static int ssd_read(char*, size_t, off_t);
static int ssd_write(const char*, size_t, off_t);