/**************************************************************************/
/*  xar.h                                                                 */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             REDOT ENGINE                               */
/*                        https://redotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2026-present Redot Engine contributors                   */
/*                                          (see REDOT_AUTHORS.md)        */
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

#include <cstddef>

// Pointers to elements in this data structure are stable!
// Resource management is not automatic. User must call `free` method to
// release resources.
struct Xar
{
	using Dtor = void (*)(void *);
	size_t elSize;
	size_t bytes;
	void *data[36]; // 36 because the top 16 bits are reserved for
					// kernelspace/userspace identification and bottom 12 are
					// unused in page aligned pointers
					// This assumes pages are 4KB
	Xar(size_t elSize);

	// returns a pointer to the element at index
	//
	// usage:
	// ```cpp
	// *((T *)xar->at(i)) = value;
	// ```
	void *at(size_t index);
	const void *at(size_t index) const;

	// returns a pointer to the last element
	//
	// usage:
	// ```cpp
	// *((T *)xar->last()) = value;
	// ```
	void *last();
	size_t count() const;
	void swapRemove(size_t index, Dtor dtor);
	void orderedRemove(size_t index, Dtor dtor);

	// returns a pointer to the new element inserted at index
	//
	// usage:
	// ```cpp
	// *((T *)xar->insert(i)) = value;
	// ```
	void *insert(size_t index);

	// returns a pointer to the new element pushed to the end of the xar
	//
	// usage:
	// ```cpp
	// *((T *)xar->push()) = value;
	// ```
	void *push();

	// returns a pointer to the popped element at the end of the xar
	//
	// usage:
	// ```cpp
	// T value = *((T *)xar->pop());
	// ```
	void *pop();
	void clear(Dtor dtor);
	void free(Dtor dtor);
private:
	void *getPtr(size_t idx) const;
};
