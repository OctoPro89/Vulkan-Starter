#pragma once
#include <defines.h>

#define VEC_DEFAULT_CAPACITY 1
#define VEC_RESIZE_FACTOR 2

#define vec_create(type) \
	_vec_create(VEC_DEFAULT_CAPACITY, sizeof(type))

#define vec_reserve(type, capacity) \
	_vec_create(capacity, sizeof(type))

#define vec_pushback(arr, value, type)	\
{									\
	type temp = value;			\
	arr = _vec_pushback(arr, &temp);	\
}

#define vec_insert(arr, index, value, type) \
{									  \
	type temp = value;			  \
	arr = _vec_insert(arr, index, &temp); \
}

#define vec_insert_at(arr, index, value, type) \
{										 \
	type temp = value;			 \
	arr = _vec_insert_at(arr, index, &temp); \
}

void* _vec_create(u64 length, u64 stride);
void* _vec_resize(void* arr);
void* _vec_pushback(void* arr, const void* value_ptr);
void* _vec_insert(void* arr, u64 slot, void* value_ptr);
void* _vec_insert_at(void* arr, u64 index, void* value_ptr);
void   vec_destroy(void* arr);
void  _vec_pop(void* arr, void* dest);
void*  vec_pop_at(void* arr, u64 index, void* dest);
void*  vec_get_at(void* arr, u64 index);
void   vec_clear(void* arr);
u64    vec_capacity(void* arr);
u64    vec_length(void* arr);
u64    vec_stride(void* arr);
void   vec_length_set(void* arr, u64 value);