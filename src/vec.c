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

// Creates a new, empty `vec_t`.
// The vector will not allocate memory until elements are pushed onto it.
//
// # Failure:
// - Returns `NULL` if the allocation of the structure fails.
//
// # Panic:
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
// # Failures:
// - Returns `NULL` if the allocation of the structure fails.
// - Returns `NULL` if the allocation of the underlying array fails.
//
// # Panic:
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

// Creates a new `vec_t` from a raw pointer.
// 
// # Safety:
// - The caller must guarantee that `raw_ptr` is a valid pointer (i.e. it 
//   must have been previously allocated with a call to `malloc()`).
// - The specified length must correspond to the number of bytes allocated. 
// - The specified element size must correspond to the size of the 
//   raw pointer original type.
//
// # Failures:
// - Returns `NULL` if the allocation of the structure fails.
// - Returns `NULL` if `raw_ptr` is not valid.
//
// # Panic:
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

// Creates an exact copy of the vector `self` into `other`.
// If `self` has not allocated any memory yet, only copies the length, capacity 
// (which SHOULD both be equal to 0) and the `elem_size` of `self`.
// Returns a `VEC_OK` if the function executed correctly.
//
// # Safety:
// - The caller must guarantee that the pointers to the underlying data
//   DO NOT overlap. This is because `vec_copy()` used `memcpy()` internaly,
//   which requires `*restrict` pointers. See `vec_inner_copy()` if the 
//   pointers happen to overlap.
//
// # Failures:
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

// TODO!
int vec_inner_copy(vec_t* self, vec_t* other, size_t start, size_t end);

// Returns 1 (true) if the vector is empty, 0 (false) otherwise.
//
// # Failure:
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
// # Safety:
// - It is NOT recommended to use the result of `vec_peak()` for tasks other
//   than "read-only" purposes.
//
// # Failure:
// - Returns `NULL` if the pointer to the underlying data is not valid.
//
// # Panic:
// - Stops the program if the specified index is equal to or greater than
//   the length of the vector.
inline
void* vec_peak(vec_t* self, size_t index) {
    if (index >= self->len) {
        printf("Error: `index` (%lu) is out of bounds", index);
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
// # Failures:
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
// # Failures:
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
// # Failures:
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

// TODO!
int vec_truncate(vec_t* self, size_t new_len);

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

    return VEC_OK;
}

// TODO!
inline
int vec_mutate(vec_t* self, void* value, size_t index);

inline
int vec_push(vec_t* self, void* item) {
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
        int ret = vec_reserve(self, 1);

        if (!ret) {
            return VEC_ERR;
        }
    }

    void* ptr = vec_offset(self, self->len);
    memcpy(ptr, item, self->elem_size);
    self->len += 1;

    return VEC_OK;
}

inline
int vec_insert(vec_t* self, void* item, size_t index) {
    if (index >= self->len) {
        printf("Error: `index` (%lu) is out of bounds", index);
        vec_drop(self);
        exit(-1);
    }

    if (!self || !self->data) {
        return VEC_ERR;
    }

    if (self->len == self->capacity) {
        int ret = vec_reserve(self, 1);

        if (!ret) {
            return VEC_ERR;
        }
    }

    void* ptr = vec_offset(self, index);
    
    memmove(ptr + self->elem_size, ptr, (self->len - index) * self->elem_size);
    memcpy(ptr, item, self->elem_size);
    self->len += 1;

    return VEC_OK;
}

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

inline
int vec_remove(vec_t* self, size_t index) {
    if (index >= self->len) {
        printf("Error: `index` (%lu) is out of bounds", index);
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

inline
int vec_swap_remove(vec_t* self, size_t index) {
    if (index >= self->len) {
        printf("Error: `index` (%lu) is out of bounds", index);
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

inline
int vec_append(vec_t* self, vec_t* other) {
    if (!self || !other || !self->data || !other->data) {
        return VEC_ERR;
    }

    if (self->len + other->len > self->capacity) {
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

inline
int vec_split_at(vec_t* self, vec_t* other, size_t index) {
    if (index >= self->len) {
        printf("Error: `index` (%lu) is out of bounds", index);
        vec_drop(self);
        exit(-1);
    }

    if (!self || !other || !self->data) {
        return VEC_ERR;
    }

    if (!other->data || other->len < self->len - index) {
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

inline
void vec_print(vec_t* self) {
    if (!self || !self->data || vec_is_empty(self)) {
        printf("[ ]\n");
        return;
    }

    printf("[");
    for (size_t i = 0; i < self->len - 1; i++) {
        printf("%d, ", ((int*)self->data)[i]);
    }
    printf("%d]\n", ((int*)self->data)[self->len - 1]);
}
