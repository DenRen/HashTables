//
// Created by tester on 14.05.2020.
//

#include <cstring>
#include <immintrin.h>
#include <cstdio>
#include <cctype>
#include <cassert>
#include <x86intrin.h>
#include "opt_func.h"

int64_t HTSearch (HT_t *ht, Elem_t str) {

    int length = strlen (str);
    __int64_t code = ht->HashFunc (str, length) % HT_SIZE;

    return ListValSearch_AVX (&ht->lists[code], ht->lists[code].size, str, length);
}

int64_t ListValSearch (List_t *list, Elem_t val) {
//    static __m256i templ_arr = _mm256_set1_epi16 ();
    for (size_t idx = list->head; idx != 0; idx = list->items[idx].next)
        if (!strcmp (list->items[idx].data, val))
            return idx;

    return -1;
}

inline __m128i word2m128i (char *word) {
    assert (strlen (word) < 16);

    __m128i _new_word = _mm_set1_epi64x (0);
    memcpy (&_new_word, word, strlen (word));

    return _new_word;
}

inline __m256i dword2m256i (char *first, char *second, int length = 16) {

    //assert (strlen (first) < 16);
    //assert (strlen (second) < 16);

    __m256i _new_word = _mm256_set1_epi64x (0);

    if (first != nullptr)                                               // TODO NEED ASM OPT
        memcpy (&_new_word, first, length);
    if (second != nullptr)
        memcpy ((__m128i *) &_new_word + 1, second, length);

    return _new_word;
}

int64_t ListValSearch_AVX (List_t *list, size_t size, Elem_t val, int length) {

    __m256i arr_val = dword2m256i (val, val, length);

    for (size_t idx = 1; idx < size - size % 2; idx += 2) {

        __m256i res = _mm256_cmpeq_epi64 (dword2m256i (list->items[idx + 0].data,
                                                       list->items[idx + 1].data),
                                          arr_val);

        if (*((char *) &res) != 0 && *((char *) &res + 8) != 0)
            return idx;

        else if (*((char *) &res + 16) != 0 && *((char *) &res + 24) != 0)
            return idx + 1;
    }

    if (size % 2)
        return !strcmp (list->items[size].data, val) ? size : -1;
    else
        return -1;

}

inline char *next_word (char *buf, __int8_t *length) {

    while (!isspace (*buf) && *buf != '\0')
        buf++;

    while (isspace (*buf) || *buf == '\0')
        buf++;

    __int8_t _lenght = 1;
    while (!isspace (buf[_lenght]) && buf[_lenght] != '\0')
        _lenght++;

    *length = _lenght;
    return buf;
}

__m128i *aligned_16_AVX (char *buf, __int64_t quant_words) {
    quant_words--;

    // Create empty aligned array
    auto *new_buf = (__m256i *) aligned_alloc (32, quant_words * 16);

    for (__int64_t i = 0; i < quant_words / 2; i++)
        new_buf[i] = _mm256_set1_epi64x (0);

    if (quant_words % 2 != 0)
        *((__m128i *) &new_buf[quant_words / 2 - 1]) = _mm_set1_epi64x (0);

    __int8_t lenght = 0;
    char *word = buf;

    __int64_t i = 0;
    for (; i < quant_words / 2; i++) {

        __m128i *cur_part_buf = (__m128i *) &new_buf[i];

        for (int j = 0; j < 2; j++) {

            word = next_word (word, &lenght);

            memcpy (&cur_part_buf[j], word, lenght);                // ...hello00000000000...

            /*for (int i1 = 0; i1 < 16; i1++) {
                printf ("%c", ((char *) &cur_part_buf[j])[i1]);
                //fflush (stdout);
            }
            printf (" %s\t", (char *) &cur_part_buf[j]);
            for (int i1 = 0; i1 < 128 * 8 * 0; i1++) {
                printf ("%c", ((char *) new_buf)[i1]);
                //fflush (stdout);
            }
            printf ("\n");*/
        }
    }

    auto *cur_part_buf = (__int8_t *) (&new_buf[i]);
    i *= 2;
    for (int j = 0; i < quant_words; i++, j++) {

        word = next_word (word, &lenght);
        memcpy (&cur_part_buf[j], word, lenght);
    }

    return (__m128i *) new_buf;
}

/*
        //if (list->items[idx].data == val)

        /*asm(
            "	.intel_syntax_noprefix\n\t"
            "	mov rsi, [v]\n\t"
            "	mov rdi, [t]\n\t"
            "	mov al, 0h\n\t"
            ".for:\n\t"
            "	cmp byte ptr rsi, rdi\n\t"
            "	jne case_ne\n\t"
            "	cmp byte ptr rsi, al\n\t"
            "	je case_e\n\t"
            "	jmp .for\n\t"
            ".case_ne:\n\t"
            "	mov r, 0h\n\t"
            "	jmp .end\n\t"
            ".case_e:\n\t"
            "	mov r, 1h\n\t\n\t"
            "	.att_syntax_prefix\n\t"
            ".end\n\t"
            "	ret\n\t"

            :"+v" (val), "+t" (temp)
            :"r" (res)
            :"%rsi", "%rdi"
        );*/
/*
if (strcmp (val, list->items[idx].data) == 0)
    return idx;
*/