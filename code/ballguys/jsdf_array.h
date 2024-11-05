#ifndef JSDF_ARRAY_H
#define JSDF_ARRAY_H
#include <stddef.h>

// #define DEBUG 1

#ifdef DEBUG
#include "debug.h"
#endif

#define jsdf_array_func_prototypes(type)                           \
    void type##Array_reserve(type##Array *array, size_t capacity); \
    void type##Array_free(type##Array *array);                     \
    void type##Array_push(type##Array *array, type element);       \
    type *type##Array_push_uninit(type##Array *array);             \
    type *type##Array_at(type##Array *array, size_t index);

#define jsdf_array(type)       \
    typedef struct type##Array \
                               \
    {                          \
        type *data;            \
        size_t capacity;       \
        size_t length;         \
    } type##Array;             \
    jsdf_array_func_prototypes(type)

// macro to create the reserve function which allocates memory for the array data
// the array object itself must be allocated separately and passed to this function
// with error checking in DEBUG mode
#ifdef DEBUG
#define jsdf_array_reserve_macro(type, malloc)                                  \
    void type##Array_reserve(type##Array *array, size_t capacity)               \
    {                                                                           \
        assertf(array != NULL, #type "Array: array is NULL");                   \
        assertf(capacity > 0, #type "Array: capacity must be greater than 0");  \
        array->data = malloc(sizeof(type) * capacity);                          \
        assertf(array->data != NULL, #type "Array: failed to allocate memory"); \
        array->length = 0;                                                      \
        array->capacity = capacity;                                             \
    }

#else
#define jsdf_array_reserve_macro(type, malloc)                    \
    void type##Array_reserve(type##Array *array, size_t capacity) \
    {                                                             \
        array->data = malloc(sizeof(type) * capacity);            \
        array->length = 0;                                        \
        array->capacity = capacity;                               \
    }
#endif

// macro to create array free function
#define jsdf_array_free_macro(type, free)     \
    void type##Array_free(type##Array *array) \
    {                                         \
        free(array->data);                    \
        array->data = NULL;                   \
        array->capacity = 0;                  \
        array->length = 0;                    \
    }

// macro to create array push function
// with bounds checking in DEBUG mode
#ifdef DEBUG
#define jsdf_array_push_macro(type)                                                                     \
    void type##Array_push(type##Array *array, type element)                                             \
    {                                                                                                   \
        assertf(array != NULL, #type "Array: array is NULL");                                           \
        assertf(array->length < array->capacity, #type "Array: failed to push element, array is full"); \
        array->data[array->length++] = element;                                                         \
    }
#else
#define jsdf_array_push_macro(type)                         \
    void type##Array_push(type##Array *array, type element) \
    {                                                       \
        array->data[array->length++] = element;             \
    }
#endif

// macro to create function to allocate item in array and return a pointer to it
// with bounds checking in DEBUG mode
#ifdef DEBUG
#define jsdf_array_push_uninit_macro(type)                                               \
    type *type##Array_push_uninit(type##Array *array)                                    \
    {                                                                                    \
        assertf(array != NULL, #type "Array: array is NULL");                            \
        assertf(array->length < array->capacity, #type "Array: failed to push element"); \
        return &array->data[array->length++];                                            \
    }
#else
#define jsdf_array_push_uninit_macro(type) \
    type *type##Array_push_uninit(type##Array *array) { return &array->data[array->length++]; }
#endif

// macro to create array at function
// with bounds checking in DEBUG mode
#ifdef DEBUG
#define jsdf_array_at_macro(type)                                           \
    type *type##Array_at(type##Array *array, size_t index)                  \
    {                                                                       \
        assertf(array != NULL, #type "Array: array is NULL");               \
        assertf(index < array->length, #type "Array: index out of bounds"); \
        return &array->data[index];                                         \
    }
#else
#define jsdf_array_at_macro(type)                          \
    type *type##Array_at(type##Array *array, size_t index) \
    {                                                      \
        return &array->data[index];                        \
    }
#endif

#define jsdf_array_foreach_ptr(v, var, iter)                            \
    if ((v)->length > 0)                                                \
        for ((iter) = 0;                                                \
             (iter) < (v)->length && (((var) = &(v)->data[(iter)]), 1); \
             ++(iter))

#define jsdf_array_funcs(type, malloc, free)       \
    jsdf_array_reserve_macro(type, malloc)         \
        jsdf_array_free_macro(type, free)          \
            jsdf_array_push_macro(type)            \
                jsdf_array_push_uninit_macro(type) \
                    jsdf_array_at_macro(type)

#endif // JSDF_ARRAY_H
