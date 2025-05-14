#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "task2_hash.h"
#include "task2_count.h"
#include "task2_sum.h"

#define INPUT_FILENAME "pgexam25_test.txt"
#define OUTPUT_FILENAME "pgexam25_output.bin"

struct TASK2_FILE_METADATA {
    char szFileName[32];
    int iFileSize;
    char byHash[4];
    int iSumOfChars;
    char aAlphaCount[26];
};

int main() {
    FILE *fInput = fopen(INPUT_FILENAME, "rb");
    if (fInput == NULL) {
        perror("Could not open input file");
        return 1;
    }

    struct TASK2_FILE_METADATA metadata;
    memset(&metadata, 0, sizeof(metadata));

    // Filename
    strncpy(metadata.szFileName, INPUT_FILENAME, sizeof(metadata.szFileName) - 1);

    // Calculate hash
    unsigned int hash = 0;
    if (Task2_SimpleDjb2Hash(fInput, &hash) != 0) {
        fprintf(stderr, "Error in hash function\n");
        fclose(fInput);
        return 1;
    }
    memcpy(metadata.byHash, &hash, sizeof(metadata.byHash));

    // Count letters
    if (Task2_CountEachCharacter(fInput, metadata.aAlphaCount) != 0) {
        fprintf(stderr, "Error in count function\n");
        fclose(fInput);
        return 1;
    }

    // Calculate file size and sum
    if (Task2_SizeAndSumOfCharacters(fInput, &metadata.iFileSize, &metadata.iSumOfChars) != 0) {
        fprintf(stderr, "Error in size/sum function\n");
        fclose(fInput);
        return 1;
    }

    fclose(fInput);

    // Write to binary file
    FILE *fOutput = fopen(OUTPUT_FILENAME, "wb");
    if (fOutput == NULL) {
        perror("Could not open output file");
        return 1;
    }

    size_t written = fwrite(&metadata, sizeof(metadata), 1, fOutput);
    fclose(fOutput);

    if (written != 1) {
        fprintf(stderr, "Failed to write output file\n");
        return 1;
    }

    printf("Metadata written successfully to %s\n", OUTPUT_FILENAME);
    return 0;
}
