#pragma once

#include <stddef.h>
#include <stdint.h>

typedef enum : uint16_t {
    U8, I8, U16, I16, U32, I32, U64, I64, F32, F64
} T;

typedef struct vec_s {
    size_t len;
    size_t capacity;
    uint8_t item_size;
    T data_type;
    void* data;
} vec_t;

// Allocations
vec_t* vec_new(T data_type);
vec_t* vec_with_capacity(T data_type, size_t capacity);
vec_t* vec_from_raw_parts(T data_type, void* raw_ptr, size_t len);
int vec_copy(vec_t* self, vec_t* other);

// Informations
int vec_is_empty(vec_t* self);
void* vec_peak_front(vec_t* self);
void* vec_peak_back(vec_t* self);
void* vec_peak_at(vec_t* self, size_t index);

// Memory management
int vec_resize(vec_t* self, size_t new_capacity);

int vec_reserve_by(vec_t* self, size_t amount);
int vec_reserve_to(vec_t* self, size_t new_capacity);

int vec_shrink_by(vec_t* self, size_t amount);
int vec_shrink_to(vec_t* self, size_t new_capacity);
int vec_shrink_to_fit(vec_t* self);

int vec_clear(vec_t*);

// Mutating
int vec_push(vec_t* self, void* item);
int vec_insert(vec_t* self, void* item, size_t index);

void* vec_pop(vec_t* self);
int vec_remove(vec_t* self, size_t index);
int vec_swap_remove(vec_t* self, size_t index);

int vec_append(vec_t* self, vec_t* other);
int vec_split_at(vec_t* self, vec_t* other, size_t index);

// Misc
int vec_sort(vec_t* self);

// De-allocation
void vec_drop(vec_t* self);
