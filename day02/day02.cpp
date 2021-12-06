#include "../utils/utils.h"

typedef enum {
    Up,
    Forward,
    Down,
} Move_Direction;

struct Movement {
    u16 dir;
    u16 distance;
};

struct Final_Position {
    s32 horizontal;
    s32 depth;
};

static Final_Position
get_final_position(Movement *movements, u32 movementCount) {
    Final_Position result = {};

    for(u32 i = 0; i < movementCount; i++) {
        Movement m = movements[i];
        if(m.dir == Up) {
            result.depth -= m.distance;
        } else if(m.dir == Down) {
            result.depth += m.distance;
        } else if(m.dir == Forward) {
            result.horizontal += m.distance;
        }
    }

    return result;
}

static Final_Position
get_final_position_with_aim(Movement *movements, u32 movementCount) {
    Final_Position result = {};

    s32 aim = 0;

    for(u32 i = 0; i < movementCount; i++) {
        Movement m = movements[i];
        if(m.dir == Up) {
            //result.depth -= m.distance;
            aim -= m.distance;
        } else if(m.dir == Down) {
            //result.depth += m.distance;
            aim += m.distance;
        } else if(m.dir == Forward) {
            result.horizontal += m.distance;
            result.depth += m.distance * aim;
        }
    }

    return result;
}

int main(int argc, char** argv) {
    File input = read_entire_file("input.txt");

    u32 movementCount = count_newlines(input);
    Movement *movements = (Movement*)malloc(movementCount * sizeof(Movement));

    for(u32 i = 0; i < movementCount; i++) {
        char dir[16] = {};
        char dist[16] = {};

        u32 spacePos = find_next(input, ' ');
        memcpy(dir, &input.data[input.cursor], spacePos - input.cursor);
        input.cursor = spacePos;

        u32 nlPos = find_next(input, '\n');
        memcpy(dist, &input.data[input.cursor], nlPos - input.cursor);
        input.cursor = nlPos + 1;

        movements[i].distance = atoi(dist);

        if(strncmp(dir, "forward", 7) == 0) {
            movements[i].dir = Forward;
        } else if(strncmp(dir, "up", 2) == 0) {
            movements[i].dir = Up;
        } else if(strncmp(dir, "down", 4) == 0) {
            movements[i].dir= Down;
        } else {
            printf("Unknown read! %s %s\n", dir, dist);
        }
    }

    Final_Position finalPos = get_final_position(movements, movementCount);
    Final_Position finalPosWithAim = get_final_position_with_aim(movements, movementCount);

    printf("Hor: %i Depth: %i\n", finalPos.horizontal, finalPos.depth);
    printf("With AIM: Hor: %i Depth %i\n", finalPosWithAim.horizontal, finalPosWithAim.depth);

    free_file(input);

    return 0;
}