#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 256

int main() {
    FILE *inputFile, *outputFile;
    char buffer[BUFFER_SIZE];

	// open file for reading 
    inputFile = fopen("datacar_locations.tmp", "r");
    if (inputFile == NULL) {
        perror("Error opening input file");
        return EXIT_FAILURE;
    }

	// open file for writting 
    outputFile = fopen("processed_locations.txt", "w");
    if (outputFile == NULL) {
        perror("Error opening output file");
        fclose(inputFile);
        return EXIT_FAILURE;
    }

	// reading lines
    while (fgets(buffer, BUFFER_SIZE, inputFile) != NULL) {
		// change the format of file
        if (strncmp(buffer, "Data from GPS:", 14) == 0) 
		{
            fprintf(outputFile, "D:%s", buffer + 14);
        }
		else if (strncmp(buffer, "Static time:", 12) == 0) {
            char *pos = strchr(buffer, '(');
            if (pos != NULL) {
                pos++;
                char *endPos = strchr(pos, ')');
                if (endPos != NULL) {
                    *endPos = '\0';  
                }
                fprintf(outputFile, "S: %s\n", pos);
            }
        }
    }

    // Закрываем файлы
    fclose(inputFile);
    fclose(outputFile);

    printf("File processing complete.\n");
    return EXIT_SUCCESS;
}
