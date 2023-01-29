/*
 * A very simple and very fast implementation of the Voss-McCartney pink-noise approximation algorithm
 * using a middle square weyl PRNG and a look-up table for octave-sequencing
 * 
 * LICENSE:
 *
 * Copyright 2020 Thomas Merchant
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of
 * the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
 * THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE
 *
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

typedef struct middle_square_weyl
{
    uint64_t s;
    uint64_t x;
    uint64_t w;
} middle_square_weyl;

int32_t pink_next_int(middle_square_weyl* rand)
{
    rand->x *= rand->x;
    rand->x += (rand->w += rand->s);
    return rand->x = (rand->x >> 32u) | (rand->x << 32u);
}

typedef struct pink_noise_gen
{
    middle_square_weyl rand;
    float out;
    float octave_hold_vals[9];
    uint8_t counter;
} pink;

void init_pink_noise_gen(pink *osc)
{
    osc->rand.s = 0x2252e7cd98846fdd;
    osc->rand.x = 0;
    osc->rand.w = 0;
    osc->counter = 0;

    memset(osc->octave_hold_vals, 0, 9*4);
    osc->out = 0;
}

uint8_t trailing_zeros[256] = {8,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,6,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,7,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,6,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,};

float pink_noise_next(pink *osc)
{
    uint8_t octave = trailing_zeros[osc->counter];
    
    int k = -1;
    osc->out -= osc->octave_hold_vals[octave];
    osc->octave_hold_vals[octave] = (float)pink_next_int(&osc->rand) / (float)INT32_MAX;
    osc->octave_hold_vals[octave] /= (float)fabsf((float)k-octave)+1;
    if (octave < k) {
      osc->octave_hold_vals[octave] /= (float)(k+1-octave);
    }
    osc->out += osc->octave_hold_vals[octave];

    osc->counter++;

    return osc->out / 2.0f;
}

