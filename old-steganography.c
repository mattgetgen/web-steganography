#include <stdio.h>
#include <string.h>

/* Function Declaration */
void standardOut(char inputFile[1], int arrayOffset[], int width[], int height[]);
void loopHandler(char inputFile[1], char outputFile[1], char *message, int arrayOffset[], int width[], int height[]);
int findNextBit(char *message);

int charIterator;
int byteIterator;


int main(int argc, char *argv[] ) {
    if ( argc != 4) printf("!! Input Arguments: <Input filename> <Output filename> <Secret Message>\n");
    else {
        FILE *inputImage = fopen(argv[1], "r");
        FILE *outputImage = fopen(argv[2], "r");

        // pointer array
        int arrayOffset[1];
        int width[1];
        int height[1];

        charIterator = 0;
        byteIterator = 0;

        if (inputImage == NULL || outputImage == NULL) {
            printf("File not found!\n");
            return -1;
        } else {
            fclose(inputImage);
            fclose(outputImage);
        }

        standardOut(argv[1], arrayOffset, width, height);
        loopHandler(argv[1], argv[2], argv[3], arrayOffset, width, height);
    }
    return 0;
}

void standardOut(char inputFile[1], int arrayOffset[], int width[], int height[]) {
    FILE *inputImage = fopen(inputFile, "r");

    /* Standard Output */

    // starting position in pixel array
    fseek(inputImage, 10, SEEK_SET);
    fread(arrayOffset, 4, 1, inputImage);

    // width of the image
    fseek(inputImage, 18, SEEK_SET);
    fread(width, 4, 1, inputImage);

    // height of the image
    fseek(inputImage, 22, SEEK_SET);
    fread(height, 4, 1, inputImage);

    // Standard Output
    printf("Pixels start at: %d\n", arrayOffset[0]);
    printf("Width of image: %d\n", width[0]);
    printf("Height of image: %d\n", height[0]);

    fclose(inputImage);
}

void loopHandler(char inputFile[1], char outputFile[1], char *message, int arrayOffset[], int width[], int height[]) {
    FILE *inputImage = fopen(inputFile, "r");
    FILE *outputImage = fopen(outputFile, "w");

    char buffer[4];
    char header[arrayOffset[0]];
    char newBuffer[1];
    unsigned char messageBit, colorLSB;

    // copy file header
    fread(header, arrayOffset[0], 1, inputImage);
    fwrite(header, arrayOffset[0], 1, outputImage);

    // run as long as there are pixels
    while ( fread(buffer, 4, 1, inputImage) ) {
        for ( int i = 0; i < sizeof(buffer); i++ ) {
            // the alpha character, or there are no more bits in the message
            if ( i == 3 || ( byteIterator == 8 && charIterator == strlen(message) ) ) {
                newBuffer[0] = buffer[i];
                fwrite(newBuffer, 1, 1, outputImage);
            } else {
                messageBit = findNextBit(message);
                colorLSB = buffer[i]&1;

                // Check if the LSBs are different
                if (colorLSB != messageBit) {
                    // LSB 0 -> 1
                    if (messageBit == 1) {
                        newBuffer[0] = buffer[i] | messageBit;
                        fwrite(newBuffer, 1, 1, outputImage);
                    // LSB 1 -> 0
                    } else if (messageBit == 0) {
                        newBuffer[0] = buffer[i] ^ 1;
                        fwrite(newBuffer, 1, 1, outputImage);
                    }
                } else { // no change necessary, LSBs are equal
                    newBuffer[0] = buffer[i];
                    fwrite(newBuffer, 1, 1, outputImage);
                }
            }
        }
    }
    fclose(inputImage);
    fclose(outputImage);
}

int findNextBit(char *message) {
    // reached end of char, go to next
    if ( byteIterator == 8 ) {
        byteIterator = 0;
        charIterator++;
    }
    // reached end of message, add null byte
    if ( charIterator >= strlen(message) ) {
        byteIterator++;
        return 0;
    }
    unsigned char byteMessage = message[charIterator]>>byteIterator;
    byteIterator++;
    return byteMessage&1;
}