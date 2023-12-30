#include <vector/vector.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

typedef struct {
	u64 capacity;
	u64 length;
	u64 stride;
} vec;

void* _vec_create(u64 length, u64 stride)
{
	u64 vecSize = sizeof(vec);
	u64 arrSize = length * stride;
	void* newArr = 0;
	newArr = malloc(vecSize + arrSize);
	memset(newArr, 0, vecSize + arrSize);
	if (length == 0)
		printf("WARNING: Tried to create Vector with Length of 0!\n");
	vec* vector = newArr;
	vector->capacity = length;
	vector->length = 0;
	vector->stride = stride;
	
	return (void*)((u8*)newArr + vecSize);
}

void* _vec_resize(void* arr)
{
	u64 vecSize = sizeof(vec);
	vec* vector = (vec*)((u8*)arr - vecSize);
	if (vector->capacity == 0)
	{
		printf("WARNING: Tried to resize a Vector with 0 capacity!\n");
		return 0;
	}
	void* temp = _vec_create((VEC_RESIZE_FACTOR * vector->capacity), vector->stride);

	vector = (vec*)((u8*)arr - vecSize);
	memcpy(temp,arr,vector->length * vector->stride);

	vec_length_set(temp, vector->length);
	vec_destroy(arr);
	return temp;
}

void* _vec_pushback(void* arr, const void* value_ptr)
{
	u64 vecSize = sizeof(vec);
	vec* vector = (vec*)((u8*)arr - vecSize);
	if (vector->length >= vector->capacity)
		arr = _vec_resize(arr);
	vector = (vec*)((u8*)arr - vecSize);

	u64 addr = (u64)arr;
	addr += (vector->length * vector->stride);
	memcpy((void*)addr, value_ptr, vector->stride);
	vec_length_set(arr, vector->length + 1);
	return arr;
}

void* _vec_insert(void* arr, u64 slot, void* value_ptr)
{
	return nullptr;
}

void* _vec_insert_at(void* arr, u64 index, void* value_ptr)
{
	u64 length = vec_length(arr);
	u64 stride = vec_stride(arr);
	if (index >= length)
	{
		printf("ERROR: Index outside of bounds on Vector Pop! Length: %i, Index: %i", (int)length, (int)index);
		return arr;
	}
	if (length >= vec_capacity(arr))
		arr = _vec_resize(arr);

	u64 addr = (u64)arr;

	memcpy(
		(void*)(addr + ((index + 1) * stride)),
		(void*)(addr + (index * stride)),
		stride * (length - index)
	);

	memcpy((void*)(addr + (index * stride)), value_ptr, stride);

	vec_length_set(arr, length + 1);
	return arr;
}

void vec_destroy(void* arr)
{
	if (arr)
	{
		u64 vecSize = sizeof(vec);
		vec* vector = (vec*)((u8*)arr - vecSize);
		u64 totalSize = vecSize + vector->capacity * vector->stride;
		free(vector, totalSize);
	}
}

void _vec_pop(void* arr, void* dest)
{
	u64 length = vec_length(arr);
	u64 stride = vec_stride(arr);
	if (length < 1)
	{
		printf("WARNING: Pop attempted on Vector that is empty!");
		return;
	}

	u64 addr = (u64)arr;
	addr += ((length - 1) * stride);
	memcpy(dest, (void*)addr, stride);
	vec_length_set(arr, length - 1);
}

void* vec_pop_at(void* arr, u64 index, void* dest)
{
	u64 length = vec_length(arr);
	u64 stride = vec_stride(arr);
	if (index >= length)
	{
		printf("ERROR: Index outside of bounds on Vector Pop! Length: %i, Index: %i", (int)length, (int)index);
		return arr;
	}

	u64 addr = (u64)arr;
	memcpy(dest, (void*)(addr + (index * stride)), stride);

	// If not on the last element, cut out the entry and copy the rest inward
	if (index != length - 1)
	{
		memcpy(
			(void*)(addr + (index * stride)),
			(void*)(addr + ((index + 1) * stride)),
			stride * (length - (index - 1))
		);

		vec_length_set(arr, length - 1);
		return arr;
	}
}

void vec_clear(void* arr)
{
	vec_length_set(arr, 0);
}

u64 vec_capacity(void* arr)
{
	u64 vecSize = sizeof(vec);
	vec* vector = (vec*)((u8*)arr - vecSize);
	return vector->capacity;
}

u64 vec_length(void* arr)
{
	u64 vecSize = sizeof(vec);
	vec* vector = (vec*)((u8*)arr - vecSize);
	return vector->length;
}

u64 vec_stride(void* arr)
{
	u64 vecSize = sizeof(vec);
	vec* vector = (vec*)((u8*)arr - vecSize);
	return vector->stride;
}

void vec_length_set(void* arr, u64 value)
{
	u64 vecSize = sizeof(vec);
	vec* vector = (vec*)((u8*)arr - vecSize);
	vector->length = value;
}
