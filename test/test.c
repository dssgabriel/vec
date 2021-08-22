#include <stdio.h>

#include "../src/vec.h"

int main() {
    vec_t* v1 = vec_new(sizeof(int));
    vec_t* v2 = vec_with_capacity(2, sizeof(int));

    int x = 0; 
    int y = 1;
    int z = 2;
    int n = 42;
    int m = 9001;

    printf("inserts\n");
    vec_push(v1, &x);
    vec_push(v1, &y);
    vec_insert(v1, &n, 1);
    vec_insert(v1, &m, 2);
    vec_print(v1);

    printf("copy\n");
    vec_copy(v1, v2);
    vec_print(v1);
    vec_print(v2);

    int r; vec_pop(v1, &r);
    printf("pop: %d\n", r);
    vec_print(v1);

    printf("removes\n");
    vec_remove(v1, 2);
    vec_swap_remove(v1, 0);
    vec_print(v1);

    printf("push again\n");
    vec_push(v1, &z);
    vec_print(v1);

    printf("append\n");
    vec_print(v2);
    printf("v1.capacity = %lu, ", v1->capacity);
    vec_append(v1, v2);
    printf("v1.capacity = %lu\n", v1->capacity);
    vec_print(v1);
    vec_print(v2);

    printf("split\n");
    vec_split_at(v1, v2, 3);
    vec_print(v1);
    vec_print(v2);

    vec_drop(v1);
    vec_drop(v2);
    return 0;
}
