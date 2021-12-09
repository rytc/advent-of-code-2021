#include "../utils/utils.h"

int main(int argc, char** argv) {
    File input = read_entire_file("input.txt");

    u32 rowCount = count_newlines(input);
    u32 colCount = find_next(input, '\n');

    s8* heightmap = (s8*)malloc((rowCount*colCount)*sizeof(s8));

    u32 index = 0;
    for(u32 y = 0; y < rowCount;) {
        for(u32 x = 0; x < colCount;) {
            if(input.data[index] == '\n') { 
                index++;
                continue;
            }
            heightmap[x + (y*colCount)] = input.data[index] - 48;
            x++;
            index++;
        }
        y++;
    }

    // Map visualization
    #if 0
    for(u32 y = 0; y < rowCount; y++) {
        for(u32 x = 0; x < colCount; x++) {
            printf("%i", heightmap[x + (y*(colCount))]);
        }
        printf("\n");
    }
    #endif

    u32 riskLevels = 0;

    for(u32 y = 0; y < rowCount; y++) {
        for(u32 x = 0; x < colCount; x++) {
            u32 thisHeight = heightmap[x + (y*colCount)];

            s8 neighbors[4] = {-1, -1, -1, -1};
            
            if(y > 0)          neighbors[0] = heightmap[(x+0) + ((y-1) * colCount)];
            if(x > 0)          neighbors[1] = heightmap[(x-1) + (y * colCount)];
            if(x < colCount-1) neighbors[2] = heightmap[(x+1) + (y * colCount)];
            if(y < rowCount-1) neighbors[3] = heightmap[(x+0) + ((y+1) * colCount)];


            b32 isLowPoint = true;
            for(u32 i = 0; i < 4; i++) {
                if(neighbors[i] >= 0 && neighbors[i] <= thisHeight) {
                    isLowPoint = false;
                    break;
                }
            }

            if(isLowPoint) {
                #if 0
                printf("  %i  \n", neighbors[0]);
                printf("%i %i %i\n", neighbors[1], thisHeight, neighbors[2]);
                printf("  %i  \n\n", neighbors[3]);
                #endif
                
                riskLevels += thisHeight+1;
            }
        }
    }

    printf("\nSum of risk levels: %i\n", riskLevels);

    free_file(input);
    return 0;
}