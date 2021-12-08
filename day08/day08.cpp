#include "../utils/utils.h"

// Unique number signal lengths
constexpr u8 LEN_1 = 2;
constexpr u8 LEN_4 = 4;
constexpr u8 LEN_7 = 3;
constexpr u8 LEN_8 = 7;
constexpr u8 ASCII_A = 97;


struct Segments {
    char e[7];
    u8 len;
};

struct Signal {
    Segments pattern[10];
    Segments output[4];
};


static u32
get_numeric(s8* mapping, Segments disp) {
    if(disp.len == LEN_1) return 1;
    if(disp.len == LEN_4) return 4;
    if(disp.len == LEN_7) return 7;
    if(disp.len == LEN_8) return 8;

    u8 segments[7] = {};
    for(u32 i = 0; i < disp.len; i++) {
        u32 mapIndex = disp.e[i] - ASCII_A;
        u32 segmentIndex = mapping[mapIndex];
        segments[segmentIndex] = 1;
    }

    if(segments[0] && segments[1] && segments[2] && segments[4] && segments[5] && segments[6]) {
        return 0;
    }

    if(segments[0] && segments[2] && segments[3] && segments[4] && segments[6]) {
        return 2;
    }

    if(segments[0] && segments[2] && segments[3] && segments[5] && segments[6]) {
        return 3;
    }

    if(segments[0] && segments[1] && segments[3] && segments[5] && segments[6]) {
        return 5;
    }

    if(segments[0] && segments[1] && segments[3] && segments[4] && segments[5] && segments[6]) {
        return 6;
    }

    printf("! ERROR: unknown numeric value %s\n", disp.e);

#if 0 
    if(segments[0]) { printf("  a\n"); } else { printf("\n"); }
    if(segments[1]) { printf(" b"); }
    if(segments[2]) { printf(" c\n"); } else {printf("\n"); }
    if(segments[3]) { printf("  d\n"); }
    if(segments[4]) { printf(" e"); }
    if(segments[5]) { printf(" f\n"); } else {printf("\n"); }
    if(segments[6]) { printf("  g\n"); }
#endif 
    
    return 0;
}

static void
sort_pattern(Signal* signal) {
    for(u32 i = 0; i < 10; i++) {
        for(u32 j = 0; j < 10; j++) {
            if(signal->pattern[j].len > signal->pattern[i].len) {
                Segments s = signal->pattern[j];
                signal->pattern[j] = signal->pattern[i];
                signal->pattern[i] = s;
            }
        }
    }
}

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
            input.cursor = space + 1;

        }

        input.cursor += 2;

        for(u32 outputIndex = 0; outputIndex < 4; outputIndex++) {
            u32 space = find_next(input, ' ');
            if(outputIndex == 3) space = find_next(input, '\n');
            u8 len = space - input.cursor;
            signal->output[outputIndex].len = len;
            memcpy(signal->output[outputIndex].e, input.data + input.cursor, len);
            input.cursor = space + 1;
        }
    }

    printf("---- PART 1 ----\n");

    u32 count1478 = 0;
    for(u32 signalIndex = 0; signalIndex < entryCount; signalIndex++) {
        for(u32 i = 0; i < 4; i++) {
            u8 len = signals[signalIndex].output[i].len;
            printf("%i ", len);
            if(len == LEN_1 || len == LEN_4 || len == LEN_7 || len == LEN_8) {
                count1478 += 1;
            }
        }
        printf("\n");
    }

    printf("1,4,7,8 appear %i times\n", count1478);


    printf("---- PART 2 ----\n");

    u32 outputTotal = 0;
    for(u32 signalIndex = 0; signalIndex < entryCount; signalIndex++) {
        Signal signal = signals[signalIndex];
        sort_pattern(&signal);

        // ascii to segment
        s8 mapping[7] = {-1,-1,-1,-1,-1,-1,-1};
        // segment to ascii
        s8 reverseMapping[7] = {-1,-1,-1,-1,-1,-1,-1};

        u8 mappingCompletion = 0;

        for(u32 i = 0; i < 10; i++) {
            Segments pattern = signal.pattern[i];

            if(pattern.len == LEN_1) {
                u8 segs[2] = {2, 5};
                
                // For each character in the pattern
                for(u32 k = 0; k < LEN_1; k++) {
                    u8 value = pattern.e[k] - ASCII_A;

                    // If that character has not yet been mapped 
                    if(mapping[value] == -1) {

                        // Try to map it to one of the two available segments
                        for(u32 s = 0; s < LEN_1; s++) {
                            u8 seg = segs[s];

                            // Check if that segment has already been mapped
                            if(reverseMapping[seg] == -1) {
                                mapping[value] = seg;
                                reverseMapping[seg] = value;
                                mappingCompletion += 1;
                                break;
                            }
                        }
                    }
                }
            }

            if(pattern.len == LEN_4) {
                u8 segs[4] = {1, 2, 3, 5};
                
                // For each character in the pattern
                for(u32 k = 0; k < LEN_4; k++) {
                    u8 value = pattern.e[k] - ASCII_A;

                    // If that character has not yet been mapped 
                    if(mapping[value] == -1) {

                        // Try to map it to one of the two available segments
                        for(u32 s = 0; s < LEN_4; s++) {
                            u8 seg = segs[s];

                            // Check if that segment has already been mapped
                            if(reverseMapping[seg] == -1) {
                                mapping[value] = seg;
                                reverseMapping[seg] = value;
                                mappingCompletion += 1;
                                break;
                            }
                        }
                    }
                }
            }

            if(pattern.len == LEN_7) {
                u8 segs[3] = {0, 2, 5};
                
                // For each character in the pattern
                for(u32 k = 0; k < LEN_7; k++) {
                    u8 value = pattern.e[k] - ASCII_A;

                    // If that character has not yet been mapped 
                    if(mapping[value] == -1) {
                        // Try to map it to one of the two available segments
                        for(u32 s = 0; s < LEN_7; s++) {
                            u8 seg = segs[s];

                            // Check if that segment has already been mapped
                            if(reverseMapping[seg] == -1) {
                                mapping[value] = seg;
                                reverseMapping[seg] = value;
                                mappingCompletion += 1;
                                break;
                            }
                        }
                    }
                }
            }

            if(pattern.len == LEN_8) {
                u8 segs[7] = {0,1,2,3,4,5,6};
                
                // For each character in the pattern
                for(u32 k = 0; k < LEN_8; k++) {
                    u8 value = pattern.e[k] - ASCII_A;

                    // If that character has not yet been mapped 
                    if(mapping[value] == -1) {
                        // Try to map it to one of the two available segments
                        for(u32 s = 0; s < LEN_8; s++) {
                            u8 seg = segs[s];

                            // Check if that segment has already been mapped
                            if(reverseMapping[seg] == -1) {
                                mapping[value] = seg;
                                reverseMapping[seg] = value;
                                mappingCompletion += 1;
                                break;
                            }
                        }
                    }
                }
            }
        }

        if(mappingCompletion != 7) {
            printf("! ERROR: incomplete mapping (%i) at signal %i\n", mappingCompletion, signalIndex);
            continue;
        }

        for(u32 i = 0; i < 4; i++) {
            outputTotal += get_numeric(&mapping[0], signal.output[i]);
        }
    }

    printf("Total of all outputs: %i\n", outputTotal);


    free(signals);
    free_file(input);
    return 0;
}