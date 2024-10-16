#define NAND_LOCATION  "/home/mikepey/OS/SSD"

#define NAND_SIZE (10) // 10 KB (10240 Bytes)
#define NAND_LOGICAL_COUNT (5)
#define NAND_PHYSICAL_COUNT (8)

/* 擴展宏 */
#define NAND_BYTE_SIZE (NAND_SIZE * 1024)
#define NAND_PAGE_COUNT (NAND_SIZE * 1024 / 512) // 20