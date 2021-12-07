#include "../utils/utils.h"


constexpr u64 MAX_FISH = 10000000000;

int main(int argc, char** argv) {
    File input = read_entire_file("input.txt");

    u64 fishCount = 0;
    //s8* fishPool = (s8*)calloc(MAX_FISH, sizeof(s8));
    //s8* fishPool = (s8*)VirtualAlloc(0, MAX_FISH*sizeof(s8), MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    u64 fishPool[9] = {};

    for(u32 i = 0; i < input.length; i++) {
        if(input.data[i] == ',') continue;
        if(input.data[i] == '\n') break;

        u32 fishAge = input.data[i] - 48; // convert ascii to real number
        fishPool[fishAge] += 1; 
    }

    for(u32 day = 0; day < 256; day++) {
        u64 dayFish[9] = {};
        memcpy(dayFish, fishPool, sizeof(u64) * 9);
        for(s32 fishGroup = 8; fishGroup >= 0; fishGroup--) {
            if(fishGroup == 0) {
                fishPool[8] += dayFish[0];
                fishPool[6] += dayFish[0];
                fishPool[0] -= dayFish[0];
            } else {
                fishPool[fishGroup] -= dayFish[fishGroup];
                fishPool[fishGroup-1] += dayFish[fishGroup];
            }
        }
    }

    for(u32 i = 0; i < 9; i++) fishCount += fishPool[i];
    printf("Fish count: %llu\n", fishCount);

    free(fishPool);
    //VirtualFree(fishPool, 0, MEM_RELEASE);
    free_file(input);
    return 0;
}