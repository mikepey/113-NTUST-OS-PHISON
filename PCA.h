#define PCA_TYPE int 

typedef union PCA {
    unsigned int value;

    struct PCA_INFO {
        unsigned int page_number : 16;
        unsigned int block_number : 16;
    } info;
} PCA;

enum PCA_STATEMENT {
    PCA_FULL = 0xFFFFFFFE, // 4294967294 (unsigned int的最大值 - 1)
    PCA_INVALID = 0xFFFFFFFF, // 4294967295 (unsigned int的最大值)
};

PCA current_PCA;