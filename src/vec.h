#ifndef VEC_H
#define VEC_H

#include <stddef.h>
#include <stdint.h>

#define VEC_OK  1
#define VEC_ERR 0

typedef struct vec_s {
    size_t len;
    size_t capacity;
    size_t item_size;
    void* data;
} vec_t;

void vec_drop(vec_t* self);
vec_t* vec_new(size_t item_size);
vec_t* vec_with_capacity(size_t capacity, size_t item_size);
vec_t* vec_from_raw_parts(void* raw_ptr, size_t len, size_t item_size);
int vec_copy(vec_t* self, vec_t* other);

int vec_is_empty(vec_t* self);
void* vec_peak(vec_t* self, size_t index);

int vec_resize(vec_t* self, size_t new_capacity);
int vec_reserve(vec_t* self, size_t amount);
int vec_shrink_to_fit(vec_t* self);
int vec_clear(vec_t* self);

int vec_push(vec_t* self, void* item);
int vec_insert(vec_t* self, void* item, size_t index);
int vec_pop(vec_t* self, void* ret);
int vec_remove(vec_t* self, size_t index);
int vec_swap_remove(vec_t* self, size_t index);
int vec_append(vec_t* self, vec_t* other);
int vec_split_at(vec_t* self, vec_t* other, size_t index);

void vec_print(vec_t* self);

#endif
