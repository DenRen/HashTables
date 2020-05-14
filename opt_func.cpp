//
// Created by tester on 14.05.2020.
//

#include <cstring>
#include <cstdio>
#include "opt_func.h"

int64_t HTSearch (HT_t *ht, Elem_t str) {

    int64_t id = -1;

    __int64_t code = ht->HashFunc (str) % HT_SIZE;
    if ((id = ListValSearch (&ht->lists[code], str)) >= 0)
        return id;

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