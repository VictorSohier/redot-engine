/**************************************************************************/
/*  math_funcs.cpp                                                        */
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

#include "math_funcs.h"

#include "core/error/error_macros.h"
#include "core/math/random_pcg.h"

static RandomPCG default_rand;

uint32_t Math::rand_from_seed(uint64_t *p_seed) {
	RandomPCG rng = RandomPCG(*p_seed);
	uint32_t r = rng.rand();
	*p_seed = rng.get_seed();
	return r;
}

void Math::seed(uint64_t p_value) {
	default_rand.seed(p_value);
}

void Math::randomize() {
	default_rand.randomize();
}

uint32_t Math::rand() {
	return default_rand.rand();
}

double Math::randfn(double p_mean, double p_deviation) {
	return default_rand.randfn(p_mean, p_deviation);
}

int Math::step_decimals(double p_step) {
	static const int maxn = 10;
	static const double sd[maxn] = {
		0.9999, // somehow compensate for floating point error
		0.09999,
		0.009999,
		0.0009999,
		0.00009999,
		0.000009999,
		0.0000009999,
		0.00000009999,
		0.000000009999,
		0.0000000009999
	};

	double abs = Math::abs(p_step);
	double decs = abs - (int)abs; // Strip away integer part
	for (int i = 0; i < maxn; i++) {
		if (decs >= sd[i]) {
			return i;
		}
	}

	return 0;
}

// Only meant for editor usage in float ranges, where a step of 0
// means that decimal digits should not be limited in String::num.
int Math::range_step_decimals(double p_step) {
	if (p_step < 0.0000000000001) {
		return 16; // Max value hardcoded in String::num
	}
	return step_decimals(p_step);
}

double Math::ease(double p_x, double p_c) {
	if (p_x < 0) {
		p_x = 0;
	} else if (p_x > 1.0) {
		p_x = 1.0;
	}
	if (p_c > 0) {
		if (p_c < 1.0) {
			return 1.0 - Math::pow(1.0 - p_x, 1.0 / p_c);
		} else {
			return Math::pow(p_x, p_c);
		}
	} else if (p_c < 0) {
		//inout ease

		if (p_x < 0.5) {
			return Math::pow(p_x * 2.0, -p_c) * 0.5;
		} else {
			return (1.0 - Math::pow(1.0 - (p_x - 0.5) * 2.0, -p_c)) * 0.5 + 0.5;
		}
	} else {
		return 0; // no ease (raw)
	}
}

double Math::snapped(double p_value, double p_step) {
	if (p_step != 0) {
		p_value = Math::floor(p_value / p_step + 0.5) * p_step;
	}
	return p_value;
}

uint32_t Math::larger_prime(uint32_t p_val) {
	static const uint32_t primes[] = {
		5,
		13,
		23,
		47,
		97,
		193,
		389,
		769,
		1543,
		3079,
		6151,
		12289,
		24593,
		49157,
		98317,
		196613,
		393241,
		786433,
		1572869,
		3145739,
		6291469,
		12582917,
		25165843,
		50331653,
		100663319,
		201326611,
		402653189,
		805306457,
		1610612741,
		0,
	};

	int idx = 0;
	while (true) {
		ERR_FAIL_COND_V(primes[idx] == 0, 0);
		if (primes[idx] > p_val) {
			return primes[idx];
		}
		idx++;
	}
}

double Math::random(double p_from, double p_to) {
	return default_rand.random(p_from, p_to);
}

float Math::random(float p_from, float p_to) {
	return default_rand.random(p_from, p_to);
}

int Math::random(int p_from, int p_to) {
	return default_rand.random(p_from, p_to);
}

uint8_t Math::uGCD(uint8_t a, uint8_t b)
{
    uint8_t x = MIN(a, b);
    uint8_t y = MAX(a, b);
    uint8_t r;
    do
    {
        r = y % x;
        y = x;
        x = r;
    } while (r);
    return y;
}

uint16_t Math::uGCD(uint16_t a, uint16_t b)
{
    uint16_t x = MIN(a, b);
    uint16_t y = MAX(a, b);
    uint16_t r;
    do
    {
        r = y % x;
        y = x;
        x = r;
    } while (r);
    return y;
}

uint32_t Math::uGCD(uint32_t a, uint32_t b)
{
    uint32_t x = MIN(a, b);
    uint32_t y = MAX(a, b);
    uint32_t r;
    do
    {
        r = y % x;
        y = x;
        x = r;
    } while (r);
    return y;
}

uint64_t Math::uGCD(uint64_t a, uint64_t b)
{
    uint64_t x = MIN(a, b);
    uint64_t y = MAX(a, b);
    uint64_t r;
    do
    {
        r = y % x;
        y = x;
        x = r;
    } while (r);
    return y;
}

int8_t sGCD(int8_t a, int8_t b)
{
    int8_t x = MIN(a, b);
    int8_t y = MAX(a, b);
    int8_t r;
    do
    {
        r = y % x;
        y = x;
        x = r;
    } while (r);
    return y;
}

int16_t sGCD(int16_t a, int16_t b)
{
    int16_t x = MIN(a, b);
    int16_t y = MAX(a, b);
    int16_t r;
    do
    {
        r = y % x;
        y = x;
        x = r;
    } while (r);
    return y;
}

int32_t sGCD(int32_t a, int32_t b)
{
    int32_t x = MIN(a, b);
    int32_t y = MAX(a, b);
    int32_t r;
    do
    {
        r = y % x;
        y = x;
        x = r;
    } while (r);
    return y;
}

int64_t sGCD(int64_t a, int64_t b)
{
    int64_t x = MIN(a, b);
    int64_t y = MAX(a, b);
    int64_t r;
    do
    {
        r = y % x;
        y = x;
        x = r;
    } while (r);
    return y;
}
