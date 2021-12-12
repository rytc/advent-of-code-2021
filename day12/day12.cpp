#include "../utils/utils.h"

constexpr u32 MAX_CAVE_COUNT = 24;
constexpr u32 MAX_CONNECTIONS = 4;

struct Cave {
    Cave *next[MAX_CONNECTIONS];
    char id;
    u8 visitCount;
    u8 isSmall;
    u8 isEnd;
};

static Cave* 
add_cave(char id, Cave* caveList, u32* caveCount) {
    for(u32 i = 0; i < *caveCount; i++) {
        if(caveList[i].id == id) {
            return caveList + i;
        }
    }

    Cave* newCave = caveList + (*caveCount);
    newCave->id = id;
    newCave->isSmall = (id >= 97);
    caveCount++;
    for(u32 i = 0; i < MAX_CONNECTIONS; i++) newCave->next[i] = nullptr;

    return newCave;
}

static void
print_cave(Cave cave) {
    printf("%c\n", cave.id);
    for(u32 i = 0; i < MAX_CONNECTIONS; i++) {
        if(cave.next[i] == nullptr) break;
        print_cave(*cave.next[i]);
    }
    if(cave.isEnd) printf("END\n\n");
    
}

int main(int argc, char** argv) {
    File input = read_entire_file("input.txt");

    Cave* starts[4] = {};
    Cave caveList[MAX_CAVE_COUNT] = {};
    u32 caveCount = 0;
    u32 startCount = 0;

    while(input.cursor < input.length) {
        if(input.data[input.cursor] == '\n') {
            input.cursor++;
            continue;
        }
        
        u32 len = find_next(input, '-');
        len = len - input.cursor;

        Cave* first;
        Cave* second;

        if(len > 2) {
            // start
            input.cursor += len + 1;
            len = find_next(input, '\n');

            starts[startCount] = add_cave(input.data[input.cursor], caveList, &caveCount);
            startCount++;
            input.cursor = len + 1;
            continue;
        } else {
            first = add_cave(input.data[input.cursor], caveList, &caveCount);
        }

        input.cursor += len+1;
        len = find_next(input, '\n');

        if(len == 3) {
            // end
            first->isEnd = 1;
            continue;
        } else {
            second = add_cave(input.data[input.cursor], caveList, &caveCount);
        }

        // If we get here, then we have a cave connection
        for(u32 i = 0; i < MAX_CONNECTIONS; i++) {
            if(first->next[i] != nullptr) continue;
            first->next[i] = second;
            break;
        }

        for(u32 i = 0; i < MAX_CONNECTIONS; i++) {
            if(second->next[i] != nullptr) continue;
            second->next[i] = first;
            break;
        }
    }

    for(u32 i = 0; i < startCount; i++) {
        printf("START\n");
        print_cave(*starts[i]);
    }

    free_file(input);
    return 0;
}