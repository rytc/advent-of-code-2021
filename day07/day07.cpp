#include "../utils/utils.h"

int main(int argc, char** argv) {
    File input = read_entire_file("input.txt");

    u32 crabCount = 0;
    for(u32 i = 0; i < input.length; i++) {
        if(input.data[i] == ',') crabCount += 1;
        if(input.data[i] == '\n') {
            crabCount += 1;
            break;
        }
    }

    printf("There are %i crab submarines!\n", crabCount);
    s16* crabmarines = (s16*)malloc(crabCount * sizeof(s16));

    for(u32 i = 0; i < crabCount; i++) {
        char num[6] = {};
        u32 comma = find_next(input, ',');
        if(comma >= input.length) comma = find_next(input, '\n');
        memcpy(num, input.data + input.cursor, comma - input.cursor);
        crabmarines[i] = atoi(num);
        input.cursor = comma + 1;
    }

    u32 minPos = 9999;
    u32 maxPos = 0;
    u32 mean = 0;

    s16 modeTable[2000] = {};

    for(u32 i = 0; i < crabCount; i++) {
        s16 pos = crabmarines[i];
        if(pos < minPos) minPos = pos;
        if(pos > maxPos) maxPos = pos;
        mean += pos;
        modeTable[pos] += 1;
    }

    u32 mode = 0;
    u32 modeMax = 0;
    for(u32 i = 0; i < 2000; i++) {
        if(modeTable[i] > modeMax) {
            modeMax = modeTable[i];
            mode = i;
        }
    }

    printf("Min: %i Max: %i\n", minPos, maxPos);
    printf("Mode %i with %i crabmarines\n", mode, modeMax);

    u32 center = (maxPos - minPos) / 2;
    u32 median = crabmarines[crabCount/2];
    mean = mean / crabCount;

    printf("Center %i\n", center);
    printf("Median %i\n", median);
    printf("Mean   %i\n", mean);

    u32 totalFuelMean = 0;
    u32 totalFuelMedian = 0;
    u32 totalFuelMode = 0;
    u32 totalFuelCenter = 0;

    for(u32 i = 0; i < crabCount; i++) {
        s16 pos = crabmarines[i];
        totalFuelMean += MAX(pos, mean) - MIN(pos, mean);
        totalFuelMedian += MAX(pos, median) - MIN(pos, median);
        totalFuelMode += MAX(pos, mode) - MIN(pos, mode);
        totalFuelCenter += MAX(pos, center) - MIN(pos, center);
    }

    u32 totalFuelBruteForce = 999999;
    u32 bruteForceBestPos = 0;
    for(u32 i = minPos; i < maxPos; i++) {
        u32 fuel = 0;
        for(u32 k = 0; k < crabCount; k++) {
            s16 pos = crabmarines[k];
            fuel += MAX(i, pos) - MIN(i, pos);
        }

        if(fuel < totalFuelBruteForce) {
            totalFuelBruteForce = fuel;
            bruteForceBestPos = i;
        }
    }

    printf("--- PART 1 ---\n");

    printf("Brute force best pos %i\n", bruteForceBestPos);

    printf("totalFuelMean:       %i\n", totalFuelMean);
    printf("totalFuelMedian:     %i\n", totalFuelMedian);
    printf("totalFuelMode:       %i\n", totalFuelMode);
    printf("totalFuelCenter:     %i\n", totalFuelCenter);
    printf("totalFuelBruteForce: %i\n", totalFuelBruteForce);


    printf("--- PART 2 ---\n");


    free_file(input);
    return 0;
}