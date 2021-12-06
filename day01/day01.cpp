#include "../utils/utils.h"


int main(int argc, char** argv) {

    File input = read_entire_file("input.txt");

    u32 sonarScanCount = count_newlines(input);
    s32 *elevationArray = (s32*)malloc(sonarScanCount * sizeof(s32));

    for(s32 i = 0; i < sonarScanCount; i++) {
        elevationArray[i] = readline_s32(&input);
    }

    s32 elevationIncreases = 0;
    s32 lastElevation = 0;
    for(s32 i = 0; i < sonarScanCount; i++) {
        s32 elevationScan = elevationArray[i];

        if(lastElevation == 0) {
            lastElevation = elevationScan;
            printf("%i (No previous measurement)\n", elevationScan);
            continue;
        }

        if(lastElevation < elevationScan) {
            printf("%i (increased)\n", elevationScan);
            elevationIncreases += 1;
        } else {
            printf("%i (decreased)\n", elevationScan);
        }

        lastElevation = elevationScan;
    }

    printf("Total elevation increases: %i\n", elevationIncreases);

    free(elevationArray);
    free_file(input);

    return 0;
}