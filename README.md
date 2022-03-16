# `vec_t`, a generic array type for C

A generic, contiguous growable array type written in pure C, designed
to be as fast and safe as possible. 

Basically, it looks like this:
```
     len     capacity  elem_size    data
 +---------+----------+---------+---------+
 |    2    |    3     |    4    | 0x06577 |
 +---------+----------+---------+---------+
        _____________________________|
       |
       V
     Heap
   +------+------+------+
   |  42  |  69  |      |
   +------+------+------+
```


## Installation
To install the library on your computer, please use the provided Makefile:
```sh
make
sudo make install
```

When using the library in your own programs, you will only need to include
the `vec.h` file and compile with the `-lvec` option.


## Example
```c
#include "vec.h"

int main() {
    vec_t* v = vec_with_capacity(3, sizeof(int));
    vec_t* w = vec_new(sizeof(int));
    int a = 0, b;

    vec_push(v, 1);
    vec_push(v, 2);
    vec_insert(v, &a, 0);
    vec_pop(v, &b);

    vec_copy(v, w);
    
    vec_append(v, w);
    vec_split_at(v, w, 4);

    vec_drop(v);
    vec_drop(w);
    return 0;
}
```

You can also have a look at the code in `test/test.c` and run it with the
provided Makefile command:
```sh
make test
```


## Indexing
The `vec_t` type allows to access values by index.

One must be careful when mutating the vector like this, as indexing outside
of the allocated memory is undefined behavior (UB). Always cast the 
underlying data to the type you have declared it with.
The library provides the "safer" `VEC_MUT()` macro for this operation.
```c
// Set the value of the first element to 42
((int*)v->data)[0] = 42;

// Same thing with `VEC_MUT()`
VEC_MUT(v, int, 0, 42);
```


## Capacity and reallocation
The capacity of a vector is the amount of space allocated for any future 
elements that will be pushed/inserted onto the vector. This is not to be 
confused with the length of a vector, which specifies the number of actual 
elements within it. If a vector’s length exceeds its capacity, it will 
automatically be increased, but its elements will have to be reallocated.
```c
vec_t* v = vec_with_capacity(1, sizeof(int));

// Pushing doesn't need reallocating (declared with a capacity of 1)
vec_push(v, 1);

// Pushing again will cause a reallocation!
vec_push(v, 2);
```

Reallocating memory can be slow, especially on large vectors. For this reason,
it is recommended to use `vec_with_capacity()` whenever possible.
One can also use `vec_resize()` or `vec_reserve()` before pushing many values
onto the vector, which may force it to reallocate multiple times.


## Safety
The C language type system being weak and not enforcing any special rules
for type conversion, the users of this library shall be especially careful
with how they use the `vec_t` type.

As its goal is to provide a way to store elements generically, it 
internally uses `void*` (which do not hold any information about the type 
being stored, but only where it points at in memory).
The use of void pointers is generally dicouraged and considered bad practice
as it often leads to many memory problems and UB.

Therefore, the user must abide to these (non-exhaustive) rules:
- Never manually change the fields of the vector.
- Always use the vector with the type it was declared with. 
- Verify that the functions return valid pointers, values or the VEC_OK macro.

Not following these rules WILL break your code and/or make you go insane
while trying to track down the bugs! (Aslo, feel free to suggest/add rules in
the list above).


## Guarantees
The `vec_t` type defined here is quite fundamental in nature as it only consists
in a (length, capacity, element size, pointer) quadruplet. These are not
necessarily stored together and their order is not specified. As mentioned in 
the usage rules defined the Safety section above, use the appropriate functions
to modidy these.
By design, `vec_t` is as low-overhead as the C language permits it, while trying
to keep it as safe to use as possible.

If a `vec_t` has allocated memory, then the memory it points to is on the heap.
Its pointer points to `len` initialized, contiguous elements in order, followed 
by `capacity - len` logically uninitialized, contiguous elements (see diagram 
above for a better visualization).

The `vec_t` type will never automatically shrink itself, even if completely empty.
This ensures no unnecessary allocations or deallocations occur. Emptying a vector
and then filling it back up to the same len should not incur calls to allocator
functions. If you wish to free up unused memory, use `vec_shrink_to_fit()`.

The library's functions `vec_push()` and `vec_insert()` will never (re)allocate
if the vector's capacity is sufficient. These will only (re)allocate if 
`len == capacity`. The vector's capacity is completely accurate, and can be 
relied on, provided that the user never manually modifies the vector's 
`capacity` field.

The `vec_t` type does not guarantee any particular growth strategy when 
reallocating, nor when `vec_reserve()` is called. The current strategy is 
basically to only allocate space for one more element.
One can modify the `VEC_GROWTH_FACTOR` macro down in the Macro section if it
wishes to optimize reallocations.
Whatever strategy is used will of course guarantee O(1) amortized push.


## Provided functions
### Declaring, droping, copying
- `vec_t* vec_new(size_t elem_size)`
- `vec_t* vec_with_capacity(size_t capacity, size_t elem_size)`
- `vec_t* vec_with_value(void* value, size_t len, size_t elem_size)`
- `vec_t* vec_from_raw_parts(void* raw_ptr, size_t len, size_t elem_size)`
- `int vec_copy(vec_t* self, vec_t* other)`
- `int vec_inner_copy(vec_t* self, vec_t* other, size_t start, size_t end)`
- `void vec_drop(vec_t* self)`
- `void vec_drop_all(size_t to_drop, ...)`

### Looking up
- `int vec_contains(vec_t* self, void* value)`
- `int vec_search(vec_t* self, void* value)`
- `int vec_is_empty(vec_t* self)`
- `void* vec_peak(vec_t* self, size_t index)`

### Managing memory
- `int vec_resize(vec_t* self, size_t new_capacity)`
- `int vec_reserve(vec_t* self, size_t additional)`
- `int vec_shrink_to_fit(vec_t* self)`
- `int vec_truncate(vec_t* self, size_t new_len)`
- `int vec_clear(vec_t* self)`

### Mutating the vector
- `int vec_push(vec_t* self, void* elem)`
- `int vec_insert(vec_t* self, void* elem, size_t index)`
- `int vec_pop(vec_t* self, void* ret)`
- `int vec_delete(vec_t* self, size_t index)`
- `int vec_remove(vec_t* self, void* ret, size_t index)`
- `int vec_swap_delete(vec_t* self, size_t index)`
- `int vec_swap_remove(vec_t* self, void* ret, size_t index)`
- `int vec_append(vec_t* self, vec_t* other)`
- `int vec_split_at(vec_t* self, vec_t* other, size_t index)`
- `int vec_swap(vec_t* self, size_t index1, size_t index2)`
- `int vec_reverse(vec_t* self)`


## Ideas for future implementations
- `vec_iter()`: A function to iterate over a vector, maybe some kind
of `FOREACH()` macro.
- `vec_rotate_right/left()`: Shifts the elements to the right/left and rotates them.
- `vec_retain()`: Only keeps the elements that match the specified argument.
- `vec_is_sorted()`: Checks that the elements of the vector are sorted.
- `vec_dedup()`: Removes consecutive repeated elements in the vector.
Removes all duplicates if the vector is sorted.
- `vec_binary_search()`: Performs a binary search on a vector 
(requires that it is sorted).
- `vec_sort()`: Sorts the vector. Needs a sorting algorithm that performs well
on any data type.

Feel free to suggest more!
