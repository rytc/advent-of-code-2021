#include "../utils/utils.h"


//
// Day01 Part 1
//
static s32
count_elevation_increases(s32 *elevationArray, u32 sonarScanCount) {
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

    return elevationIncreases;
}

//
// Day01 Part 2
//
static s32
count_elevation_increases_windowed(s32 *elevationArray, u32 sonarScanCount) {
    s32 elevationIncreases = 0;
    s32 lastElevation = 0;
    for(u32 i = 0; i < sonarScanCount - 2; i++) {
        s32 scan0 = elevationArray[i];
        s32 scan1 = elevationArray[i + 1];
        s32 scan2 = elevationArray[i + 2];

        s32 total = scan0 + scan1 + scan2;

        if(lastElevation == 0) {
            lastElevation = total;
            printf("%i (No previous measurement)\n", total);
            continue;
        }

        if(total > lastElevation) {
            elevationIncreases += 1;
            printf("%i (increased)\n", total);
        } else {
            printf("%i (decreased)\n", total);
        }


        lastElevation = total;
    }

    return elevationIncreases;

}


int main(int argc, char** argv) {
    File input = read_entire_file("input.txt");

    u32 sonarScanCount = count_newlines(input);
    s32 *elevationArray = (s32*)malloc(sonarScanCount * sizeof(s32));

    for(u32 i = 0; i < sonarScanCount; i++) {
        elevationArray[i] = readline_s32(&input);
    }

    // Part 1
    s32 elevationIncreases = count_elevation_increases(elevationArray, sonarScanCount); 

    // Part 2
    s32 elevationIncreasesWindowed = count_elevation_increases_windowed(elevationArray, sonarScanCount);

    printf("Total elevation increases: %i\n", elevationIncreases);
    printf("Elevation increases over 3 scan window: %i\n", elevationIncreasesWindowed);

    free(elevationArray);
    free_file(input);

    return 0;
}