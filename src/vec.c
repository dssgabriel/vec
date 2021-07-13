#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "vec.h"

static const size_t type_size[10] = {
    sizeof(uint8_t),
    sizeof(int8_t),
    sizeof(uint16_t),
    sizeof(int16_t),
    sizeof(uint32_t),
    sizeof(int32_t),
    sizeof(uint64_t),
    sizeof(int64_t),
    sizeof(float),
    sizeof(double)
};

void vec_drop(vec_t* self) {
    free(self->data);
    free(self);
}

vec_t* vec_new(T data_type) {
    vec_t* v = malloc(sizeof(vec_t));

    if (v) {
        v->len = 0;
        v->capacity = 1;
        v->data_type = data_type;

        if (data_type > U8 && data_type <= F64) {
            v->item_size = type_size[data_type];
        }

        v->data = malloc(v->item_size);

        if (v->data) {
            return v;
        } else {
            return NULL;
        }
    } else {
        return NULL;
    }
}

vec_t* vec_with_capacity(T data_type, size_t capacity) {
    vec_t* v = vec_new(data_type);

    if (v) {
        v->capacity = capacity;
        v->data = malloc(capacity * v->item_size);

        if (v->data) {
            return v;
        } else {
            return NULL;
        }
    } else {
        return NULL;
    }
}

vec_t* vec_from_raw_parts(T data_type, void* raw_ptr, size_t len) {
    vec_t* v = vec_new(data_type);

    if (v) {
        if (raw_ptr) {
            v->len = len;
        } else {
            v->len = 0;
        }

        v->capacity = len;
        v->data = raw_ptr;

        return v;
    } else {
        return NULL;
    }
}

int vec_copy(vec_t* self, vec_t* other) {
    if (!self || !other) {
        return -1;
    }

    other->len = self->len;
    other->capacity = self->capacity;
    other->item_size = self->item_size;

    free(other->data);
    other->data = malloc(self->item_size * self->capacity);

    if (other->data) {
        memcpy(other->data, self->data, self->item_size * self->capacity);
        return 0;
    } else {
        return -2;
    }
}

int vec_is_empty(vec_t* self) {
    if (self) {
        return self->len == 0 ? 1 : 0;
    } else {
        return -1;
    }
}

void* vec_peak_front(vec_t* self) {
    if (self->data) {
        return &self->data[0];
    } else {
        return NULL;
    }
}

void* vec_peak_back(vec_t* self) {
    if (self->data) {
        return &self->data[self->len - 1];
    } else {
        return NULL;
    }
}

void* vec_peak_at(vec_t* self, size_t index) {
    if (index < self->len) {
        if (self->data) {
            return &self->data[index];
        } else {
            return NULL;
        }
    } else {
        printf("Error: `index` (%lu) is out of bounds", index);
        vec_drop(self);
        exit(-1);
    }
}

int vec_resize(vec_t* self, size_t new_capacity) {
    if (self) {
        if (self->data) {
            if (self->capacity > new_capacity) {
                self->len = new_capacity;
            }

            self->capacity = new_capacity;
            self->data = realloc(self->data, new_capacity * self->item_size);

            return (self->data != NULL) ? 0 : -3;
        } else {
            return -2;
        }
    } else {
        return -1;
    }
}

int vec_reserve_by(vec_t* self, size_t amount) {
    if (self) {
        if (self->data) {
            self->capacity += amount;
            self->data = realloc(self->data, self->capacity * self->item_size);

            return (self->data != NULL) ? 0 : -3;
        } else {
            return -2;
        }
    } else {
        return -1;
    }
}

int vec_reserve_to(vec_t* self, size_t new_capacity) {
    if (self) {
        if (self->data) {
            if (self->capacity < new_capacity) {
                self->capacity = new_capacity;
                self->data = realloc(self->data, self->capacity * self->item_size);

                return (self->data != NULL) ? 0 : -3;
            } else {
                return -4;
            }
        } else {
            return -2;
        }
    } else {
        return -1;
    }
}

int vec_shrink_by(vec_t* self, size_t amount) {
    if (self) {
        if (self->data) {
            self->capacity -= amount;
            self->data = realloc(self->data, self->capacity * self->item_size);

            return (self->data != NULL) ? 0 : -3;
        } else {
            return -2;
        }
    } else {
        return -1;
    }
}

int vec_shrink_to(vec_t* self, size_t new_capacity) {
    if (self) {
        if (self->data) {
            if (self->capacity > new_capacity) {
                self->capacity = new_capacity;
                self->data = realloc(self->data, self->capacity * self->item_size);

                return (self->data != NULL) ? 0 : -3;
            } else {
                return -4;
            }
        } else {
            return -2;
        }
    } else {
        return -1;
    }
}

int vec_shrink_to_fit(vec_t* self) {
    if (self) {
        if (self->data) {
            self->capacity = self->len;
            self->data = realloc(self->data, self->capacity * self->item_size);

            return (self->data != NULL) ? 0 : -3;
        } else {
            return -2;
        }
    } else {
        return -1;
    }
}

int vec_clear(vec_t* self) {
    if (self) {
        self->len = 0;
        self->capacity = 0;
        free(self->data);

        return 0;
    } else {
        return -1;
    }
}

int vec_push(vec_t* self, void* item) {
    if (self) {
        if (self->data) {
            if (self->capacity == self->len) {
                int ret = vec_resize(self, self->capacity * 2);

                if (!ret) {
                    return -2;
                }
            }

            void* offset = self->data + self->len;
            memcpy(offset, item, self->item_size);
            self->len += 1;

            return 0;
        } else {
            self->data = malloc(self->item_size);

            if (self->data) {
                memcpy(self->data, item, self->item_size);

                return 0;
            } else {
                return -3;
            }
        }
    } else {
        return -1;
    }
}

// int vec_insert(vec_t* self, void* item, size_t index) {}

// TODO: Needs checking
void* vec_pop(vec_t* self) {
    if (self) {
        if (self->data) {
            void* offset = self->data + self->len - self->item_size;
            void* value;
            memcpy(value, offset, self->item_size);
            offset = NULL;
            self->len -= 1;

            return value;
        } else {
            return NULL;
        }
    } else {
        return NULL;
    }
}

// int vec_remove(vec_t* self, size_t index) {}

// TODO: Needs checking
int vec_swap_remove(vec_t* self, size_t index) {
    if (self) {
        if (self->data) {
            if (index < self->len) {
                void* value;
                void* swap = self->data + self->len - self->item_size;
                memcpy(value, swap, self->item_size);
                swap = NULL;

                return 0;
            } else {
                return -3;
            }
        } else {
            return -2;
        }
    } else {
        return -1;
    }
}
