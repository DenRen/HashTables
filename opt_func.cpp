//
// Created by tester on 14.05.2020.
//

#include <cstring>
#include <immintrin.h>
#include <cstdio>
#include "opt_func.h"

int64_t HTSearch (HT_t *ht, Elem_t str) {

    int64_t id = -1;

    __int64_t code = ht->HashFunc (str) % HT_SIZE;
    if ((id = ListValSearch (&ht->lists[code], str)) >= 0) {
        //printf ("true\n");
        return id;
    }
    //printf ("false\n");
    int i = -1;
    while (++i < code)
        if (ListValSearch (&ht->lists[i], str) >= 0)
            return id;
    // i == code
    while (++i < HT_SIZE)
        if (ListValSearch (&ht->lists[i], str) >= 0)
            return id;

    return -1;
}

int64_t ListValSearch (List_t *list, Elem_t val) {
//    static __m256i templ_arr = _mm256_set1_epi16 ();
    for (size_t idx = list->head; idx != 0; idx = list->items[idx].next)
        if (!strcmp (list->items[idx].data, val))
            return idx;

    return -1;
}

int64_t ListValSearch_AVX (List_t *list, Elem_t val) {

    for (size_t idx = list->head; idx != 0; idx = list->items[idx].next)
        if (!strcmp (list->items[idx].data, val))
            return idx;

    return -1;
}

inline char *next_word (char *buf, __int8_t *length) {
    // ????????? ??????? ?????
    while (((*buf >= 'a' && *buf <= 'z') || (*buf >= 'A' && *buf <= 'Z')))
        buf++;
    // ???? ?????? ????????? ?????
    while (!((*buf >= 'a' && *buf <= 'z') || (*buf >= 'A' && *buf <= 'Z')))
        buf++;
    printf ("%s\n", buf);
    __int8_t _lenght = 1;
    while (((buf[_lenght] >= 'a' && buf[_lenght] <= 'z') ||
            (buf[_lenght] >= 'A' && buf[_lenght] <= 'Z')))
        _lenght++;

    *length = _lenght;
    return buf;

}

__int16_t *aligned_16_AVX (char *buf, __int64_t quant_words) {
    // Create empty aligned array
    auto *new_buf = (__m256i *) (aligned_alloc (32, quant_words * 16));
    for (__int64_t i = 0; i < quant_words / 16 + 1; i++)
        new_buf[i] = _mm256_set1_epi64x (0);

    __int8_t lenght = 0;
    char *word = buf;

    __int64_t i = 0;
    for (; i < quant_words / 16; i++) {

        auto *cur_part_buf = (__int16_t *) (&new_buf[i]);

        for (int j = 0; j < 16; j++) {

            word = next_word(word, &lenght);
            memcpy (&cur_part_buf[j], word, lenght); // ...hello00000000000...
        }
    }

    auto *cur_part_buf = (__int16_t *) (&new_buf[i]);
    for (int j = 0; i < quant_words; i++, j++) {

        word = next_word(word, &lenght);
        memcpy (&cur_part_buf[j], word, lenght);
    }

    free (buf);

    return (__int16_t *) new_buf;
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