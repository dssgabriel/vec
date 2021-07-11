#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct vec_s {
    size_t len;
    size_t capacity;
    size_t item_size;
    void* data;
} vec_t;

// Allocations
vec_t* vec_new();
vec_t* vec_with_capacity(size_t capacity);
vec_t* vec_from_raw_parts(void* raw_ptr, size_t capacity, size_t item_size);
void vec_copy_to(vec_t* self, vec_t* other);
void vec_copy_from(vec_t* self, vec_t* other);

// Informations
int vec_is_empty(vec_t* self);
void* vec_peak_front(vec_t* self);
void* vec_peak_back(vec_t* self);
void* vec_peak_at(vec_t* self, size_t index);

// Memory management
void vec_resize(vec_t* self, size_t new_capacity);

void vec_reserve(vec_t* self, size_t amount);
void vec_reserve_to(vec_t* self, size_t new_capacity);

void vec_shrink(vec_t* self, size_t amount);
void vec_shrink_to(vec_t* self, size_t new_capacity);
void vec_shrink_to_fit(vec_t* self);

void vec_clear(vec_t*);

// Mutating
void vec_push(vec_t* self, void* item);
void vec_insert_at(vec_t* self, void* item, size_t index);

void* vec_pop(vec_t* self);
void* vec_remove_at(vec_t* self, size_t index);
void* vec_swap_remove(vec_t* self, size_t index);

void vec_append(vec_t* self, vec_t* other);
void vec_split_at(vec_t* self, vec_t* other, size_t index);

// Misc
void vec_print(vec_t* self);
void vec_sort(vec_t* self);

// De-allocation
void vec_drop(vec_t* self);
