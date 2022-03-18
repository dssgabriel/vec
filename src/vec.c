#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vec.h"

// This function is local to this file and is not available to users of this
// library. Therefore, it does not make any safety checks and assumes the
// caller of the function to have done so in advance.
//
// Returns a void pointer to the underlying data, from the specified offset.
static inline
void* vec_offset(vec_t* self, size_t offset) {
    return self->data + offset * self->elem_size;
}

// Deallocates the memory for the vector.
inline
void vec_drop(vec_t* self) {
    free(self->data);
    free(self);
}

// Deallocates the memory for the specified numbers of vectors to drop.
// The user must specify the number of vectors to drop, then the vectors
// themselves.
inline
void vec_drop_many(size_t to_drop, ...) {
    va_list args;
    va_start(args, to_drop);

    for (size_t i = 0; i < to_drop; i++) {
        vec_t* self = va_arg(args, vec_t*);

        free(self->data);
        free(self);
    }

    va_end(args);
}

// Creates a new, empty `vec_t`.
// The vector will not allocate memory until elements are pushed onto it.
//
// # Failure
// - Returns `NULL` if the allocation of the structure fails.
//
// # Panic
// - Stops the program if the specified element size is 0.
vec_t* vec_new(size_t elem_size) {
    if (elem_size == 0) {
        printf("Error: element size of a `vec_t` cannot be 0\n");
        exit(-1);
    }

    vec_t* v = malloc(sizeof(vec_t));

    if (!v) {
        return NULL;
    }

    v->len = 0;
    v->capacity = 0;
    v->elem_size = elem_size;
    v->data = NULL;

    return v;
}

// Creates a new, empty `vec_t` with the specified capacity.
// The vector will be able to hold `capacity` items without needing 
// to reallocate.
// If `capacity` is 0, the function has the same behavior as `vec_new`.
//
// # Failures
// - Returns `NULL` if the allocation of the structure fails.
// - Returns `NULL` if the allocation of the underlying array fails.
//
// # Panic
// - Stops the program if the specified element size is 0.
vec_t* vec_with_capacity(size_t capacity, size_t elem_size) {
    if (elem_size == 0) {
        printf("Error: element size of a `vec_t` cannot be 0\n");
        exit(-1);
    }

    if (capacity == 0) {
        return vec_new(elem_size);
    }

    vec_t* v = malloc(sizeof(vec_t));

    if (!v) {
        return NULL;
    }

    v->len = 0;
    v->capacity = capacity;
    v->elem_size = elem_size;
    v->data = malloc(capacity * elem_size);

    if (!v->data) {
        return NULL;
    }

    return v;
}

// TODO!
vec_t* vec_with_value(void* value, size_t len, size_t elem_size) {
    if (elem_size == 0) {
        printf("Error: element size of a `vec_t` cannot be 0\n");
        exit(-1);
    }

    if (len == 0) {
        return vec_new(elem_size);
    }

    vec_t* v = malloc(sizeof(vec_t));

    if (!v) {
        return NULL;
    }

    v->len = len;
    v->capacity = len;
    v->elem_size = elem_size;
    v->data = malloc(len * elem_size);

    if (!v->data) {
        return NULL;
    }

    for (size_t i = 0; i < v->len; i++) {
        memcpy(vec_offset(v, i), value, elem_size);
    }

    if (!v->data) {
        return NULL;
    }

    return v;
}

// Creates a new `vec_t` from a raw pointer.
// 
// # Safety
// - The caller must guarantee that `raw_ptr` is a valid pointer (i.e. it 
//   must have been previously allocated with a call to `malloc()`).
// - The specified length must correspond to the number of bytes allocated. 
// - The specified element size must correspond to the size of the 
//   raw pointer original type.
//
// # Failures
// - Returns `NULL` if the allocation of the structure fails.
// - Returns `NULL` if the specified raw pointer is not valid.
//
// # Panic
// - Stops the program if the specified element size is 0.
vec_t* vec_from_raw_parts(void* raw_ptr, size_t len, size_t elem_size) {
    if (elem_size == 0) {
        printf("Error: element size of a `vec_t` cannot be 0\n");
        exit(-1);
    }

    vec_t* v = vec_with_capacity(len, elem_size);

    if (!v || !raw_ptr) {
        return NULL;
    }

    v->data = raw_ptr;

    return v;
}

// Copies the contents of the vector `self` into `other`.
// If `self` has not allocated any memory yet, only copies the length, capacity 
// (which SHOULD both be equal to 0) and the `elem_size` of `self`.
// Returns a `VEC_OK` if the function executed correctly.
//
// # Safety
// - The caller must guarantee that the pointers to the underlying data
//   DO NOT overlap. This is because `vec_copy()` used `memcpy()` internally,
//   which requires `*restrict` pointers. See `vec_inner_copy()` if the 
//   pointers happen to overlap.
//
// # Failures
// - Returns a `VEC_ERR` if `self` or `other` are not valid pointers.
// - Returns a `VEC_ERR` if the reallocation of the underlying array 
//   of `other` failed.
int vec_copy(vec_t* self, vec_t* other) {
    if (!self || !other) {
        return VEC_ERR;
    }

    other->len = self->len;
    other->capacity = self->capacity;

    if (self->data) {
        free(other->data);
        other->data = malloc(self->elem_size * self->capacity);

        if (!other->data) {
            return VEC_ERR;
        }

        memcpy(other->data, self->data, self->elem_size * self->capacity);
    }
    
    return VEC_OK;
}

// Copies a slice of the vector `self` into `other`, starting from the index
// `start` to the index `end`.
// This function should be used to create vector copies whenever the caller
// cannot guarantee that the pointers to the underlying data do not overlap.
// Returns a `VEC_OK` if the function executed correctly.
//
// # Failures
// - Returns a `VEC_ERR` if `self` or `other` are not valid pointers.
// - Returns a `VEC_ERR` if the reallocation of the underlying array 
//   of `other` failed.
//
// # Panic
// - Stops the execution of the program if the specified end index is equal
//   to or greater than the length of the vector.
int vec_inner_copy(vec_t* self, vec_t* other, size_t start, size_t end) {
    if (end >= self->len) {
        printf("Error: index out of bounds, `len` is %lu but `index` is %lu\n", 
            self->len, 
            end
        );
        vec_drop(self);
        exit(-1);
    }

    if (!self || !other) {
        return VEC_ERR;
    }

    size_t len = end - start;
    other->len = len;
    other->capacity = len;

    free(other->data);
    other->data = malloc(self->elem_size * len);

    if (!other->data) {
        return VEC_ERR;
    }

    void* ptr = vec_offset(self, start);

    memmove(other->data, ptr, len * self->elem_size);
    
    return VEC_OK;
}

// Returns 1 (true) if the vector contains the specified value,
// 0 (false) otherwise.
//
// # Failures
// - Returns a `VEC_ERR` if `self` is not a valid pointer.
// - Returns a `VEC_ERR` if the underlying data of the vector is not 
//   a valid pointer.
inline
int vec_contains(vec_t* self, void* value) {
    if (!self || !self->data) {
        return VEC_ERR;
    }

    int contains = 1;
    
    // Optimized to stop iterating when a match is found
    for (size_t i = 0; i < self->len && contains != 0; i++) {
        contains = memcmp(vec_offset(self, i), value, self->elem_size);
    }

    return contains == 0;
}

// Returns the index where the specified value was found,
// -1 otherwise.
//
// # Failures
// - Returns a `VEC_ERR` if `self` is not a valid pointer.
// - Returns a `VEC_ERR` if the underlying data of the vector is not 
//   a valid pointer.
inline
int vec_search(vec_t* self, void* value) {
    if (!self || !self->data) {
        return VEC_ERR;
    }

    int contains = 1;
    size_t i = 0;

    // Optimized to stop iterating when a match is found
    while (i < self->len && contains != 0) {
        contains = memcmp(vec_offset(self, i), value, self->elem_size);
        i += 1;
    }

    return contains == 0 ? (int)i - 1 : -1;
}

// Returns 1 (true) if the vector is empty, 0 (false) otherwise.
//
// # Failure
// - Returns a `VEC_ERR` if `self` is not a valid pointer.
inline
int vec_is_empty(vec_t* self) {
    if (!self) {
        return VEC_ERR;
    }

    return self->len == 0;
}

// Returns a pointer to the underlying data at the specified index.
//
// # Safety
// - The user should use the result of `vec_peek()` for read-only
//   purposes.
//
// # Failure
// - Returns `NULL` if the pointer to the underlying data is not valid.
//
// # Panic
// - Stops the program if the specified index is equal to or greater than
//   the length of the vector.
inline
void* vec_peek(vec_t* self, size_t index) {
    if (index >= self->len) {
        printf("Error: index out of bounds, `len` is %lu but `index` is %lu\n", 
            self->len, 
            index
        );
        vec_drop(self);
        exit(-1);
    }
    
    if (!self->data) {
        return NULL;
    }

    return vec_offset(self, index);
}

// Resizes the `vec_t` in place so that `capacity` is equal to `new_capacity`.
// Does nothing if `new_capacity` is smaller than `capacity`.
// Returns a `VEC_OK` if the function executed correctly.
//
// # Failures
// - Returns a `VEC_ERR` if `self` is not a valid pointer.
// - Returns a `VEC_ERR` if the underlying data of the vector is not 
//   a valid pointer.
// - Returns a `VEC_ERR` if the length of the vector is greater than the 
//   requested new capacity. See `vec_truncate()` in this case.
// - Returns a `VEC_ERR` if the reallocation of the underlying data of the
//   vector failed.
int vec_resize(vec_t* self, size_t new_capacity) {
    if (!self || !self->data || self->len > new_capacity) {
        return VEC_ERR;
    }

    if (self->capacity >= new_capacity) {
        return VEC_OK;
    }

    self->capacity = new_capacity;
    self->data = realloc(self->data, new_capacity * self->elem_size);

    return self->data ? VEC_OK : VEC_ERR;
}

// Reserves capacity for at least `additional` more elements to be 
// inserted/pushed onto the vector.
// Returns a `VEC_OK` if the function executed correctly.
// 
// # Failures
// - Returns a `VEC_ERR` if `self` is not a valid pointer.
// - Returns a `VEC_ERR` if the underlying data of the vector is not 
//   a valid pointer.
// - Returns a `VEC_ERR` if the reallocation of the underlying data of the
//   vector failed.
int vec_reserve(vec_t* self, size_t additional) {
    if (!self || !self->data) {
        return VEC_ERR;
    }

    self->capacity += additional;
    self->data = realloc(self->data, self->capacity * self->elem_size);

    return self->data ? VEC_OK : VEC_ERR;
}

// Shrinks the capacity of the vector so that `capacity` is equal to `len`.
// Returns a `VEC_OK` if the function executed correctly.
// 
// # Failures
// - Returns a `VEC_ERR` if `self` is not a valid pointer.
// - Returns a `VEC_ERR` if the underlying data of the vector is not 
//   a valid pointer.
// - Returns a `VEC_ERR` if the reallocation of the underlying data of the
//   vector failed.
int vec_shrink_to_fit(vec_t* self) {
    if (!self || !self->data) {
        return VEC_ERR;
    }

    self->capacity = self->len;
    self->data = realloc(self->data, self->capacity * self->elem_size);

    return self->data ? VEC_OK : VEC_ERR;
}

// Truncates the vector so that `len` and `capacity` are equal to `new_len`.
// Clears the vector if the specified new length is 0.
// Does nothing if the specified new length is greater than the current length.
// Returns a `VEC_OK` if the function executed correctly.
// 
// # Failures
// - Returns a `VEC_ERR` if `self` is not a valid pointer.
// - Returns a `VEC_ERR` if the underlying data of the vector is not 
//   a valid pointer.
// - Returns a `VEC_ERR` if the allocation of the temporary array failed.
int vec_truncate(vec_t* self, size_t new_len) {
    if (!self || !self->data) {
        return VEC_ERR;
    }

    if (new_len == 0) {
        vec_clear(self);
        return VEC_OK;
    }

    if (self->len >= new_len) {
        return VEC_OK;
    }

    void* tmp = malloc(new_len * self->elem_size);

    if (!tmp) {
        return VEC_ERR;
    }
    
    memcpy(tmp, self->data, new_len * self->elem_size);
    free(self->data);
    self->len = new_len;
    self->capacity = new_len;
    self->data = tmp;

    return self->data ? VEC_OK : VEC_ERR;
}

// Clears the vector, deallocating the memory used by the underlying data.
// Returns a `VEC_OK` if the function executed correctly.
// 
// Unlike in other libraries (let it be in C or in other languages), this 
// function deallocates memory for safety reasons.
// Removing the values of a vector and leaving the memory allocated for 
// possible future use is dangerous and can be a vulnerability exploited 
// by malwares for code injection.
//
// # Failure:
// - Returns a `VEC_ERR` if `self` is not a valid pointer.
int vec_clear(vec_t* self) {
    if (!self) {
        return VEC_ERR;
    }

    self->len = 0;
    self->capacity = 0;
    free(self->data);
    self->data = NULL;

    return VEC_OK;
}

// Pushes an element onto the vector.
// Does not reallocate memory if the length of the vector is smaller than
// its capacity.
// Returns a `VEC_OK` if the function executed correctly.
//
// # Failures
// - Returns a `VEC_ERR` if `self` is not a valid pointer.
// - Returns a `VEC_ERR` if the underlying data of the vector is not
//   a valid pointer.
// - Returns a `VEC_ERR` in case a reallocation of the underlying data of
// the vector is needed but fails.
inline
int vec_push(vec_t* self, void* elem) {
    if (!self) {
        return VEC_ERR;
    }

    if (!self->data) {
        self->capacity = 1;
        self->data = malloc(self->capacity * self->elem_size);

        if (!self->data) {
            return VEC_ERR;
        }
    }

    if (self->len == self->capacity) {
        int ret = vec_reserve(self, VEC_GROWTH_FACTOR);

        if (!ret) {
            return VEC_ERR;
        }
    }

    void* ptr = vec_offset(self, self->len);

    memcpy(ptr, elem, self->elem_size);
    self->len += 1;

    return VEC_OK;
}

// Inserts an element at the specified index.
// Does not reallocate memory if the length of the vector is smaller than
// its capacity.
// Returns a `VEC_OK` if the function executed correctly.
//
// # Failures
// - Returns a `VEC_ERR` if `self` is not a valid pointer.
// - Returns a `VEC_ERR` if the underlying data of the vector is not
//   a valid pointer.
// - Returns a `VEC_ERR` in case a reallocation of the underlying data of
// the vector is needed but fails.
//
// # Panic
// - Stops the program if the specified index is equal to or greater than
//   the length of the vector.
inline
int vec_insert(vec_t* self, void* elem, size_t index) {
    if (index >= self->len) {
        printf("Error: index out of bounds, `len` is %lu but `index` is %lu\n", 
            self->len, 
            index
        );
        vec_drop(self);
        exit(-1);
    }

    if (!self || !self->data) {
        return VEC_ERR;
    }

    if (self->len == self->capacity) {
        int ret = vec_reserve(self, VEC_GROWTH_FACTOR);

        if (!ret) {
            return VEC_ERR;
        }
    }

    void* ptr = vec_offset(self, index);
    
    memmove(ptr + self->elem_size, ptr, (self->len - index) * self->elem_size);
    memcpy(ptr, elem, self->elem_size);
    self->len += 1;

    return VEC_OK;
}

// Pops the last element off of the vector and returns it to the caller.
// The vector keeps its capacity and does not erase the value.
// Returns a `VEC_OK` if the function executed correctly.
//
// # Failures
// - Returns a `VEC_ERR` if `self` is not a valid pointer.
// - Returns a `VEC_ERR` if the underlying data of the vector is not
//   a valid pointer.
// - Returns a `VEC_ERR` if the vector is empty.
inline
int vec_pop(vec_t* self, void* ret) {
    if (!self || !self->data || vec_is_empty(self)) {
        return VEC_ERR;
    }
    
    void* ptr = vec_offset(self, self->len - 1);

    memcpy(ret, ptr, self->elem_size);
    self->len -= 1;

    return VEC_OK;
}

// Deletes the element at the specified index from the vector.
// The vector keeps its capacity and does not erase the value.
// Returns a `VEC_OK` if the function executed correctly.
//
// # Failures
// - Returns a `VEC_ERR` if `self` is not a valid pointer.
// - Returns a `VEC_ERR` if the underlying data of the vector is not
//   a valid pointer.
//
// # Panic
// - Stops the program if the specified index is equal to or greater than
//   the length of the vector.
inline
int vec_delete(vec_t* self, size_t index) {
    if (index >= self->len) {
        printf("Error: index out of bounds, `len` is %lu but `index` is %lu\n", 
            self->len, 
            index
        );
        vec_drop(self);
        exit(-1);
    }

    if (!self || !self->data) {
        return VEC_ERR;
    }

    void* ptr = vec_offset(self, index);

    memmove(ptr, ptr + self->elem_size, (self->len - index) * self->elem_size);
    self->len -= 1;

    return VEC_OK;
}

// Removes the element at the specified index from the vector and returns
// it to the caller.
// The vector keeps its capacity and does not erase the value.
// Returns a `VEC_OK` if the function executed correctly.
//
// # Failures
// - Returns a `VEC_ERR` if `self` is not a valid pointer.
// - Returns a `VEC_ERR` if the underlying data of the vector is not
//   a valid pointer.
//
// # Panic
// - Stops the program if the specified index is equal to or greater than
//   the length of the vector.
inline
int vec_remove(vec_t* self, void* ret, size_t index) {
    if (index >= self->len) {
        printf("Error: index out of bounds, `len` is %lu but `index` is %lu\n", 
            self->len, 
            index
        );
        vec_drop(self);
        exit(-1);
    }

    if (!self || !self->data) {
        return VEC_ERR;
    }

    void* ptr = vec_offset(self, index);

    memcpy(ret, ptr, self->elem_size);
    memmove(ptr, ptr + self->elem_size, (self->len - index) * self->elem_size);
    self->len -= 1;

    return VEC_OK;
}

// Deletes the element at the specified index from the vector.
// The function swaps the last element of the vector with the target
// to avoid a call to `memcpy`. Prefer this operation whenever the order
// of the elements of the vector does not matter. 
// The vector keeps its capacity and does not erase the value.
// Returns a `VEC_OK` if the function executed correctly.
//
// # Failures
// - Returns a `VEC_ERR` if `self` is not a valid pointer.
// - Returns a `VEC_ERR` if the underlying data of the vector is not
//   a valid pointer.
//
// # Panic
// - Stops the program if the specified index is equal to or greater than
//   the length of the vector.
inline
int vec_swap_delete(vec_t* self, size_t index) {
    if (index >= self->len) {
        printf("Error: index out of bounds, `len` is %lu but `index` is %lu\n", 
            self->len, 
            index
        );
        vec_drop(self);
        exit(-1);
    }

    if (!self || !self->data) {
        return VEC_ERR;
    }

    void* ptr = vec_offset(self, index);
    void* last = vec_offset(self, self->len - 1);

    memmove(ptr, last, self->elem_size);
    self->len -= 1;

    return VEC_OK;
}

// Removes the element at the specified index from the vector and returns
// it to the caller.
// The function swaps the last element of the vector with the target
// to avoid a call to `memcpy`. Prefer this operation whenever the order
// of the elements of the vector does not matter. 
// The vector keeps its capacity and does not erase the value.
// Returns a `VEC_OK` if the function executed correctly.
//
// # Failures
// - Returns a `VEC_ERR` if `self` is not a valid pointer.
// - Returns a `VEC_ERR` if the underlying data of the vector is not
//   a valid pointer.
//
// # Panic
// - Stops the program if the specified index is equal to or greater than
//   the length of the vector.
inline
int vec_swap_remove(vec_t* self, void* ret, size_t index) {
    if (index >= self->len) {
        printf("Error: index out of bounds, `len` is %lu but `index` is %lu\n", 
            self->len, 
            index
        );
        vec_drop(self);
        exit(-1);
    }

    if (!self || !self->data) {
        return VEC_ERR;
    }

    void* ptr = vec_offset(self, index);
    void* last = vec_offset(self, self->len - 1);

    memcpy(ret, ptr, self->elem_size);
    memmove(ptr, last, self->elem_size);
    self->len -= 1;

    return VEC_OK;
}

// Appends all the elements of `other` to `self`, leaving `other` empty (actually
// calls `vec_clear()` on `other`).
// If the underlying data of `self` is not a valid pointer, the function
// will allocate the necessary memory to hold the elements of `other`.
// The function might also need to reallocate memory for the underlying array
// of `self` in case it is not enough to hold all the elements of `other`.
// Returns a `VEC_OK` if the function executed correctly.
//
// # Safety
// - The caller must guarantee that the pointers to the underlying data
//   DO NOT overlap. This is because `vec_append()` uses `memcpy()` internally,
//   which requires `*restrict` pointers.
//
// # Failures
// - Returns a `VEC_ERR` if `self` or `other` are not valid pointers.
// _ Returns a `VEC_ERR` if the underlying data of `other` is not a valid pointer.
// - Returns a `VEC_ERR` if the allocation or reallocation of the underlying array 
//   of `self` failed.
inline
int vec_append(vec_t* self, vec_t* other) {
    if (!self || !other || !other->data) {
        return VEC_ERR;
    }

    if (!self->data) {
        self->len = 0;
        self->capacity = other->len;
        self->data = malloc(self->capacity * self->elem_size);

        if (!self->data) {
            return VEC_ERR;
        }
    } else if (self->len + other->len > self->capacity) {
        int ret = vec_reserve(self, other->len);

        if (!ret) {
            return VEC_ERR;
        }
    }

    void* ptr = vec_offset(self, self->len);

    memcpy(ptr, other->data, other->len * other->elem_size);
    self->len += other->len;
    vec_clear(other);

    return VEC_OK;
}

// Splits the vector `self` at the specified index and copies the rest into `other`.
// `self` will contain all the elements up to `index` (excluded): `[0..index)`.
// `other` will contain all the remaining elements of `self`: `[index..len)`.
// Returns a `VEC_OK` if the function executed correctly.
//
// # Safety
// TODO! 
// Make sure that pointers do not overlap
// Make sure that the vec_t have the same type
//
// # Failure
// - Returns a `VEC_ERR` if `self` or `other` are not valid pointers.
// _ Returns a `VEC_ERR` if the underlying data of `self` is not a valid pointer.
// - Returns a `VEC_ERR` if the allocation or reallocation of the underlying array 
//   of `other` failed.
//
// # Panic
// - Stops the program if the specified index is equal to or greater than
//   the length of `self`.
inline
int vec_split_at(vec_t* self, vec_t* other, size_t index) {
    if (index >= self->len) {
        printf("Error: index out of bounds, `len` is %lu but `index` is %lu\n", 
            self->len, 
            index
        );
        vec_drop(self);
        exit(-1);
    }

    if (!self || !other || !self->data) {
        return VEC_ERR;
    }

    if (!other->data) {
        other->capacity = self->len - index;
        other->data = malloc(other->capacity * self->elem_size);

        if (!other->data) {
            return VEC_ERR;
        }
    } else if (other->len < self->len - index) {
        int ret = vec_reserve(other, (self->len - index));

        if (!ret) {
            return VEC_ERR;
        }
    }

    void* ptr = vec_offset(self, index);

    memcpy(other->data, ptr, (self->len - index) * self->elem_size);
    other->len = self->len - index;
    self->len = index;

    return VEC_OK;
}

// Swaps the value at the specified `index1` with the one at `index2`.
// This function makes an allocation of size `self->elem_size`.
// Returns a `VEC_OK` if the function executed correctly.
//
// # Failures
// - Returns a `VEC_ERR` if `self` is not a valid pointer.
// - Returns a `VEC_ERR` if the underlying data of the vector is not
//   a valid pointer.
//
// # Panic
// - Stops the program if the specified index is equal to or greater than
//   the length of the vector.
inline
int vec_swap(vec_t* self, size_t index1, size_t index2) {
    if (index1 >= self->len || index2 >= self->len) {
        printf("Error: index out of bounds, `len` is %lu but `indexes` are %lu and %lu\n", 
            self->len, 
            index1,
            index2
        );
        vec_drop(self);
        exit(-1);
    }

    if (!self || !self->data) {
        return VEC_ERR;
    }

    void* tmp = malloc(self->elem_size);
    void* ptr1 = vec_offset(self, index1);
    void* ptr2 = vec_offset(self, index2);
    
    memcpy(tmp, ptr1, self->elem_size);
    memmove(ptr1, ptr2, self->elem_size);
    memcpy(ptr2, tmp, self->elem_size);

    free(tmp);

    return VEC_OK;
}

// Reverses the order of the elements in the vector, in place.
// Internally, this function uses the `vec_swap()` function
// implemented above.
// Returns a `VEC_OK` if the function executed correctly.
//
// # Failures
// - Returns a `VEC_ERR` if `self` is not a valid pointer.
// - Returns a `VEC_ERR` if the underlying data of the vector is not
//   a valid pointer.
// - Returns a `VEC_ERR` if `vec_swap()` failed.
int vec_reverse(vec_t* self) {
    if (!self || !self->data) {
        return VEC_ERR;
    }

    int ret;
    for (size_t i = 0; i < self->len / 2; i++) {
        ret = vec_swap(self, i, self->len - i - 1);

        if (!ret) {
            return VEC_ERR;
        }
    }

    return VEC_OK;
}
