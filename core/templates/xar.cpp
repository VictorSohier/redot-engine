/**************************************************************************/
/*  xar.cpp                                                               */
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
#include <cstdint>
#include <cstdlib>
#include <cstring>

#include "xar.h"
#include "core/math/math_funcs.h"
#include "core/typedefs.h"

// these two functions are here in the case anyone ever complains about the
// size of Xar and we want to get into pointer stuffing.
void *Xar::getPtr(size_t idx) const
{
	void *ret = nullptr;
	if (idx < 36)
	{
		ret = data[idx];
	}
	return ret;
}

static void setPtr(Xar *self, size_t ptrIdx, void *ptr)
{
	self->data[ptrIdx] = ptr;
}

// right shift by 12 because page sizes are 4KB
// LCM to avoid an awkward and ugly edge case which also leaves wasted space
constexpr size_t getBasePageSize(size_t elSize)
{
	return Math::uLCM((elSize), (1 << 12));
}

Xar::Xar(size_t elSize) : elSize(elSize), bytes(0) { }

void *Xar::at(size_t index)
{
	void *ret = nullptr;
	size_t basePageSize = getBasePageSize(elSize);
	size_t byteIdx = index * elSize;
	size_t pageIdx = byteIdx / basePageSize;
	size_t chunkIdx = MSB(pageIdx);
	byteIdx = byteIdx - (pageIdx * basePageSize);
	pageIdx = pageIdx & ((1 << chunkIdx) - 1);
	if ((byteIdx + elSize) <= bytes)
	{
		ret = ((uint8_t *)getPtr(chunkIdx)) +
			(basePageSize * pageIdx) +
			byteIdx;
	}
	return ret;
}

const void *Xar::at(size_t index) const
{
	const void *ret = nullptr;
	size_t basePageSize = getBasePageSize(elSize);
	size_t byteIdx = index * elSize;
	size_t pageIdx = byteIdx / basePageSize;
	size_t chunkIdx = MSB(pageIdx);
	byteIdx = byteIdx - (pageIdx * basePageSize);
	pageIdx = pageIdx & ((1 << chunkIdx) - 1);
	if ((byteIdx + elSize) <= bytes)
	{
		ret = ((uint8_t *)getPtr(chunkIdx)) +
			(basePageSize * pageIdx) +
			byteIdx;
	}
	return ret;
}

void *Xar::last()
{
	return at(count() - 1);
}

size_t Xar::count() const
{
	return bytes / elSize;
}

void Xar::swapRemove(size_t index, Dtor dtor)
{
	dtor(at(index));
	memcpy(at(index), at((bytes / elSize) - 1), elSize);
	bytes -= elSize;
}

void Xar::orderedRemove(size_t index, Dtor dtor)
{
	size_t elCount = count();
	dtor(at(index));
	for (size_t i = index; i < elCount - 1; i += 1)
	{
		memcpy(at(i), at(i + 1), elSize);
	}
	bytes -= elSize;
}

void *Xar::insert(size_t index)
{
	size_t basePageSize = getBasePageSize(elSize);
	size_t byteIdx = index * elSize;
	size_t pageIdx = byteIdx / basePageSize;
	size_t chunkIdx = MSB(pageIdx);
	size_t elCount = count();
	void *ptr;
	if (!getPtr(chunkIdx))
	{
		for (size_t i = 0; i <= chunkIdx; i += 1)
		{
			ptr = getPtr(i);
			if (!ptr) {
				ptr = malloc(basePageSize << chunkIdx);
				memset(ptr, 0, basePageSize << chunkIdx);
				setPtr(this, i, ptr);
			}
		}
	}
	for (size_t i = elCount; i > index; i -= 1)
	{
		memcpy(at(i), at(i - 1), elSize);
	}
	bytes = MAX(byteIdx, bytes) + elSize;
	return at(index);
}

void *Xar::push()
{
	size_t basePageSize = getBasePageSize(elSize);
	size_t byteIdx = bytes;
	size_t pageIdx = byteIdx / basePageSize;
	size_t chunkIdx = MSB(pageIdx);
	void *ptr;
	if (!getPtr(chunkIdx))
	{
		for (size_t i = 0; i <= chunkIdx; i += 1)
		{
			ptr = getPtr(i);
			if (!ptr) {
				ptr = malloc(basePageSize << chunkIdx);
				memset(ptr, 0, basePageSize << chunkIdx);
				setPtr(this, i, ptr);
			}
		}
	}
	bytes += elSize;
	return last();
}

void *Xar::pop()
{
	void *ret = last();
	bytes -= elSize;
	return ret;
}

void Xar::clear(Dtor dtor)
{
	for (size_t i = 0; i < count(); i += 1)
	{
		dtor(at(i));
	}
	bytes = 0;
}

void Xar::free(Dtor dtor)
{
	size_t i = 0;
	void *ptr = getPtr(i);
	for (; i < count(); i += 1)
	{
		dtor(at(i));
	}
	i = 0;
	for (; ptr != nullptr;)
	{
		::free(ptr);
		i += 1;
		ptr = getPtr(i);
	}
}

bool Xar::contains(size_t *idx, const void *value, Comparer cmp) const
{
	size_t i = 0;
	for (; i < count(); i += 1)
	{
		if (!cmp(at(i), value))
		{
			if (idx)
			{
				*idx = i;
			}
			return true;
		}
	}
	return false;
}
