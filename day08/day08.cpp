#include "../utils/utils.h"

struct Segments {
    char e[7];
    u8 len;
};

struct Signal {
    Segments pattern[10];
    Segments output[4];
};

int main(int argc, char** argv) {
    File input = read_entire_file("input.txt");

    u32 entryCount = count_newlines(input);
    Signal* signals = (Signal*)calloc(entryCount, sizeof(Signal));

    for(u32 signalIndex = 0; signalIndex < entryCount; signalIndex++) {
        Signal* signal = &signals[signalIndex];
        for(u32 patternIndex = 0; patternIndex < 10; patternIndex++) {
            u32 space = find_next(input, ' ');
            u8 len = space - input.cursor;
            signal->pattern[patternIndex].len = len;
            memcpy(signal->pattern[patternIndex].e, input.data + input.cursor, len);
            input.cursor = space;

        }

        input.cursor = find_next(input, '|') + 2;

        for(u32 outputIndex = 0; outputIndex < 4; outputIndex++) {
            u32 space = find_next(input, ' ');
            if(outputIndex == 3) space = find_next(input, '\n');
            u8 len = space - input.cursor;
            signal->output[outputIndex].len = len;
            memcpy(signal->output[outputIndex].e, input.data + input.cursor, len);
            input.cursor = space + 1;
        }
    }

    // Part 1
    u32 count1478 = 0;
    for(u32 signalIndex = 0; signalIndex < entryCount; signalIndex++) {
        for(u32 i = 0; i < 4; i++) {
            u8 len = signals[signalIndex].output[i].len;
            printf("%i ", len);
            if(len == 2 || len == 4 || len == 3 || len == 7) {
                count1478 += 1;
            }
        }
        printf("\n");
    }

    printf("1,4,7,8 appear %i times\n", count1478);

    free(signals);
    free_file(input);
    return 0;
}