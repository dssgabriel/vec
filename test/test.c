#include <stdio.h>

#include "../src/vec.h"

int main() {
    vec_t* v1 = vec_new(sizeof(int));
    vec_t* v2 = vec_with_capacity(2, sizeof(int));

    int a = 0, b = 1, c = 2, d = 3, e = 4, r;

    printf("\n:: Push/insert (0, 1, 2 & 3) ::\nBefore: v1 = ");
    VEC_PRINT(v1, int);
    vec_push(v1, &a);
    vec_push(v1, &d);
    vec_insert(v1, &b, 1);
    vec_insert(v1, &c, 2);
    printf("After:  v1 = ");
    VEC_PRINT(v1, int);

    printf("\n:: Copy (v2 = v1) ::\nBefore: v1 = ");
    VEC_PRINT(v1, int);
    printf("        v2 = ");
    VEC_PRINT(v2, int);
    vec_copy(v1, v2);
    printf("After:  v1 = ");
    VEC_PRINT(v1, int);
    printf("        v2 = ");
    VEC_PRINT(v2, int);

    printf("\n:: Pop ::\nBefore: v1 = ");
    VEC_PRINT(v1, int);
    vec_pop(v1, &r);
    printf("After:  v1 = ");
    VEC_PRINT(v1, int);
    printf("  poped value = %d\n", r);

    printf("\n:: Delete/remove (index 2 and 0) ::\nBefore: v1 = ");
    VEC_PRINT(v1, int);
    vec_delete(v1, 2);
    vec_remove(v1, &r, 0);
    printf("After:  v1 = ");
    VEC_PRINT(v1, int);
    printf("  removed value = %d\n", r);

    printf("\n:: Pushing again... ::\nBefore: v1 = ");
    VEC_PRINT(v1, int);
    vec_push(v1, &e);
    printf("After:  v1 = ");
    VEC_PRINT(v1, int);

    printf("\n:: Append (v2 to v1) ::\nBefore: v1 = ");
    VEC_PRINT(v1, int);
    printf("        v2 = ");
    VEC_PRINT(v2, int);
    vec_append(v1, v2);
    printf("After:  v1 = ");
    VEC_PRINT(v1, int);
    printf("        v2 = ");
    VEC_PRINT(v2, int);

    printf("\n:: Split (v1 at index 3) ::\nBefore: v1 = ");
    VEC_PRINT(v1, int);
    printf("        v2 = ");
    VEC_PRINT(v2, int);
    vec_split_at(v1, v2, 3);
    printf("After:  v1 = ");
    VEC_PRINT(v1, int);
    printf("        v2 = ");
    VEC_PRINT(v2, int);

    printf("\n:: Unsafe deref/macro (0, 1 & 2) ::\nBefore: v1 = ");
    VEC_PRINT(v1, int);
    ((int*)v1->data)[0] = 0;
    VEC_DEREF_MUT(v1, int, 1, b);
    VEC_DEREF_MUT(v1, int, 2, c);
    printf("After:  v1 = ");
    VEC_PRINT(v1, int);

    printf("\n:: Swap (index 0 and 2) ::\nBefore: v1 = ");
    VEC_PRINT(v1, int);
    vec_swap(v1, 0, 2);
    printf("After:  v1 = ");
    VEC_PRINT(v1, int);

    printf("\n:: Contains ::\nv1 = ");
    VEC_PRINT(v1, int);
    r = vec_contains(v1, &b);
    printf("  contains 1? : %d\n", r);
    r = vec_contains(v1, &d);
    printf("  contains 3? : %d\n", r);

    vec_drop(v1);
    vec_drop(v2);
    return 0;
}
