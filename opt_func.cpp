//
// Created by tester on 14.05.2020.
//

#include <cstring>
#include <cstdio>
#include "opt_func.h"

bool HTSearch (HT_t *ht, Elem_t str) {

    size_t temp = 0;
    for (int i = 0; i < HT_SIZE; i++)
        if (ListValSearch (&ht->lists[i], str) >= 0)
            return true;

    return false;

}

int64_t ListValSearch (List_t *list, Elem_t val) {

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