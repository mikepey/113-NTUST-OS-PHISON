gcc -Wall ssd_fuse.c definitions/*.c functions/*.c interfaces/*.c `pkg-config fuse3 --cflags --libs` -D_FILE_OFFSET_BITS=64 -o ssd_fuse
gcc -Wall ssd_fuse_dut.c -o ssd_fuse_dut