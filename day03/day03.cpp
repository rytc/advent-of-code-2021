#include "../utils/utils.h"

static void
count_ones_in_columns(u16* data, u32 dataCount, s32 *columns) {
    for(u32 i = 0; i < dataCount; i++) {
        for(u32 k = 0; k < 12; k++) {
            if(data[i] & (1 << (11 - k))) {
                columns[k] += 1;
            }
        }
    }
}

static void
clear_array_s32(s32 *counts, u32 count) {
    for(u32 i = 0; i < count; i++) counts[i] = 0;
}

int main(int argc, char** argv) {
    File input = read_entire_file("input.txt");

    u32 inputCount = count_newlines(input);
    u16 *data = (u16*)calloc(inputCount, sizeof(u16));
    //printf("Line count: %i\n", inputCount);

    s32 onesCount[12] = {};

    for(u32 i = 0; i < inputCount; i++) {
        char line[12] = {};
        readline_string_len(&input, line, 12);
        for(u32 k = 0; k < 12; k++) {
            if(line[k] == '1') {
                data[i] |=  (1 << (11 - k));
            } 
        }
    }

    count_ones_in_columns(data, inputCount, onesCount);

    u16 gammaRate = 0;
    u16 epsilonRate = 0;

    for(u32 i = 0; i < 12; i++) {
        if((inputCount - onesCount[i]) > onesCount[i]) {
            gammaRate |= (1 << (11 - i));
        } else {
            epsilonRate |= (1 << (11 - i));
        }
    }

    s32 oxygenOnesCount[12] = {};
    u16* oxygenGeneratorList = (u16*)malloc(inputCount * sizeof(u16));
    memcpy(oxygenGeneratorList, data, inputCount * sizeof(u16));
    u32 oxygenGenListCount = inputCount;
    u32 oxyIndex = 0;

    s32 co2OnesCount[12] = {};
    u16* co2GeneratorList = (u16*)malloc(inputCount * sizeof(u16));
    memcpy(co2GeneratorList, data, inputCount * sizeof(u16));
    u32 co2GenListCount = inputCount;
    u32 co2Index = 0;

    for(u32 k = 0; k < 12; k++) {
        clear_array_s32(oxygenOnesCount, 12);
        count_ones_in_columns(oxygenGeneratorList, oxygenGenListCount, oxygenOnesCount);

        for(u32 i = 0; i < oxygenGenListCount; i++) {
            u16 value = oxygenGeneratorList[i];
            u16 valueMask = (value & (1 << (11 - k)));
            u32 zeroCount = oxygenGenListCount - oxygenOnesCount[k];
            if(zeroCount < oxygenOnesCount[k] && valueMask > 0) {
                oxygenGeneratorList[oxyIndex] = value;
                oxyIndex += 1;
            } else if(zeroCount > oxygenOnesCount[k] && valueMask == 0) {
                oxygenGeneratorList[oxyIndex] = value;
                oxyIndex += 1;
            } else if(zeroCount == oxygenOnesCount[k] && valueMask > 0) {
                oxygenGeneratorList[oxyIndex] = value;
                oxyIndex += 1;
            }
        }
        oxygenGenListCount = oxyIndex;
        if(oxygenGenListCount == 1) break;
        oxyIndex = 0;
    }

    for(u32 k = 0; k < 12; k++) {
        clear_array_s32(co2OnesCount, 12);
        count_ones_in_columns(co2GeneratorList, co2GenListCount, co2OnesCount);

        for(u32 i = 0; i < co2GenListCount; i++) {
            u16 value = co2GeneratorList[i];
            u16 valueMask = (value & (1 << (11 - k)));
            u32 zeroCount = co2GenListCount - co2OnesCount[k];
            if(zeroCount < co2OnesCount[k] && valueMask == 0) {
                co2GeneratorList[co2Index] = value;
                co2Index += 1;
            } else if(zeroCount > co2OnesCount[k] && valueMask > 0) {
                co2GeneratorList[co2Index] = value;
                co2Index += 1;
            } else if(zeroCount == co2OnesCount[k] && valueMask == 0) {
                co2GeneratorList[co2Index] = value;
                co2Index += 1;
            }
        }
        co2GenListCount = co2Index;
        if(co2GenListCount == 1) break;
        co2Index = 0;
    }



    printf("Gamma Rate: %i\n", gammaRate);
    printf("Epsilon Rate: %i\n", epsilonRate);
    printf("Power consumption: %i\n", gammaRate*epsilonRate);
    printf("Oxygen %i - %i\n", oxygenGenListCount, oxygenGeneratorList[0]);
    printf("CO2 %i - %i\n", co2GenListCount, co2GeneratorList[0]);
    printf("Life support rating: %i\n", oxygenGeneratorList[0] * co2GeneratorList[0]);

    free(oxygenGeneratorList);
    free(co2GeneratorList);
    free_file(input);
    return 0;
}