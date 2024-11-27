#include "utilities.h"

#define INPUT_FILE_NAME "input/in%d.bmp"
#define OUTPUT_FILE_NAME "output/out%d.bmp"

int main(int argc, char *argv[])
{
	int bodySize;
	int width, height;

	double time_spent;
    double cummulativetime = 0;

    unsigned char *header = NULL;
	unsigned char *body = NULL;
	unsigned char *results = NULL;

	clock_t start, end;
    ImageData *imageData = (ImageData *) malloc(NUMBER_OF_INPUTS * sizeof(ImageData));
    
    char inputFile[15];
    char outputFilename[20];


    for (int index = 1; index <= NUMBER_OF_INPUTS; index++) {
        sprintf(inputFile, INPUT_FILE_NAME, index);

        int statusOpeningImage = OpenImage(inputFile, &header, &body, &bodySize, &width, &height);
	    if (statusOpeningImage != OPENING_IMAGE_SUCCESS) {
		    printf("Error: An earlier error occured with the given description.\n");
		    return statusOpeningImage;
	    }
        
        imageData[index - 1].grayValues = body;
        imageData[index - 1].size = bodySize;

        
        start = clock();
        ConvertRGBtoGrayscale((void *) &imageData[index - 1]);
        end = clock();


        time_spent = (double)(end - start) / CLOCKS_PER_SEC;
        printf("Time spent on input %d: %f microseconds \n", index, time_spent * 1000);

        cummulativetime += time_spent;
        
        sprintf(outputFilename, OUTPUT_FILE_NAME, index);
        WriteImage(outputFilename, header, body, bodySize);
        
    }

    free(results);
	free(header);
	free(body);


    printf("Total Time Spent: %f microseconds \n", cummulativetime * 1000);

    return 0;
}
