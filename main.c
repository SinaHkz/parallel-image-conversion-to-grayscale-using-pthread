#include "utilities.h"
#include "pthread.h"

#define INPUT_FILE_NAME "input/in%d.bmp"
#define OUTPUT_FILE_NAME "output/out%d.bmp"

void *processImage(void *imageNum){
    int bodySize;
	int width, height;

    int index = (int) imageNum;
    char inputFile[15];
    char outputFilename[20];

    clock_t start,end;
    int time_spent;
    ImageData *imageData = (ImageData *) malloc(sizeof(ImageData));


    unsigned char *header = NULL;
	unsigned char *body = NULL;
	unsigned char *results = NULL;

    sprintf(inputFile, INPUT_FILE_NAME, index);

    int statusOpeningImage = OpenImage(inputFile, &header, &body, &bodySize, &width, &height);
    if (statusOpeningImage != OPENING_IMAGE_SUCCESS) {
        printf("Error: An earlier error occured with the given description.\n");
    }
    
    (*imageData).grayValues = body;
    (*imageData).size = bodySize;

    
    start = clock();
    ConvertRGBtoGrayscale((void *) imageData);
    end = clock();


    time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Time spent on input %d: %f microseconds \n", index, time_spent * 1000);

    // cummulativetime += time_spent;
    
    sprintf(outputFilename, OUTPUT_FILE_NAME, index);
    WriteImage(outputFilename, header, body, bodySize);

    free(results);
	free(header);
	free(body);
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    int rc;
    void *status;

	double time_spent;
    double cummulativetime = 0;

    // unsigned char *header = NULL;
	// unsigned char *body = NULL;
	// unsigned char *results = NULL;

	clock_t start, end;
    
    pthread_t threads[NUMBER_OF_INPUTS];


    for (int index = 1; index <= NUMBER_OF_INPUTS; index++) {
        rc = pthread_create(&threads[index], NULL, processImage, (void *)index);
        if (rc){
            printf("Error; return code from pthread_creat() is %d \n",rc);
            exit(-1);
        }
    }

    for (int index = 1; index <= NUMBER_OF_INPUTS; index++){
        rc = pthread_join(threads[index], &status);
        if (rc){
            printf("Error; return code from pthread_join() is %d \n",rc);
            exit(-1);
        }
        printf("Complete join with thread %d \n", (long)status);
        }
    pthread_exit(NULL);

   


    // printf("Total Time Spent: %f microseconds \n", cummulativetime * 1000);

    return 0;
}
