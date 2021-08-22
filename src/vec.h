#ifndef VEC_H
#define VEC_H

#include <stddef.h>
#include <stdint.h>

#define VEC_ERR 0
#define VEC_OK  1

// A contiguous growable array type, written as `vec_t` and pronounced 'vector'.
// In memory, it looks like this:
//
//      len     capacity  elem_size    data
//  +---------+----------+---------+---------+
//  |    2    |    3     |    4    | 0x06577 |
//  +---------+----------+---------+---------+
//         _____________________________|
//        |
//        V
//      Heap
//    +------+------+------+
//    |  42  |  69  |      |
//    +------+------+------+
//
//
// # Indexing
// The `vec_t` type allows to access values by index.
// 
// One must be careful when mutating the vector like this, as indexing outside
// of the allocated memory is undefined behavior (UB). Always cast the 
// underlying data to the type you have declared it with.
// The library provides the "safer" `vec_mutate()` function for this
// operation.
//
// An example: ```c
// // Declare a new `vec_t` of size 2, storing integers
// vec_t* v = vec_with_capacity(2, sizeof(int));
// 
// // Set the value of the first element to 42
// ((int*)v->data)[0] = 42;
// // Same thing with `vec_mutate()`
// int x = 42;
// vec_mutate(v, &x, 0);
// ```
//
//
// # Capacity and reallocation
// The capacity of a vector is the amount of space allocated for any future 
// elements that will be pushed/inserted onto the vector. This is not to be 
// confused with the length of a vector, which specifies the number of actual 
// elements within it. If a vector’s length exceeds its capacity, it will 
// automatically be increased, but its elements will have to be reallocated.
//
// To keep going with our example:```c
// // Pushing onto our vector doesn't need a reallocation because we declared
// // it with a size of 2 (i.e. `v->capacity = 2`)
// vec_push(v, 69);
//
// // Pushing again will cause a reallocation!
// vec_push(v, 420);
// ```
//
// Reallocating memory can be slow, especially on large vectors. For this reason,
// it is recommended to use `vec_with_capacity()` whenever possible.
// One can also use `vec_resize()` or `vec_reserve()` when pushing many values
// onto the vector, which may force it to reallocate multiple times.
//
//
// # Safety
typedef struct vec_s {
    size_t len;
    size_t capacity;
    size_t elem_size;
    void* data;
} vec_t;

// Implementations
vec_t* vec_new(size_t elem_size);
vec_t* vec_with_capacity(size_t capacity, size_t elem_size);
vec_t* vec_from_raw_parts(void* raw_ptr, size_t len, size_t elem_size);
int vec_copy(vec_t* self, vec_t* other);
// TODO!
int vec_inner_copy(vec_t* self, vec_t* other, size_t start, size_t end);

int vec_is_empty(vec_t* self);
void* vec_peak(vec_t* self, size_t index);

int vec_resize(vec_t* self, size_t new_capacity);
int vec_reserve(vec_t* self, size_t additional);
int vec_shrink_to_fit(vec_t* self);
// TODO!
int vec_truncate(vec_t* self, size_t new_len);
int vec_clear(vec_t* self);

// TODO!
int vec_mutate(vec_t* self, void* value, size_t index);
int vec_push(vec_t* self, void* elem);
int vec_insert(vec_t* self, void* elem, size_t index);
int vec_pop(vec_t* self, void* ret);
int vec_remove(vec_t* self, size_t index);
int vec_swap_remove(vec_t* self, size_t index);
int vec_append(vec_t* self, vec_t* other);
int vec_split_at(vec_t* self, vec_t* other, size_t index);

void vec_print(vec_t* self);

void vec_drop(vec_t* self);

#endif
