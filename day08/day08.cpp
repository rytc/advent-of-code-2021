#include "../utils/utils.h"

// Unique number signal lengths
constexpr u8 LEN_1 = 2;
constexpr u8 LEN_4 = 4;
constexpr u8 LEN_7 = 3;
constexpr u8 LEN_8 = 7;
constexpr u8 ASCII_A = 97;


struct Segments {
    char e[8];
    u8 len;
};

struct Signal {
    Segments pattern[10];
    Segments output[4];
};

//
// Don't mind my lovely o(n^n) sort :|
//
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


static b32
find_in_str(char* str, char delim) {
    char* p = str;
    while(*p != '\0') {
        if(*p == delim) return true;
        p++;
    }

    return false;
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
            signal->pattern[patternIndex].e[7] = '\0';
            input.cursor = space + 1;

        }

        input.cursor += 2;

        for(u32 outputIndex = 0; outputIndex < 4; outputIndex++) {
            u32 space = find_next(input, ' ');
            if(outputIndex == 3) space = find_next(input, '\n');
            u8 len = space - input.cursor;
            signal->output[outputIndex].len = len;
            memcpy(signal->output[outputIndex].e, input.data + input.cursor, len);
            signal->output[outputIndex].e[7] = '\0';

            input.cursor = space + 1;
        }
    }

    printf("---- PART 1 ----\n");

    u32 count1478 = 0;
    for(u32 signalIndex = 0; signalIndex < entryCount; signalIndex++) {
        for(u32 i = 0; i < 4; i++) {
            u8 len = signals[signalIndex].output[i].len;
            //printf("%i ", len);
            if(len == LEN_1 || len == LEN_4 || len == LEN_7 || len == LEN_8) {
                count1478 += 1;
            }
        }
        //printf("\n");
    }

    printf("1,4,7,8 appear %i times\n", count1478);


    printf("---- PART 2 ----\n");


    u32 outputTotal = 0;
    for(u32 signalIndex = 0; signalIndex < entryCount; signalIndex++) {
        Signal signal = signals[signalIndex];
        sort_pattern(&signal);

        u32 oneIndex, sevenIndex, fourIndex;
        for(u32 i = 0; i < 10; i++) {
            if(signal.pattern[i].len == 2) { 
                oneIndex = i;
            } else if(signal.pattern[i].len == 3) {
                sevenIndex = i;
            } else if(signal.pattern[i].len == 4) {
                fourIndex = i;
            }
        }

        u32 values[4] = {};
        for(u32 outputIndex = 0; outputIndex < 4; outputIndex++) {
            u32 len = signal.output[outputIndex].len;
            char* str = signal.output[outputIndex].e;
            if(len == 2) {
                values[outputIndex] = 1;
            } else if(len == 3) {
                values[outputIndex] = 7;
            } else if(len == 4) {
                values[outputIndex] = 4;
            } else if(len == 7) {
                values[outputIndex] = 8;
            } else{
                if(len == 5) {
                    // 2, 3, 5
                    u32 matches = 0;
                    
                    // If this character has in commmon with 1, then it must be a 3
                    if(find_in_str(str, signal.pattern[oneIndex].e[0]) &&
                        find_in_str(str, signal.pattern[oneIndex].e[1])) {
                            values[outputIndex] = 3;
                            continue;
                    }

                    // 2, 5

                    // How many matches against 4
                    for(u32 i = 0; i < 4; i++) {
                        if(find_in_str(str, signal.pattern[fourIndex].e[i])) matches++;
                    }

                    // 5 has three parts in common with 4
                    if(matches >= 3) {
                        values[outputIndex] = 5;
                    } else { // Otherwise it's 5
                        values[outputIndex] = 2;
                    } 

                } else if(len == 6) {
                    // 0, 6, 9
                    u32 matches = 0;

                    // Check against 4
                    for(u32 i = 0; i < 4; i++) {
                        if(find_in_str(str, signal.pattern[fourIndex].e[i])) matches++;
                    }

                    // 4 shares all parts with 9
                    if(matches == 4) {
                        values[outputIndex] = 9;
                        continue;
                    }

                    // 0, 6
                    matches = 0;

                    // check against 7
                    for(u32 i = 0; i < 3; i++) {
                        if(find_in_str(str, signal.pattern[sevenIndex].e[i])) matches++;
                    }

                    // 0 shares with 7, 6 does not
                    if(matches >= 3) {
                        values[outputIndex] = 0;
                    } else {
                        values[outputIndex] = 6;
                    }
                }
            }
        }

        outputTotal += (values[0] * 1000) + (values[1] * 100) + (values[2] * 10) + (values[3]); 
    }


    printf("Total of all outputs: %i\n", outputTotal);


    free(signals);
    free_file(input);
    return 0;
}


#if 0
//
// First attempt at part 2
//

struct Mapping {
    // ascii to segment
    s8 mapping[7];
    // segment to ascii
    s8 reverseMapping[7];
    u32 completion;
};

static Mapping
make_mapping() {
    Mapping result = {
        {-1,-1,-1,-1,-1,-1,-1},
        {-1,-1,-1,-1,-1,-1,-1}
    };
    result.completion = 0;
    return result;
}

static void
rotate_left(u8* array, u8 size) {
    u8 temp = array[0];
    for(u8 i = 0; i < size; i++)
        array[i] = array[i + 1];
    array[size-1] = temp;
}

static void
rotate_left_by(u8* array, u8 size, u32 n) {
    for(u32 i = 0; i < n; i++) {
        rotate_left(array, size);
    }
}

static Mapping
make_mapping(Signal signal, u32 permutation) {
    Mapping map = make_mapping();

    //printf("! Permutation %u\n", permutation);

    for(u32 i = 0; i < 10; i++) {
        Segments pattern = signal.pattern[i];

        if(pattern.len == LEN_1) {
            u8 segs[2] = {2, 5};
            if(permutation % 2) {
                //printf("  Rotate 1\n");
                rotate_left(segs, 2);
            }

            // For each character in the pattern
            for(u32 k = 0; k < LEN_1; k++) {
                u8 value = pattern.e[k] - ASCII_A;

                // If that character has not yet been mapped 
                if(map.mapping[value] == -1) {
                    // Try to map it to one of the two available segments
                    for(u32 s = 0; s < LEN_1; s++) {
                        u8 seg = segs[s];
                        // Check if that segment has already been mapped
                        if(map.reverseMapping[seg] == -1) {
                            map.mapping[value] = seg;
                            map.reverseMapping[seg] = value;
                            map.completion += 1;
                            break;
                        }
                    }
                }
            }
        }
        
        if(pattern.len == LEN_7) {
            u8 segs[3] = {0, 2, 5};
            if(permutation > 2) {
                //printf("  Rotate 7 by %i\n", permutation-2); 
                rotate_left_by(segs, 3, permutation - 2);
            }

            // For each character in the pattern
            for(u32 k = 0; k < LEN_7; k++) {
                u8 value = pattern.e[k] - ASCII_A;
                // If that character has not yet been mapped 
                if(map.mapping[value] == -1) {
                    // Try to map it to one of the two available segments
                    for(u32 s = 0; s < LEN_7; s++) {
                        u8 seg = segs[s];
                        // Check if that segment has already been mapped
                        if(map.reverseMapping[seg] == -1) {
                            map.mapping[value] = seg;
                            map.reverseMapping[seg] = value;
                            map.completion += 1;
                            break;
                        }
                    }
                }
            }
        }

        if(pattern.len == LEN_4) {
            u8 segs[4] = {1, 2, 3, 5};
            if(permutation > 6) { 
                //printf("  Rotate 4 by %i\n", permutation - 6);
                rotate_left_by(segs,4, permutation - 6);
            }

            // For each character in the pattern
            for(u32 k = 0; k < LEN_4; k++) {
                u8 value = pattern.e[k] - ASCII_A;
                // If that character has not yet been mapped 
                if(map.mapping[value] == -1) {
                    // Try to map it to one of the two available segments
                    for(u32 s = 0; s < LEN_4; s++) {
                        u8 seg = segs[s];
                        // Check if that segment has already been mapped
                        if(map.reverseMapping[seg] == -1) {
                            map.mapping[value] = seg;
                            map.reverseMapping[seg] = value;
                            map.completion += 1;
                            break;
                        }
                    }
                }
            }
        }

        

        if(pattern.len == LEN_8) {
            u8 segs[7] = {0,1,2,3,4,5,6};
            //if(permutation > 11) rotate_left_by(segs, 7, permutation - 11);
            
            // For each character in the pattern
            for(u32 k = 0; k < LEN_8; k++) {
                u8 value = pattern.e[k] - ASCII_A;
                // If that character has not yet been mapped 
                if(map.mapping[value] == -1) {
                    // Try to map it to one of the two available segments
                    for(u32 s = 0; s < LEN_8; s++) {
                        u8 seg = segs[s];
                        // Check if that segment has already been mapped
                        if(map.reverseMapping[seg] == -1) {
                            map.mapping[value] = seg;
                            map.reverseMapping[seg] = value;
                            map.completion += 1;
                            break;
                        }
                    }
                }
            }
        }
    }

    return map;
}

static u32
get_numeric(Mapping map, Segments disp) {
    if(disp.len == LEN_1) return 1;
    if(disp.len == LEN_4) return 4;
    if(disp.len == LEN_7) return 7;
    if(disp.len == LEN_8) return 8;

    u8 segments[7] = {};
    for(u32 i = 0; i < disp.len; i++) {
        u32 mapIndex = disp.e[i] - ASCII_A;
        u32 segmentIndex = map.mapping[mapIndex];
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

    //printf("! ERROR: unknown numeric value %s\n", disp.e);

#if 0 
    if(segments[0]) { printf("  a\n"); } else { printf("\n"); }
    if(segments[1]) { printf(" b"); }
    if(segments[2]) { printf(" c\n"); } else {printf("\n"); }
    if(segments[3]) { printf("  d\n"); }
    if(segments[4]) { printf(" e"); }
    if(segments[5]) { printf(" f\n"); } else {printf("\n"); }
    if(segments[6]) { printf("  g\n"); }
#endif 
    
    return 0xCAFEBABE;
}

    u32 outputTotal = 0;
    for(u32 signalIndex = 0; signalIndex < entryCount; signalIndex++) {
        Signal signal = signals[signalIndex];
        sort_pattern(&signal);

        b32 complete = 0;
        u32 permutation = 0;
        u32 values[4] = {};
        printf("--- SIGNAL %i ---\n", signalIndex);

        do {
            Mapping map = make_mapping(signal, permutation);

            if(map.completion != 7) {
                printf("! ERROR: incomplete mapping (%i) at signal %i\n", map.completion, signalIndex);
            }

            for(u32 i = 0; i < 4; i++) {
                u32 numeric = get_numeric(map, signal.output[i]);
                if(numeric == 0xCAFEBABE) {
                    permutation += 1;
                    complete = 0;

                    if(permutation > 40) {
                        printf("FAILED TO FIND PERMUTATION for signal %i\n", signalIndex);
                        return 1;
                    }
                    break;
                } else {
                    //printf("Found numeric for %s\n", signal.output[i].e);
                    values[i] = numeric;
                    complete += 1;
                }
            }
        } while(complete < 4);

        u32 value = (values[0] * 1000) + (values[1] * 100) + (values[2] * 10) + (values[3]);

        printf("===== Value: %i\n", value);
        outputTotal += value;
        permutation = 0;
    }
#endif

