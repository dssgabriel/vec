#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vec.h"

static inline
void* vec_offset(vec_t* self, size_t offset) {
    return self->data + offset * self->item_size;
}

inline
void vec_drop(vec_t* self) {
    free(self->data);
    free(self);
}

vec_t* vec_new(size_t item_size) {
    vec_t* v = malloc(sizeof(vec_t));

    if (!v) {
        return NULL;
    }

    v->len = 0;
    v->capacity = 0;
    v->item_size = item_size;
    v->data = NULL;

    return v;
}

vec_t* vec_with_capacity(size_t capacity, size_t item_size) {
    vec_t* v = malloc(sizeof(vec_t));

    if (!v) {
        return NULL;
    }

    v->len = 0;
    v->capacity = capacity;
    v->item_size = item_size;
    v->data = malloc(capacity * item_size);

    if (!v->data) {
        return NULL;
    }

    return v;
}

vec_t* vec_from_raw_parts(void* raw_ptr, size_t len, size_t item_size) {
    vec_t* v = vec_with_capacity(len, item_size);

    if (!v) {
        return NULL;
    }

    v->data = raw_ptr;

    return v;
}

int vec_copy(vec_t* self, vec_t* other) {
    if (!self || !other) {
        return VEC_ERR;
    }

    other->len = self->len;
    other->capacity = self->capacity;

    free(other->data);
    other->data = malloc(self->item_size * self->capacity);

    if (!other->data) {
        return VEC_ERR;
    }

    memcpy(other->data, self->data, self->item_size * self->capacity);
    
    return VEC_OK;
}

inline
int vec_is_empty(vec_t* self) {
    if (!self) {
        return VEC_ERR;
    }

    return self->len == 0 ? 1 : 0;
}

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

int vec_resize(vec_t* self, size_t new_capacity) {
    if (!self || !self->data || self->len > new_capacity) {
        return VEC_ERR;
    }

    if (self->capacity >= new_capacity) {
        return VEC_OK;
    }

    self->capacity = new_capacity;
    self->data = realloc(self->data, new_capacity * self->item_size);

    return self->data ? VEC_OK : VEC_ERR;
}

int vec_reserve(vec_t* self, size_t amount) {
    if (!self || !self->data) {
        return VEC_ERR;
    }

    self->capacity += amount;
    self->data = realloc(self->data, self->capacity * self->item_size);

    return self->data ? VEC_OK : VEC_ERR;
}

int vec_shrink_to_fit(vec_t* self) {
    if (!self || !self->data) {
        return VEC_ERR;
    }

    self->capacity = self->len;
    self->data = realloc(self->data, self->capacity * self->item_size);

    return self->data ? VEC_OK : VEC_ERR;
}

int vec_clear(vec_t* self) {
    if (!self) {
        return VEC_ERR;
    }

    self->len = 0;
    self->capacity = 0;
    free(self->data);

    return VEC_OK;
}

int vec_push(vec_t* self, void* item) {
    if (!self) {
        return VEC_ERR;
    }

    if (!self->data) {
        self->capacity = 2;
        self->data = malloc(self->capacity * self->item_size);

        if (!self->data) {
            return VEC_ERR;
        }
    }

    if (self->len == self->capacity) {
        int ret = vec_resize(self, self->capacity * 2);

        if (!ret) {
            return VEC_ERR;
        }
    }

    void* ptr = vec_offset(self, self->len);
    memcpy(ptr, item, self->item_size);
    self->len += 1;

    return VEC_OK;
}

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
    
    memcpy(ptr + self->item_size, ptr, (self->len - index) * self->item_size);
    memcpy(ptr, item, self->item_size);
    self->len += 1;

    return VEC_OK;
}

int vec_pop(vec_t* self, void* ret) {
    if (!self || !self->data || vec_is_empty(self)) {
        return VEC_ERR;
    }
    
    void* ptr = vec_offset(self, self->len - 1);

    memcpy(ret, ptr, self->item_size);
    self->len -= 1;

    return VEC_OK;
}

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

    memcpy(ptr, ptr + self->item_size, (self->len - index) * self->item_size);
    self->len -= 1;

    return VEC_OK;
}

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

    memcpy(ptr, last, self->item_size);
    self->len -= 1;

    return VEC_OK;
}

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
    memcpy(ptr, other->data, other->len * other->item_size);
    self->len += other->len;
    vec_clear(other);

    return VEC_OK;
}

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
    memcpy(other->data, ptr, (self->len - index) * self->item_size);
    other->len = self->len - index;
    self->len = index;

    return VEC_OK;
}

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
