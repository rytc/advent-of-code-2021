#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <stdint.h>

typedef int8_t   s8;
typedef uint8_t  u8;
typedef int16_t  s16;
typedef uint16_t u16;
typedef int32_t  s32;
typedef int32_t  b32;
typedef float    f32;
typedef int64_t  s64;
typedef uint32_t u32;
typedef uint64_t u64;
typedef double   f64;

struct File {
    u64 length;
    u8* data;

    u64 cursor;
};

static void
free_file(File file) {
    free(file.data);
}

static u32
count_newlines(File file) {
    u32 result = 0;
    for(u32 i = 0; i < file.length; i++) {
        if(file.data[i] == '\n') result++;
    }
    return result;
}

static u32
next_newline(File file) {
    u32 tempCursor = file.cursor + 1;

    while(tempCursor < file.length) {
        if(file.data[tempCursor] == '\n') {
            return tempCursor; 
        }
        tempCursor += 1;
    }

    return tempCursor;
}

static s32
readline_s32(File *file) {
    u32 lineEnd = next_newline(*file);
    char line[128];
    memcpy(line, &file->data[file->cursor], lineEnd - file->cursor);
    s32 result = atoi(line);
    file->cursor = lineEnd;
    return result; 
}

#if OS_LINUX
#include <fcntl.h>
#include <sys/stat.h>

static File
read_entire_file(const char* file_name) {
    File result = {};
    
    s32 hdl = open(file_name, O_RDONLY);
        
    if(hdl != -1) {
        struct stat file_info;
        fstat(hdl, &file_info);

        result.length = file_info.st_size;
        result.data = (u8*)malloc(result.length * sizeof(u8));
        
        auto read_size = read(hdl, result.data, file_info.st_size);

        if(read_size < result.length) {
            printf("[OS] Failed to read file %s. Read size: %li expected size: %li\n", file_name, read_size, file_info.st_size);
        }
        
        close(hdl);
    }
    
    return result;
}

#elif OS_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

static File
read_entire_file(const char* file_name) {
    File result = {};

    // @robustness @performance handle overlapped/async IO
    HANDLE hdl = CreateFile(file_name, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL/* | FILE_FLAG_OVERLAPPED*/, NULL);

    if(hdl == INVALID_HANDLE_VALUE) {
        return result;
    }

    DWORD file_size_high;
    DWORD file_size_low = GetFileSize(hdl, &file_size_high);

    if(file_size_low == INVALID_FILE_SIZE && file_size_high == NULL) {
        printf("Failed to get file size for file %s\n", file_name);
    } else {
        result.length = file_size_low; // @todo handle file_size_high
        result.data = (u8*)malloc(result.length * sizeof(u8));
        
        OVERLAPPED ol = {0};
        DWORD bytes_read;

        BOOL success = ReadFile(hdl, result.data, result.length, &bytes_read, &ol);
        if(success == FALSE || bytes_read != result.length) {
            DWORD error = GetLastError();
            printf("Failed to read file %s of file size %llu (ERROR: %i, %i bytes read)\n", file_name, result.length, error, bytes_read);
        }
    }

    CloseHandle(hdl);

    return result;
}


#endif