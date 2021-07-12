#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vec.h"

void vec_drop(vec_t* self) {
    free(self->data);
    free(self);
}

vec_t* vec_new(type_e data_type) {
    vec_t* v = malloc(sizeof(vec_t));

    if (v) {
        v->len = 0;
        v->capacity = 1;
        v->data_type = data_type;

        switch (data_type) {
            case U8:
                v->item_size = sizeof(uint8_t);
                break;
            case I8:
                v->item_size = sizeof(int8_t);
                break;
            case U16:
                v->item_size = sizeof(uint16_t);
                break;
            case I16:
                v->item_size = sizeof(int16_t);
                break;
            case U32:
                v->item_size = sizeof(uint32_t);
                break;
            case I32:
                v->item_size = sizeof(int32_t);
                break;
            case U64:
                v->item_size = sizeof(uint64_t);
                break;
            case I64:
                v->item_size = sizeof(int64_t);
                break;
            case F32:
                v->item_size = sizeof(float);
                break;
            case F64:
                v->item_size = sizeof(double);
                break;
            case BYTE_PTR:
                v->item_size = sizeof(uint8_t*);
                break;
            default:
                return NULL;
        }

        v->data = malloc(v->item_size);

        if (v->data) {
            return v;
        } else {
            vec_drop(v);
            return NULL;
        }
    } else {
        return NULL;
    }
}

vec_t* vec_with_capacity(type_e data_type, size_t capacity) {
    vec_t* v = vec_new(data_type);

    if (v) {
        v->capacity = capacity;
        v->data = malloc(capacity * v->item_size);

        if (v->data) {
            return v;
        } else {
            vec_drop(v);
            return NULL;
        }
    } else {
        return NULL;
    }
}

// TODO: Check v pointer, re-think raw_ptr section (ask yaspr), add data_type
vec_t* vec_from_raw_parts(type_e data_type, void* raw_ptr, size_t len, size_t item_size) {
    vec_t* v = vec_new(data_type);

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

int vec_copy_to(vec_t* self, vec_t* other) {
    if (!self || !other) {
        vec_drop(self);
        vec_drop(other);
        return -1;
    }

    other->len = self->len;
    other->capacity = self->capacity;
    other->item_size = self->item_size;

    free(other->data);
    other->data = malloc(self->item_size * self->capacity);

    if (!other->data) {
        vec_drop(self);
        vec_drop(other);
        return -1;
    } else {
        memcpy(other->data, self->data, self->item_size * self->capacity);
        return 0;
    }
}

int vec_copy_from(vec_t* self, vec_t* other) {
    if (!self || !other) {
        vec_drop(self);
        vec_drop(other);
        return -1;
    }

    self->len = other->len;
    self->capacity = other->capacity;
    self->item_size = other->item_size;

    free(self->data);
    self->data = malloc(other->item_size * other->capacity);

    if (!self->data) {
        vec_drop(self);
        vec_drop(other);
        return -1;
    } else {
        memcpy(self->data, other->data, other->item_size * other->capacity);
        return 0;
    }
}

int vec_is_empty(vec_t* self) {
    return self->len == 0 ? 1 : 0;
}

void* vec_peak_front(vec_t* self) {
    if (self->data) {
        return &self->data[0];
    } else {
        vec_drop(self);
        return NULL;
    }
}

void* vec_peak_back(vec_t* self) {
    if (self->data) {
        return &self->data[self->len - 1];
    } else {
        vec_drop(self);
        return NULL;
    }
}

void* vec_peak_at(vec_t* self, size_t index) {
    if (index < self->len) {
        if (self->data) {
            return &self->data[self->len - 1];
        } else {
            vec_drop(self);
            return NULL;
        }
    } else {
        printf("Error: `index` is out of bounds");
        vec_drop(self);
        exit(-1);
    }
}

int vec_resize(vec_t* self, size_t new_capacity) {
    if (self) {
        if (self->data) {
            if (self->capacity < new_capacity) {
                self->len = new_capacity;
            }

            self->capacity = new_capacity;
            self->data = realloc(self->data, new_capacity * self->item_size);

            return 0;

            if (!self->data) {
                vec_drop(self);
                return -1;
            }
        } else {
            vec_drop(self);
            return -1;
        }
    } else {
        return -1;
    }
}
