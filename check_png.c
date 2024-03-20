/* check_png.c by katahiromz. License: MIT */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#if 1
    typedef unsigned char uint8_t;
    typedef unsigned int uint32_t;

    uint32_t ntohl(uint32_t value)
    {
        return ((uint32_t)(uint8_t)(value >> 0)) << 24 |
               ((uint32_t)(uint8_t)(value >> 8)) << 16 |
               ((uint32_t)(uint8_t)(value >> 16)) << 8 |
               ((uint32_t)(uint8_t)(value >> 24)) << 0;
    }
#else
    #include <stdint.h>
    #include <winsock.h>
#endif

int check_png(
    const void *data, size_t dataSize,
    int *width, int *height,
    int *bit_depth, int *color_type)
{
    size_t index, next;
    uint32_t length;
    const uint8_t *chunk_data;
    const uint8_t *ptr = data;

    if (dataSize < 8 || memcmp(ptr, "\x89PNG\r\n\x1A\n", 8) != 0)
        return 0;

    for (index = 8; index < dataSize; index = next)
    {
        length = ntohl(*(uint32_t*)&ptr[index]);
        next = index + length + 4;
        if (next > dataSize)
            break;

        chunk_data = &ptr[index + 4 + 4];
        if (memcmp(&ptr[index + 4], "IHDR", 4) == 0 && length == 13)
        {
            *width = ntohl(*(uint32_t*)&chunk_data[0]);
            *height = ntohl(*(uint32_t*)&chunk_data[4]);
            *bit_depth = chunk_data[8];
            *color_type = chunk_data[9];
            return 1;
        }
    }

    return 0;
}

int main(int argc, char* argv[])
{
    FILE* fp;
    static uint8_t data[1000000];
    size_t size;
    int width, height, color_type, bit_depth;

    if (argc != 2)
    {
        printf("Usage: %s <png file>\n", argv[0]);
        return 1;
    }

    fp = fopen(argv[1], "rb");
    if (!fp)
    {
        printf("Cannot open file\n");
        return 1;
    }

    size = fread(data, 1, sizeof(data), fp);
    printf("%d\n", size);
    fclose(fp);

    width = height = bit_depth = color_type = -1;
    if (check_png(data, size, &width, &height, &bit_depth, &color_type))
    {
        printf("width: %u px, height: %u px\n", width, height);
        printf("bit_depth: %d, color_type: %d\n", bit_depth, color_type);
    }
    else
    {
        printf("invalid png\n");
    }

    return 0;
}
