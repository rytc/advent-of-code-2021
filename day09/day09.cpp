#include "../utils/utils.h"

u32 rowCount;
u32 colCount;

// First naive attempt
static u32
get_basin_size(s8* heightmap, s8* checkedIndices, u32 x, u32 y) {
    u32 size = 1;

    #define CHECK(ax,ay) \
        index = ax + (ay * colCount); \
        if(heightmap[index] == 9) break; \
        if(checkedIndices[index] > 0) continue; \
        checkedIndices[index] = 1


    u32 tmpX = x;
    u32 tmpY = y;
    u32 index = 0;

    // check left
    while(tmpX > 0) {
        tmpX--;
        CHECK(tmpX, tmpY);
        size++;

        while(tmpY > 0) {
            tmpY--;
            CHECK(tmpX, tmpY);
            size++;
        }

        tmpY = y;
        while(tmpY < rowCount) {
            tmpY++;
            CHECK(tmpX, tmpY);
            size++;
        }

        tmpY = y;
    }

    tmpX = x;
    tmpY = y;

    // check right
    while(tmpX < colCount) {
        tmpX++;
        CHECK(tmpX, tmpY);
        size++;

        while(tmpY > 0) {
            tmpY--;
            CHECK(tmpX, tmpY);
            size++;
        }

        tmpY = y;
        while(tmpY < rowCount) {
            tmpY++;
            CHECK(tmpX, tmpY);
            size++;
        }

        tmpY = y;
    }

    tmpX = x;
    tmpY = y;

    // check up 
    while(tmpY > 0) {
        tmpY--;
        CHECK(tmpX, tmpY);
        size++;

        while(tmpX > 0) {
            tmpX--;
            CHECK(tmpX, tmpY);
            size++;
        }

        tmpX = x;
        while(tmpX < colCount) {
            tmpX++;
            CHECK(tmpX, tmpY);
            size++;
        }

        tmpX = x;
    }

    tmpX = x;
    tmpY = y;

    // check down
    while(tmpY < rowCount) {
        tmpY++;
        CHECK(tmpX, tmpY);
        size++;

        while(tmpX > 0) {
            tmpX--;
            CHECK(tmpX, tmpY);
            size++;
        }

        tmpX = x;
        while(tmpX < colCount) {
            tmpX++;
            CHECK(tmpX, tmpY);
            size++;
        }

        tmpX = x;
    }

    return size;
}

static u32 
floodfill(s8* heightmap, s8* checkedIndices, s32 x, s32 y) {
    if(x < 0 || x >= colCount)
        return 0;
    if(y < 0 || y >= rowCount)
        return 0;

    u32 index = x + (y * colCount);
    if(checkedIndices[index] > 0)
        return 0;

    if(heightmap[index] == 9)
        return 0;

    u32 result = 1;
    checkedIndices[index] = 1;

    result += floodfill(heightmap, checkedIndices, x, y-1);
    result += floodfill(heightmap, checkedIndices, x-1, y);
    result += floodfill(heightmap, checkedIndices, x+1, y);
    result += floodfill(heightmap, checkedIndices, x, y+1);

    return result;
}

// Based floodfill attempt
static u32
get_basin_size2(s8* heightmap, s8* checkedIndices, s32 x, s32 y) {
    return floodfill(heightmap, checkedIndices, x, y);
}


int main(int argc, char** argv) {
    File input = read_entire_file("input.txt");

    rowCount = count_newlines(input);
    colCount = find_next(input, '\n');

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

    // 
    // Part 1
    //

    u32 riskLevels = 0;
    u32 lowPointCount = 0;
    u32 lowPointList[250] = {};

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
                lowPointList[lowPointCount] = x + (y*colCount);
                lowPointCount++; 
                riskLevels += thisHeight+1;
            }
        }
    }

    printf("\nSum of risk levels: %i over %i points\n", riskLevels, lowPointCount);

    //
    // Part 2
    //
    s8* checkedIndices = (s8*)calloc(colCount*rowCount, sizeof(s8));
    u32 largestBasins[3] = {};

    for(s32 y = 0; y < rowCount; y++) {
        for(s32 x = 0; x < colCount; x++) {
            u32 thisHeight = heightmap[x + (y*colCount)];
            
            if(thisHeight == 9) continue;
            if(checkedIndices[x + (y+colCount)] > 0) continue;

            u32 basinSize = get_basin_size2(heightmap, checkedIndices, x, y);
            if(basinSize == 0) continue;



            //printf("Basin size: %i\n", basinSize);
            for(u32 i = 0; i < 3; i++) {
                if(largestBasins[i] < basinSize) {
                    // See if we should keep this size, because it's bigger than the rest
                    for(u32 j = 0; j < 3; j++) {
                        if(largestBasins[i] > largestBasins[j]) {
                            largestBasins[j] = largestBasins[i];
                            break;
                        }
                    }
                    largestBasins[i] = basinSize;
                    break;
                }
            }
        }
    }

#if 1 
    for(u32 my = 0; my < rowCount; my++) {
        for(u32 mx = 0; mx < colCount; mx++) {
            u32 mindex = mx + (my*(colCount));
            if(checkedIndices[mindex] == 0) {
                printf("%i", heightmap[mindex]);
            } else  {
                printf("-");
            }
        }
        printf("\n");
    }
#endif

    printf("Largest basins: %i %i %i\n", largestBasins[0], largestBasins[1], largestBasins[2]);
    u32 basinSizeProduct = largestBasins[0] * largestBasins[1] * largestBasins[2];
    printf("Largest basins product: %i\n", basinSizeProduct);

    free(checkedIndices);
    free(heightmap);
    free_file(input);
    return 0;
}