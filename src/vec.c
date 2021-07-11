#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vec.h"

vec_t* vec_new() {
    vec_t* v = malloc(sizeof(vec_t));

    if (v) {
        v->len = 0;
        v->capacity = 0;
        v->item_size = 1;
        v->data = NULL;

        return v;
    } else {
        printf("Error: cannot allocate memory for `vec_t`");
        exit(-1);
    }
}

void vec_drop(vec_t* self) {
    free(self->data);
    free(self);
}

vec_t* vec_with_capacity(size_t capacity) {
    vec_t* v = vec_new();

    v->capacity = capacity;
    v->data = malloc(capacity);

    if (v->data) {
        return v;
    } else {
        printf("Error: cannot allocate memory for field `data` of `vec_t`");
        vec_drop(v);
        exit(-1);
    }
}

vec_t* vec_from_raw_parts(void* raw_ptr, size_t len, size_t item_size) {
    vec_t* v = vec_new();

    if (raw_ptr) {
        v->len = len;
        v->capacity = len;
    } else {
        v->len = 0;
        v->capacity = 0;
    }

    v->item_size = item_size;
    v->data = raw_ptr;

    return v;
}

void vec_copy_to(vec_t* self, vec_t* other) {
    if (!self || !other) {
        printf("Error: `self` and/or `other` are not valid pointers");
        vec_drop(self);
        vec_drop(other);
        exit(-1);
    }

    other->len = self->len;
    other->capacity = self->capacity;
    other->item_size = self->item_size;
    other->data = realloc(other->data, self->item_size * self->capacity);

    if (other->data) {
        memcpy(other->data, self->data, self->item_size * self->capacity);
    } else {
        printf("Error: cannot reallocate memory for field `data` of `other`");
        vec_drop(self);
        vec_drop(other);
        exit(-1);
    }
}

void vec_copy_from(vec_t* self, vec_t* other) {
    if (!self || !other) {
        printf("Error: `self` and/or `other` are not valid pointers");
        vec_drop(self);
        vec_drop(other);
        exit(-1);
    }

    self->len = other->len;
    self->capacity = other->capacity;
    self->item_size = other->item_size;
    self->data = realloc(self->data, other->item_size * other->capacity);

    if (self->data) {
        memcpy(self->data, other->data, other->item_size * other->capacity);
    } else {
        printf("Error: cannot reallocate memory for field `data` of `self`");
        vec_drop(self);
        vec_drop(other);
        exit(-1);
    }
}

int vec_is_empty(vec_t* self) {
    if (self) {
        return self->len == 0 ? 1 : 0;
    } else {
        printf("Error: `self` is not a valid pointer");
        exit(-1);
    }
}

void* vec_peak_front(vec_t* self) {
    if (self) {
        if (self->data) {
            return &self->data[0];
        } else {
            printf("Error: field `data` of `self` is not a valid pointer");
            vec_drop(self);
            exit(-1);
        }
    } else {
        printf("Error: `self` is not a valid pointer");
        exit(-1);
    }
}

void* vec_peak_back(vec_t* self) {
    if (self) {
        if (self->data) {
            return &self->data[self->len - 1];
        } else {
            printf("Error: field `data` of `self` is not a valid pointer");
            vec_drop(self);
            exit(-1);
        }
    } else {
        printf("Error: `self` is not a valid pointer");
        exit(-1);
    }
}

void* vec_peak_at(vec_t* self, size_t index) {
    if (self) {
        if (index < self->len) {
            if (self->data) {
                return &self->data[self->len - 1];
            } else {
                printf("Error: field `data` of `self` is not a valid pointer");
                vec_drop(self);
                exit(-1);
            }
        } else {
            printf("Error: `index` is out of bounds");
            vec_drop(self);
            exit(-1);
        }
    } else {
        printf("Error: `self` is not a valid pointer");
        exit(-1);
    }
}
