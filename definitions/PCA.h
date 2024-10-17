#ifndef PCA_H
#define PCA_H

#define PCA_TYPE unsigned int 
#define L2P_EMPTY_PCA 0xFFFFFFFF

typedef union PCA {
    unsigned int value;

    struct PCA_INFO {
        unsigned int page_number : 16;
        unsigned int block_number : 16;
    } info;
} PCA;

typedef enum PCA_STATEMENT {
    PCA_EMPTY,
    PCA_DISABLE,
    PCA_HAVE_DATA,
} PCA_STATEMENT;

extern PCA current_PCA;

#endif // PCA_H