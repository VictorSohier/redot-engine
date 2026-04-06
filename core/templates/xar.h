/**************************************************************************/
/*  xar.h                                                                 */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             REDOT ENGINE                               */
/*                        https://redotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2024-present Redot Engine contributors                   */
/*                                          (see REDOT_AUTHORS.md)        */
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#pragma once

#include <algorithm>
#include <cstddef>

// Pointers to elements in this data structure are stable!
// Resource management is not automatic. User must call `free` method to
// release resources.
struct Xar {
	using Dtor = void (*)(void *);
	using Comparer = int (*)(const void *, const void *);
	using Swap = void (*)(void *, void *);
	size_t elSize;
	size_t bytes;
	void *data[36]; // 36 because the top 16 bits are reserved for
					// kernelspace/userspace identification and bottom 12 are
					// unused in page aligned pointers
					// This assumes pages are 4KB
	Xar(size_t elSize);

	// Returns a pointer to the element at index. Returns null when out of
	// bounds.
	//
	// usage:
	// ```cpp
	// *((T *)xar->at(i)) = value;
	// ```
	void *at(size_t index);
	const void *at(size_t index) const;

	// Returns a pointer to the last element. Returns null when the container
	// is empty.
	//
	// usage:
	// ```cpp
	// *((T *)xar->last()) = value;
	// ```
	void *last();
	size_t count() const;
	size_t cap() const;
	void swapRemove(size_t index, Dtor dtor);
	void orderedRemove(size_t index, Dtor dtor);

	// Returns a pointer to the new element inserted at index.
	//
	// usage:
	// ```cpp
	// *((T *)xar->insert(i)) = value;
	// ```
	void *insert(size_t index);

	// Returns a pointer to the new element pushed to the end of the xar.
	//
	// usage:
	// ```cpp
	// *((T *)xar->push()) = value;
	// ```
	void *push();

	// returns a pointer to the popped element at the end of the xar. This
	// memory may be reused later. Please copy or move the element out if you
	// want to use it later!
	//
	// usage:
	// ```cpp
	// T value = *((T *)xar->pop());
	// ```
	void *pop();

	// idx is the destination for the index where the first instance of value
	// resides. idx is optional. Returns true if the value was found.
	bool contains(size_t *idx, const void *value, Comparer cmp) const;

	// Warning: This resize does not destroy extraneous elements!
	void resize(size_t elemCount);
	void sort(Comparer cmp, Swap swap);
	void clear(Dtor dtor);
	void free(Dtor dtor);

private:
	void *getPtr(size_t idx) const;
};

template <typename T>
void swap(T *a, T *b)
{
	T tmp = std::move(*a);
	*a = std::move(*b);
	*b = std::move(tmp);
}

template <typename T>
int compare(const T *a, const T *b)
{
	return ((*a) < (*b)) ? -1 : (((*a) == (*b)) ? 0 : 1);
}

template <typename T>
void destroy(T *self)
{
	self->~T();
}
