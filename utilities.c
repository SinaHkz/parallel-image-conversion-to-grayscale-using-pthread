#include "utilities.h"


void GetAttributes(FILE* inputFile, unsigned char** header, unsigned char** body, int* bodySize, int* width, int* height)
{
    int size;

    *header = (unsigned char *) malloc((HEADER_SIZE) * sizeof(unsigned char));
    fread(*header, sizeof(unsigned char), HEADER_SIZE, inputFile);

    fseek(inputFile, 0, SEEK_END);
    size = (int) ftell(inputFile);
    fseek(inputFile, HEADER_SIZE, SEEK_SET);

    *body = (unsigned char *) malloc((size - HEADER_SIZE) * sizeof(unsigned char));
    fread(*body, sizeof(unsigned char), size - HEADER_SIZE, inputFile);

    *bodySize = size - HEADER_SIZE;

    *width = *(int*)&(*header)[18];  // Width is at offset 18 in the BMP header
    *height = *(int*)&(*header)[22]; // Height is at offset 22 in the BMP header

}

int OpenImage(char* fileName, unsigned char** header, unsigned char** body, int* bodySize, int* width, int* height)
{
    FILE *inputFile = fopen(fileName, "r");
    if (inputFile == NULL) {
        printf("Error: Could not open file.\n");
        return OPENING_IMAGE_FAILURE;
    }
    
	GetAttributes(inputFile, header, body, bodySize, width, height);
    fclose(inputFile);

    if (header == NULL || body == NULL) {
        printf("Error: Header or body is null.\n");
        return OPENING_IMAGE_NULL_HB;
    }
	
    return OPENING_IMAGE_SUCCESS;
}

int WriteImage(char* fileName, unsigned char* header, unsigned char* body, int bodySize)
{
    FILE *outputFile = fopen(fileName, "w");
    if (outputFile == NULL) {
        printf("Error: Could not open file.\n");
        return OPENING_IMAGE_FAILURE;
    }
    if (header == NULL || body == NULL) {
        printf("Error: Header or body is null.\n");
        return OPENING_IMAGE_NULL_HB;
    }

    fwrite(header, sizeof(unsigned char), HEADER_SIZE, outputFile);
    fwrite(body, sizeof(unsigned char), bodySize, outputFile);
    fclose(outputFile);
	
    return OPENING_IMAGE_SUCCESS;
}


typedef struct 
{
    unsigned char *gray_values;
    int start;
    int end;
}threadData;



void *convertThread(void *arg){
    threadData *data = (threadData *)arg;
    unsigned char *gray_values = data->gray_values;
    int start = data->start;
    int end = data->end;
    unsigned char red, green, blue, gray;
    for (size_t i = start; i < end; i += 3 * ADJACENT){
        red = gray_values[i];
        green = gray_values[i + 1];
        blue = gray_values[i + 2];
        

        gray = 0.2126 * red + 0.7152 * green + 0.0722 * blue;

        gray_values[i] = gray;
        gray_values[i + 1] = gray;
        gray_values[i + 2] = gray;

        //unrolling 1

        red = gray_values[i + 3];
        green = gray_values[i + 4];
        blue = gray_values[i + 5];
        

        gray = 0.2126 * red + 0.7152 * green + 0.0722 * blue;

        gray_values[i + 3] = gray;
        gray_values[i + 4] = gray;
        gray_values[i + 5] = gray;

        //unrolling 2

        red = gray_values[i + 6];
        green = gray_values[i + 7];
        blue = gray_values[i + 8];
        

        gray = 0.2126 * red + 0.7152 * green + 0.0722 * blue;

        gray_values[i + 6] = gray;
        gray_values[i + 7] = gray;
        gray_values[i + 8] = gray;
    }
    pthread_exit(NULL);
}


void ConvertRGBtoGrayscale(void* img)
{
    ImageData *data = (ImageData *) img;
    unsigned char *rgb_values = data->rgbValues;
    unsigned char *gray_values = data->grayValues;
    size_t size = data->size;
    pthread_t threads[THREAD_NUM];
    threadData threadData[THREAD_NUM];
    size_t chunk_size = size / THREAD_NUM;
    size_t remainder = size % THREAD_NUM;

        


    for (size_t i = 0; i < THREAD_NUM; i ++) {
        threadData[i].gray_values = gray_values;
        threadData[i].start = i * chunk_size;
        threadData[i].end = (i + 1) * chunk_size;
        if (threadData[i].start % 3 != 0)
            threadData[i].start -= threadData[i].start % 3;
        if (threadData[i].end % 3 != 0)
            threadData[i].end -= threadData[i].end % 3;

        if (i == THREAD_NUM - 1) {
        threadData[i].end += remainder;
        }

        int rc = pthread_create(&threads[i],NULL,convertThread,&threadData[i]);
        if (rc) {
            fprintf(stderr, "Error creating thread %d. Return code: %d\n", i, rc);
            exit(EXIT_FAILURE);
        }      
    }

    for (size_t i = 0; i < THREAD_NUM; i++) {
    printf("Thread %zu: start=%d, end=%d\n", i, threadData[i].start, threadData[i].end);
    }   

    for (int i = 0; i < THREAD_NUM; i++) {
        int rc = pthread_join(threads[i], NULL);
        if (rc) {
            fprintf(stderr, "Error joining thread %d. Return code: %d\n", i, rc);
            exit(EXIT_FAILURE);
        }
    }
}