#ifndef VEC_H
#define VEC_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define VEC_ERR 0
#define VEC_OK  1

// A fast, generic, contiguous growable array type. 
// It's written as `vec_t` but pronounced 'vector'!
//
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
// # Safety and guarantees
typedef struct vec_s {
    size_t len;
    size_t capacity;
    size_t elem_size;
    void* data;
} vec_t;


// # Implementation
// Declarations, allocations, copies
vec_t* vec_new(size_t elem_size);
vec_t* vec_with_capacity(size_t capacity, size_t elem_size);
vec_t* vec_from_raw_parts(void* raw_ptr, size_t len, size_t elem_size);
int vec_copy(vec_t* self, vec_t* other);
int vec_inner_copy(vec_t* self, vec_t* other, size_t start, size_t end);
void vec_drop(vec_t* self);

// Lookup
int vec_contains(vec_t* self, void* value);
int vec_is_empty(vec_t* self);
void* vec_peak(vec_t* self, size_t index);

// Memory management
int vec_resize(vec_t* self, size_t new_capacity);
int vec_reserve(vec_t* self, size_t additional);
int vec_shrink_to_fit(vec_t* self);
int vec_truncate(vec_t* self, size_t new_len);
int vec_clear(vec_t* self);

// Mutating
int vec_push(vec_t* self, void* elem);
int vec_insert(vec_t* self, void* elem, size_t index);
int vec_pop(vec_t* self, void* ret);
int vec_delete(vec_t* self, size_t index);
int vec_remove(vec_t* self, void* ret, size_t index);
int vec_swap_delete(vec_t* self, size_t index);
int vec_swap_remove(vec_t* self, void* ret, size_t index);
int vec_append(vec_t* self, vec_t* other);
int vec_split_at(vec_t* self, vec_t* other, size_t index);
int vec_swap(vec_t* self, size_t index1, size_t index2);

// # Future ideas
// vec_cut()
// vec_deref_slice_mut()
// vec_iter()
// vec_search()
// vec_dedup()
// vec_drain()
// vec_retain()
// vec_reverse()
// vec_rotate_right/left()
// vec_is_sorted()
// vec_binary_search()
// vec_sort()


// # Macros

// Mutates the element at the specified index, assigning it `value`.
//
// # Safety
// - The caller must guarantee that the specified type corresponds to the 
//   type of the specified value. Both the type and the value must be the 
//   same size as the vector `elem_size`.
//
// # Failures
// - Returns a `VEC_ERR` if `self` is not a valid pointer.
// - Returns a `VEC_ERR` if the underlying data of the vector is not
//   a valid pointer.
//
// # Panic
// - Stops the program if the specified index is equal to or greater than
//   the length of the vector.
#define VEC_DEREF_MUT(vec, type, index, value)                                  \
{                                                                               \
    if (!vec || !vec->data) {                                                   \
        return VEC_ERR;                                                         \
    }                                                                           \
                                                                                \
    if (index >= vec->len || index < 0) {                                       \
        printf("Error: index out of bounds, `len` is %lu but `index` is %u\n",  \
            vec->len,                                                           \
            index                                                               \
        );                                                                      \
        vec_drop(vec);                                                          \
        exit(-1);                                                               \
    }                                                                           \
                                                                                \
    ((type*)vec->data)[index] = value;                                          \
}

// Prints a vector to `stdout`.
#define VEC_PRINT(vec, type)                                    \
{                                                               \
    if (!vec || !vec->data || vec_is_empty(vec)) {              \
        printf("[ ]\n");                                        \
    } else {                                                    \
        printf("[");                                            \
        for (size_t i = 0; i < vec->len - 1; i++) {             \
            printf("%d, ", ((type*)vec->data)[i]);              \
        }                                                       \
        printf("%d]\n", ((type*)vec->data)[vec->len - 1]);      \
    }                                                           \
} 

#endif
