#ifndef VEC_H
#define VEC_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// A fast, generic, contiguous growable array type. 
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
// The library provides the "safer" `VEC_MUT()` macro for this operation.
//
// An example: ```c
// // Declare a new `vec_t` of size 2, storing integers
// vec_t* v = vec_with_capacity(2, sizeof(int));
// 
// // Set the value of the first element to 42
// ((int*)v->data)[0] = 42;
// // Same thing with `VEC_MUT()`
// int x = 42;
// VEC_MUT(v, int, 0, x);
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
// One can also use `vec_resize()` or `vec_reserve()` before pushing many values
// onto the vector, which may force it to reallocate multiple times.
//
//
// # Safety
// The C language type system being weak and not enforcing any special rules
// for type conversion, the users of this library shall be especially careful
// with how they use the `vec_t` type.
// 
// As its goal is to provide a way to store elements generically, it 
// internally uses `void*` (which do not hold any information about the type 
// being stored, but only where it points at in memory).
// The use of void pointers is generally dicouraged and considered bad practice
// as it often leads to many memory problems and UB.
//
// Therefore, the user must abide to these (non-exhaustive) rules:
// - Never manually change the fields of the vector.
// - Always use the vector with the type it was declared with. 
// - Verify that the functions return valid pointers, values or the VEC_OK macro.
// 
// Not following these rules WILL break your code and/or make you go insane
// while trying to track down the bugs! (Aslo, feel free to suggest/add rules in
// the list above).
//
//
// # Guarantees
// The `vec_t` type defined here is quite fundamental in nature as it only consists
// in a (length, capacity, element size, pointer) quadruplet. These are not
// necessarily stored together and their order is not specified. As mentioned in 
// the usage rules defined the Safety section above, use the appropriate functions
// to modidy these.
// By design, `vec_t` is as low-overhead as the C language permits it, while trying
// to keep it as safe to use as possible.
//
// If a `vec_t` has allocated memory, then the memory it points to is on the heap.
// Its pointer points to `len` initialized, contiguous elements in order, followed 
// by `capacity - len` logically uninitialized, contiguous elements (see diagram 
// above for a better visualization).
//
// The `vec_t` type will never automatically shrink itself, even if completely empty.
// This ensures no unnecessary allocations or deallocations occur. Emptying a vector
// and then filling it back up to the same len should not incur calls to allocator
// functions. If you wish to free up unused memory, use `vec_shrink_to_fit()`.
//
// The library's functions `vec_push()` and `vec_insert()` will never (re)allocate
// if the vector's capacity is sufficient. These will only (re)allocate if 
// `len == capacity`. The vector's capacity is completely accurate, and can be 
// relied on, provided that the user never manually modifies the vector's 
// `capacity` field.
// 
// The `vec_t` type does not guarantee any particular growth strategy when 
// reallocating, nor when `vec_reserve()` is called. The current strategy is 
// basically to only allocate space for one more element.
// One can modify the `VEC_GROWTH_FACTOR` macro down in the Macro section if it
// wishes to optimize reallocations.
// Whatever strategy is used will of course guarantee O(1) amortized push.
typedef struct vec_s {
    size_t len;
    size_t capacity;
    size_t elem_size;
    void* data;
} vec_t;


// # Implementation
// Declarations, (de)allocations, copies
vec_t* vec_new(size_t elem_size);
vec_t* vec_with_capacity(size_t capacity, size_t elem_size);
vec_t* vec_with_value(void* value, size_t len, size_t elem_size);
vec_t* vec_from_raw_parts(void* raw_ptr, size_t len, size_t elem_size);
int vec_copy(vec_t* self, vec_t* other);
int vec_inner_copy(vec_t* self, vec_t* other, size_t start, size_t end);
void vec_drop(vec_t* self);
void vec_drop_many(size_t to_drop, ...);

// Lookup
int vec_contains(vec_t* self, void* value);
int vec_search(vec_t* self, void* value);
int vec_is_empty(vec_t* self);
void* vec_peek(vec_t* self, size_t index);

// Memory management
int vec_resize(vec_t* self, size_t new_capacity);
int vec_reserve(vec_t* self, size_t additional);
int vec_shrink_to_fit(vec_t* self);
int vec_truncate(vec_t* self, size_t new_len);
int vec_clear(vec_t* self);

// Mutation
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
int vec_reverse(vec_t* self);

// # Future ideas
// vec_iter() -- Macro to iterate over a vector?
// vec_dedup() -- Remove duplicated items in the vector
// vec_retain() -- Only keep the elements that match the specified argument
// vec_rotate_right/left() -- Shift values right/left and rotate
// vec_is_sorted() -- Check if the elements are sorted
// vec_binary_search() -- Binary search on a sorted vector
// vec_sort() -- Find a good generic sort?


// # Macros
#define VEC_ERR 0
#define VEC_OK  1
#define VEC_GROWTH_FACTOR 2

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
#define VEC_MUT(vec, type, index, value)                                        \
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
