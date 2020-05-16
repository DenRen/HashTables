//
// Created by tester on 14.05.2020.
//

#ifndef INC_6_HASHTABLE_OPT_FUNC_H
#define INC_6_HASHTABLE_OPT_FUNC_H

#include <cstdlib>

extern const int HT_SIZE;

typedef char * Elem_t;

typedef struct {
    Elem_t data = nullptr;
    size_t prev = 0, next = 0;
} List_it;

struct List_t {
    List_it *items = nullptr;
    size_t size = 0;
    size_t head = 0, tail = 0;
    size_t free = 1;
    size_t dump_count = 1;
};

typedef struct {
    List_t  *lists = nullptr;
    size_t  *sizes = nullptr;
    __m128i *buf = nullptr;
    __uint64_t (*HashFunc) (char *str, int len) = nullptr;
} HT_t;

int64_t HTSearch (HT_t *ht, Elem_t str);

int64_t ListValSearch (List_t *list, Elem_t val);

int64_t ListValSearch_AVX (List_t *list, size_t size, Elem_t val, int length);

__m128i *aligned_16_AVX (char *buf, __int64_t size);

inline char *next_word (char *buf, __int8_t *length);
#endif //INC_6_HASHTABLE_OPT_FUNC_H
