#include "Algorithm.h"

#define POLY 0xedb88320

uint32_t Algorithm::CRC32(const char *buffer, int length)
{
    int k;
    uint32_t crc = 0;

    while (length--) {
        crc ^= *buffer++;
        for (k = 0; k < 8; k++)
            crc = crc & 1 ? (crc >> 1) ^ POLY : crc >> 1;
    }

    return ~crc;
}