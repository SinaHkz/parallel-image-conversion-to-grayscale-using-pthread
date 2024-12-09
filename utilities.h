#if !defined(UTILS)
#define UTILS

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stddef.h>
#include <math.h>

#define HEADER_SIZE 54
#define NUMBER_OF_INPUTS 8
#define ADJACENT 3

#define OPENING_IMAGE_SUCCESS 0
#define OPENING_IMAGE_FAILURE 1
#define OPENING_IMAGE_NULL_HB 2
#define THREAD_NUM 5

typedef struct
{
    size_t size;
    unsigned char *rgbValues;
    unsigned char *grayValues;
} ImageData;

void GetAttributes(FILE*, unsigned char**, unsigned char**, int*, int*, int*);
int OpenImage(char*, unsigned char**, unsigned char**, int*, int*, int*);
void ConvertRGBtoGrayscale(void*);
int WriteImage(char*, unsigned char*, unsigned char*, int);

#endif // UTILS