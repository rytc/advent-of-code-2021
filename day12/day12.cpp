#include "../utils/utils.h"

constexpr u32 MAX_CAVE_COUNT = 24;
constexpr u32 MAX_CONNECTIONS = 4;

struct Cave {
    Cave* next[MAX_CONNECTIONS];
    char id;
    u8 visitCount;
    u8 isSmall;
    u8 isEnd;
    u8 isStart;
};

char path[100];
u32 pathCount = 0;

static u32
p1_descend(Cave* cave, u32 *smallVisited) {
    u32 paths = 0;
    
    path[pathCount] = cave->id;
    pathCount++;

    if(cave->isSmall) {
        (*smallVisited) += 1;
        cave->visitCount += 1;
    }

    for(u32 i = 0; i < MAX_CONNECTIONS; i++) {
        Cave* next = cave->next[i];
        if(next) {
            if(next->isEnd) {
                path[pathCount] = next->id;
                pathCount++;
                for(u32 p = 0; p < pathCount; p++) printf("%c, ", path[p]);
                printf("end");
                pathCount--;
                if((*smallVisited) > 0) {
                    paths += 1;
                    printf(" +1\n");
                } else {
                    printf("\n");
                }
            } else { 
                if(next->isSmall && next->visitCount == 0) {
                    paths += p1_descend(next, smallVisited);
                } else if(!next->isSmall) {
                    paths += p1_descend(next, smallVisited);
                }
                
            }
        }
    }
    pathCount--;

    if(cave->isSmall) {
        (*smallVisited) -= 1;
        cave->visitCount--;
    }

    return paths;
}


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
    (*caveCount)++;
    //for(u32 i = 0; i < MAX_CONNECTIONS; i++) newCave->next[i] = nullptr;

    return newCave;
}

int main(int argc, char** argv) {
    File input = read_entire_file("input.txt");

    Cave caveList[MAX_CAVE_COUNT] = {};
    u32 caveCount = 0;

    while(input.cursor < input.length) {
        if(input.data[input.cursor] == '\n') {
            input.cursor++;
            continue;
        }
        
        u32 len = find_next(input, '-');
        len = len - input.cursor;

        Cave* first;
        Cave* second;

        if(len > 4) {
            // start
            input.cursor += len + 1;
            len = find_next(input, '\n') - input.cursor;

            Cave* cave = add_cave(input.data[input.cursor], caveList, &caveCount);
            cave->isStart = 1;
            ///printf("  %c is a starting point\n", cave->id);
            input.cursor += len;
            continue;
        } else if(len > 2) {
            // end could be defined in the first part
            input.cursor += len+1;
            len = find_next(input, '\n') - input.cursor;
            first = add_cave(input.data[input.cursor], caveList, &caveCount);
            first->isEnd = 1;
            //printf("  %c is an ending..\n", first->id);
            input.cursor += len + 1;
            continue;
        } else {
            first = add_cave(input.data[input.cursor], caveList, &caveCount);
            //printf("Added cave %c\n", first->id);
        }

        input.cursor += len+1;
        len = find_next(input, '\n') - input.cursor;

        if(len == 3) {
            // end
            first->isEnd = 1;
            //printf("  %c is an ending\n", first->id);
            input.cursor += len;
            continue;
        } else {
            second = add_cave(input.data[input.cursor], caveList, &caveCount);
            //printf("Added second cave %c\n", second->id);
        }

        input.cursor += len+1;

        // If we get here, then we have a cave connection
        for(u32 i = 0; i < MAX_CONNECTIONS; i++) {
            if(first->next[i] != nullptr) continue;
            first->next[i] = second;
            //if(second->isEnd) first->dist = 1;
            break;
        }
        

        for(u32 i = 0; i < MAX_CONNECTIONS; i++) {
            if(second->next[i] != nullptr) continue;
            second->next[i]  = first;
            break;
        }
    }

    printf("------\n");

    u32 p1PathCount = 0;
    for(u32 i = 0; i < caveCount; i++) {
        Cave *cave = &caveList[i];

        if(cave->isStart) {
            printf("Start, ", cave->id);
            u32 sv = 0;
            p1PathCount += p1_descend(cave, &sv);
            //for(u32 k = 0; k < caveCount; k++) caveList[k].visitCount = 0;
            printf("\n");
        }
    }

    printf("Part 1 path count: %i\n", p1PathCount);

#if 0
    printf("--- CAVE LIST ---\n");

    for(u32 i = 0; i < caveCount; i++) {
        Cave cave = caveList[i];
        printf("Cave %c ", cave.id);
        if(cave.isSmall) {
            printf("isSmall ");
        }
        if(cave.isEnd) {
            printf("isEnd ");
        }

        printf("Connections: ");
        for(u32 k = 0; k < MAX_CONNECTIONS; k++) {
            if(caveList[i].next[k] != nullptr) {
                Cave *t = caveList[i].next[k];
                printf("%c ", t->id);

            }
        }

        printf("\n");
    }
#endif

    free_file(input);
    return 0;
}