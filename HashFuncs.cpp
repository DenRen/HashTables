//
// Created by tester on 14.05.2020.
//

#include <cctype>
#include <cstring>
#include <cstdio>
#include <stdexcept>
#include "HashFuncs.h"

unsigned long int EQ1 (char *str) {
    //?????????? ???????
    return 1;
}

unsigned long int FirstLetter (char *str) {
    //?????????? ASCII ??? ??????? ???????
    return (unsigned long int) tolower (str[0]);
}

unsigned long int BesouIdentity (char *str) {
    //??????? ?????
    const int A = 54059, B = 76963, C = 86969, FIRSTH = 37;
    char *s = str;
    unsigned h = FIRSTH;
    while (*s) {
        h = (h * A) ^ (s[0] * B);
        s++;
    }
    return h % C;
}

unsigned long int RSHash (char *str) {
    unsigned int b    = 378551;
    unsigned int a    = 63689;
    unsigned int hash = 0;

    for(std::size_t i = 0; i < strlen(str); i++)
    {
        hash = hash * a + str[i];
        a    = a * b;
    }

    return (hash & 0x7FFFFFFF);
}

unsigned long int JSHash (char *str) {
    unsigned int hash = 1315423911;
    for(size_t i = 0; i < strlen(str); i++)
        hash ^= ((hash << 5) + str[i] + (hash >> 2));
    return (hash & 0x7FFFFFFF);
}

unsigned long int Len (char *str) {
    //?????????? ????? ??????
    return strlen (str);
}

unsigned long int HashLy(char *str) {
    unsigned int hash = 0;

    for(; *str; str++)
        hash = (hash * 1664525) + (unsigned char)(*str) + 1013904223;

    return hash;
}

unsigned long int Sum (char *str) {
    unsigned long int code = 0;
    char *c = str;
    while (*c++ != '\0')
        code += (int) *c;
    return code;
}

unsigned long int GNU_HASH (char *str)
{
    unsigned long int h = 5381;

    for (unsigned char c = *str; c != '\0'; c = *++str)
        h = h * 33 + c;

    return h;
}

unsigned long int MURMUR (char *str) {
    char *key = str;
    unsigned int len = strlen (str);

    const unsigned int m = 0x5bd1e995;
    const unsigned int seed = 0;
    const int r = 24;

    unsigned int h = seed ^ len;

    const unsigned char * data = (const unsigned char *) key;
    unsigned int k;

    while (len >= 4) {
        k  = data [0];
        k |= data [1] << 8;
        k |= data [2] << 16;
        k |= data [3] << 24;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len -= 4;
    }

    switch (len) {
        case 3:
            h ^= data [2] << 16;
        case 2:
            h ^= data [1] << 8;
        case 1:
            h ^= data [0];
            h *= m;
    };

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return h;
}

unsigned long int rol (unsigned long int v, unsigned long shift) {
    unsigned long s =  shift >=  0 ? shift % 32 : - ((-shift) % 32);
    return (v << s) | (v >> (32 - s) );
}

unsigned long int RolHash (char *str) {
    unsigned long int code = 0;
    char *c = str;
    while (*c++ != '\0') {
        code = rol (code, 1);
        code ^= (unsigned long int) *c;
    }
    return code;
}
/*
unsigned long int CRC32 (char *str) {

    unsigned long len = strlen (str);
    unsigned long crc_table[256] = {};
    unsigned long crc = 0;
    for (int i = 0; i < 256; i++)
    {
        crc = i;
        for (int j = 0; j < 8; j++)
            crc = crc & 1 ? (crc >> 1) ^ 0xEDB88320UL : crc >> 1;
        crc_table[i] = crc;
    };
    crc = 0xFFFFFFFFUL;
    while (len--)
        crc = crc_table[(crc ^ *str++) & 0xFF] ^ (crc >> 8);
    return crc ^ 0xFFFFFFFFUL;
}

unsigned long int CRC32_opt1 (char *str) {

    unsigned long len = strlen (str);
    unsigned long crc_table[256] = {};
    unsigned long crc = 0;

    //CRC32_tablefill();

    crc = 0xFFFFFFFFUL;
    while (len--)
        crc = crc_table_opt1[(crc ^ *str++) & 0xFF] ^ (crc >> 8);
    return crc ^ 0xFFFFFFFFUL;
}

void CRC32_tablefill () {
    unsigned long crc = 0;

    for (int i = 0; i < 256; i++) {
        crc = i;
        for (int j = 0; j < 8; j++)
            crc = crc & 1 ? (crc >> 1) ^ 0xEDB88320UL : crc >> 1;
        crc_table_opt1[i] = crc;
    };
}*/
//------------------------------------------------------------------------

char *_readfile (const char *path, long *size) {
    if (path == nullptr)
        return nullptr;

    FILE *file = nullptr;
    if ((file = fopen(path, "rb")) == nullptr)
        return nullptr;

    *size = _getSizeFile (file);
    // Checking on empty pointer not needed here,
    // because previous does it

    char *buf = nullptr;
    if ((buf = (char *) calloc(*size, sizeof(char))) == nullptr)
        throw std::runtime_error("Failed to create buffer");

    if (fread(buf, sizeof(char), *size, file) != *size)
        throw std::runtime_error("Failed to read in buffer");

    fclose(file);

    return buf;
}

void _writefile(char *path, char *data, long size) {
    FILE *file = fopen(path, "wb");
    if (data == nullptr || size <= 0 || file == nullptr) {
        fclose(file);
        throw std::runtime_error("Empty args in function");
    }

    if (fwrite(data, sizeof(char), size, file) != size) {
        fclose(file);
        throw std::runtime_error("Failed to write data in file");
    }

    fclose(file);
}

long _getSizeFile(FILE *file) {
    if (file == nullptr)
        return -1;

    long size = 0;
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, 0, SEEK_SET);

    return size;
}