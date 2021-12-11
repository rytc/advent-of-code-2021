#include "../utils/utils.h"
#include <cassert>

constexpr u32 OCTOPUS_COUNT = 100;

static void
print_octopi(s8* octopi) {
    for(u32 i = 0; i < OCTOPUS_COUNT; i++) {
        if(i > 0 && i % 10 == 0) printf("\n");

        if(octopi[i] < 0) {
            printf(" (%i) ", 0);
        } else {
            printf("  %i  ", octopi[i]);
        }
    }
    printf("\n");
    //getchar();
}

static u32
flash(s8* octopi, u32 index) {
    u32 result = 1;
    s8 x = index - ((index / 10) * 10);
    s8 y = index / 10;

    //printf("Index %i is at (%i, %i)\n", index, x, y);

    for(s32 ay = y - 1; ay <= y+1; ay++) {
        for(s32 ax = x - 1; ax <= x+1; ax++) {
            if(ax < 0 || ay < 0 || ax > 9 || ay > 9 || (ax == x && ay == y)) continue;
            u32 index = ax + (ay*10);
            octopi[index] += 1;
        }
    }

    return result;
}

int main(int argc, char** argv) {
    File input = read_entire_file("input.txt");

    s8 octopi[OCTOPUS_COUNT] = {};
    u32 counter = 0;
    while(input.cursor < input.length) {
        if(input.data[input.cursor] == '\n') {
            input.cursor++;
            continue;
        }
        octopi[counter] = input.data[input.cursor] - 48;
        counter++;
        input.cursor++;
    }
    assert(counter == 100);

    u32 flashCount = 0;
    for(u32 step = 0; step < 100; step++) {
        u32 flashesThisStep = 0;

        printf("-- initial\n");
        print_octopi(octopi);

        for(u32 octoIndex = 0; octoIndex < OCTOPUS_COUNT; octoIndex++) {
            octopi[octoIndex] += 1;
        }

        printf("-- increment\n");
        print_octopi(octopi);

        u32 tmpFlashCount = 0;
        do {
            tmpFlashCount = 0;
            for(u32 octoIndex = 0; octoIndex < OCTOPUS_COUNT; octoIndex++) {
                if(octopi[octoIndex] > 9) {
                    octopi[octoIndex] = -127;
                    tmpFlashCount = flash(octopi, octoIndex);
                }
            }
        } while(tmpFlashCount > 0);
        

        printf("-- flash\n");
        print_octopi(octopi);

        for(u32 octoIndex = 0; octoIndex < OCTOPUS_COUNT; octoIndex++) {
            if(octopi[octoIndex] < 0) { 
                flashCount++;
                octopi[octoIndex] = 0;
            }
        }

        printf("-- reset\n");
        print_octopi(octopi);

        printf("step..\n");
    }


    printf("Part 1 - Flash count: %i\n", flashCount);

    free_file(input);
    return 0;
}