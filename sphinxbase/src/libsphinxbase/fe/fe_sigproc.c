/* -*- c-basic-offset: 4; indent-tabs-mode: nil -*- */
/* ====================================================================
 * Copyright (c) 1996-2004 Carnegie Mellon University.  All rights 
 * reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * This work was supported in part by funding from the Defense Advanced 
 * Research Projects Agency and the National Science Foundation of the 
 * United States of America, and the CMU Sphinx Speech Consortium.
 *
 * THIS SOFTWARE IS PROVIDED BY CARNEGIE MELLON UNIVERSITY ``AS IS'' AND 
 * ANY EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY
 * NOR ITS EMPLOYEES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ====================================================================
 *
 */

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef _MSC_VER
#pragma warning (disable: 4244)
#endif

/**
 * Windows math.h does not contain M_PI
 */
#ifndef M_PI
#define M_PI		3.14159265358979323846  /* pi */
#endif                          // M_PI

#include "sphinxbase/prim_type.h"
#include "sphinxbase/ckd_alloc.h"
#include "sphinxbase/byteorder.h"
#include "sphinxbase/fixpoint.h"
#include "sphinxbase/fe.h"
#include "sphinxbase/genrand.h"
#include "sphinxbase/err.h"

#include "fe_internal.h"
#include "fe_warp.h"

/* Use extra precision for cosines, Hamming window, pre-emphasis
 * coefficient, twiddle factors. */
#ifdef FIXED_POINT
#define FLOAT2COS(x) FLOAT2FIX_ANY(x,30)
#define COSMUL(x,y) FIXMUL_ANY(x,y,30)
#else
#define FLOAT2COS(x) (x)
#define COSMUL(x,y) ((x)*(y))
#endif

#ifdef FIXED_POINT
/* Internal log-addition table for natural log with radix point at 8
 * bits.  Each entry is 256 * log(1 + e^{-n/256}).  This is used in the
 * log-add computation:
 *
 * e^z = e^x + e^y
 * e^z = e^x(1 + e^{y-x})     = e^y(1 + e^{x-y})
 * z   = x + log(1 + e^{y-x}) = y + log(1 + e^{x-y})
 *
 * So when y > x, z = y + logadd_table[-(x-y)]
 *    when x > y, z = x + logadd_table[-(y-x)]
 */
static const unsigned char fe_logadd_table[] = {
    177, 177, 176, 176, 175, 175, 174, 174, 173, 173,
    172, 172, 172, 171, 171, 170, 170, 169, 169, 168,
    168, 167, 167, 166, 166, 165, 165, 164, 164, 163,
    163, 162, 162, 161, 161, 161, 160, 160, 159, 159,
    158, 158, 157, 157, 156, 156, 155, 155, 155, 154,
    154, 153, 153, 152, 152, 151, 151, 151, 150, 150,
    149, 149, 148, 148, 147, 147, 147, 146, 146, 145,
    145, 144, 144, 144, 143, 143, 142, 142, 141, 141,
    141, 140, 140, 139, 139, 138, 138, 138, 137, 137,
    136, 136, 136, 135, 135, 134, 134, 134, 133, 133,
    132, 132, 131, 131, 131, 130, 130, 129, 129, 129,
    128, 128, 128, 127, 127, 126, 126, 126, 125, 125,
    124, 124, 124, 123, 123, 123, 122, 122, 121, 121,
    121, 120, 120, 119, 119, 119, 118, 118, 118, 117,
    117, 117, 116, 116, 115, 115, 115, 114, 114, 114,
    113, 113, 113, 112, 112, 112, 111, 111, 110, 110,
    110, 109, 109, 109, 108, 108, 108, 107, 107, 107,
    106, 106, 106, 105, 105, 105, 104, 104, 104, 103,
    103, 103, 102, 102, 102, 101, 101, 101, 100, 100,
    100, 99, 99, 99, 98, 98, 98, 97, 97, 97,
    96, 96, 96, 96, 95, 95, 95, 94, 94, 94,
    93, 93, 93, 92, 92, 92, 92, 91, 91, 91,
    90, 90, 90, 89, 89, 89, 89, 88, 88, 88,
    87, 87, 87, 87, 86, 86, 86, 85, 85, 85,
    85, 84, 84, 84, 83, 83, 83, 83, 82, 82,
    82, 82, 81, 81, 81, 80, 80, 80, 80, 79,
    79, 79, 79, 78, 78, 78, 78, 77, 77, 77,
    77, 76, 76, 76, 75, 75, 75, 75, 74, 74,
    74, 74, 73, 73, 73, 73, 72, 72, 72, 72,
    71, 71, 71, 71, 71, 70, 70, 70, 70, 69,
    69, 69, 69, 68, 68, 68, 68, 67, 67, 67,
    67, 67, 66, 66, 66, 66, 65, 65, 65, 65,
    64, 64, 64, 64, 64, 63, 63, 63, 63, 63,
    62, 62, 62, 62, 61, 61, 61, 61, 61, 60,
    60, 60, 60, 60, 59, 59, 59, 59, 59, 58,
    58, 58, 58, 58, 57, 57, 57, 57, 57, 56,
    56, 56, 56, 56, 55, 55, 55, 55, 55, 54,
    54, 54, 54, 54, 53, 53, 53, 53, 53, 52,
    52, 52, 52, 52, 52, 51, 51, 51, 51, 51,
    50, 50, 50, 50, 50, 50, 49, 49, 49, 49,
    49, 49, 48, 48, 48, 48, 48, 48, 47, 47,
    47, 47, 47, 47, 46, 46, 46, 46, 46, 46,
    45, 45, 45, 45, 45, 45, 44, 44, 44, 44,
    44, 44, 43, 43, 43, 43, 43, 43, 43, 42,
    42, 42, 42, 42, 42, 41, 41, 41, 41, 41,
    41, 41, 40, 40, 40, 40, 40, 40, 40, 39,
    39, 39, 39, 39, 39, 39, 38, 38, 38, 38,
    38, 38, 38, 37, 37, 37, 37, 37, 37, 37,
    37, 36, 36, 36, 36, 36, 36, 36, 35, 35,
    35, 35, 35, 35, 35, 35, 34, 34, 34, 34,
    34, 34, 34, 34, 33, 33, 33, 33, 33, 33,
    33, 33, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 31, 31, 31, 31, 31, 31, 31, 31, 31,
    30, 30, 30, 30, 30, 30, 30, 30, 30, 29,
    29, 29, 29, 29, 29, 29, 29, 29, 28, 28,
    28, 28, 28, 28, 28, 28, 28, 28, 27, 27,
    27, 27, 27, 27, 27, 27, 27, 27, 26, 26,
    26, 26, 26, 26, 26, 26, 26, 26, 25, 25,
    25, 25, 25, 25, 25, 25, 25, 25, 25, 24,
    24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
    23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
    23, 23, 22, 22, 22, 22, 22, 22, 22, 22,
    22, 22, 22, 22, 21, 21, 21, 21, 21, 21,
    21, 21, 21, 21, 21, 21, 21, 20, 20, 20,
    20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
    19, 19, 19, 19, 19, 19, 19, 19, 19, 19,
    19, 19, 19, 19, 18, 18, 18, 18, 18, 18,
    18, 18, 18, 18, 18, 18, 18, 18, 18, 17,
    17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
    17, 17, 17, 17, 16, 16, 16, 16, 16, 16,
    16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
    16, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 14, 14,
    14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
    14, 14, 14, 14, 14, 14, 14, 13, 13, 13,
    13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
    13, 13, 13, 13, 13, 13, 13, 12, 12, 12,
    12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
    12, 12, 12, 12, 12, 12, 12, 12, 12, 11,
    11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
    11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
    11, 11, 11, 10, 10, 10, 10, 10, 10, 10,
    10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
    10, 10, 10, 10, 10, 10, 10, 10, 10, 9,
    9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
    9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
    9, 9, 9, 9, 9, 9, 9, 9, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    6, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 5, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 0
};

static const int fe_logadd_table_size =
    sizeof(fe_logadd_table) / sizeof(fe_logadd_table[0]);

static fixed32
fe_log_add(fixed32 x, fixed32 y)
{
    fixed32 d, r;

    if (x > y) {
        d = (x - y) >> (DEFAULT_RADIX - 8);
        r = x;
    }
    else {
        d = (y - x) >> (DEFAULT_RADIX - 8);
        r = y;
    }
    if (d > fe_logadd_table_size - 1)
        return r;
    else {
        r += ((fixed32) fe_logadd_table[d] << (DEFAULT_RADIX - 8));
/*
        printf("%d + %d = %d | %f + %f = %f | %f + %f = %f\n",
               x, y, r, FIX2FLOAT(x), FIX2FLOAT(y), FIX2FLOAT(r),
               exp(FIX2FLOAT(x)), exp(FIX2FLOAT(y)), exp(FIX2FLOAT(r)));
*/
        return r;
    }
}

static fixed32
fe_log(float32 x)
{
    if (x <= 0) {
        return MIN_FIXLOG;
    }
    else {
        return FLOAT2FIX(log(x));
    }
}
#endif

static float32
fe_mel(melfb_t * mel, float32 x)
{
    float32 warped = fe_warp_unwarped_to_warped(mel, x);

    return (float32) (2595.0 * log10(1.0 + warped / 700.0));
}

static float32
fe_melinv(melfb_t * mel, float32 x)
{
    float32 warped = (float32) (700.0 * (pow(10.0, x / 2595.0) - 1.0));
    return fe_warp_warped_to_unwarped(mel, warped);
}

int32
fe_build_melfilters(melfb_t * mel_fb)
{
    float32 melmin, melmax, melbw, fftfreq;
    int n_coeffs, i, j;


    /* Filter coefficient matrix, in flattened form. */
    mel_fb->spec_start =
        ckd_malloc(mel_fb->num_filters * sizeof(*mel_fb->spec_start));
    mel_fb->filt_start =
        ckd_malloc(mel_fb->num_filters * sizeof(*mel_fb->filt_start));
    mel_fb->filt_width =
        ckd_malloc(mel_fb->num_filters * sizeof(*mel_fb->filt_width));

    /* First calculate the widths of each filter. */
    /* Minimum and maximum frequencies in mel scale. */
    melmin = fe_mel(mel_fb, mel_fb->lower_filt_freq);
    melmax = fe_mel(mel_fb, mel_fb->upper_filt_freq);

    /* Width of filters in mel scale */
    melbw = (melmax - melmin) / (mel_fb->num_filters + 1);
    if (mel_fb->doublewide) {
        melmin -= melbw;
        melmax += melbw;
        if ((fe_melinv(mel_fb, melmin) < 0) ||
            (fe_melinv(mel_fb, melmax) > mel_fb->sampling_rate / 2)) {
            E_WARN
                ("Out of Range: low  filter edge = %f (%f)\n",
                 fe_melinv(mel_fb, melmin), 0.0);
            E_WARN
                ("              high filter edge = %f (%f)\n",
                 fe_melinv(mel_fb, melmax), mel_fb->sampling_rate / 2);
            return FE_INVALID_PARAM_ERROR;
        }
    }

    /* DFT point spacing */
    fftfreq = mel_fb->sampling_rate / (float32) mel_fb->fft_size;

    /* Count and place filter coefficients. */
    n_coeffs = 0;
    for (i = 0; i < mel_fb->num_filters; ++i) {
        float32 freqs[3];

        /* Left, center, right frequencies in Hertz */
        for (j = 0; j < 3; ++j) {
            if (mel_fb->doublewide)
                freqs[j] = fe_melinv(mel_fb, (i + j * 2) * melbw + melmin);
            else
                freqs[j] = fe_melinv(mel_fb, (i + j) * melbw + melmin);
            /* Round them to DFT points if requested */
            if (mel_fb->round_filters)
                freqs[j] = ((int) (freqs[j] / fftfreq + 0.5)) * fftfreq;
        }

        /* spec_start is the start of this filter in the power spectrum. */
        mel_fb->spec_start[i] = -1;
        /* There must be a better way... */
        for (j = 0; j < mel_fb->fft_size / 2 + 1; ++j) {
            float32 hz = j * fftfreq;
            if (hz < freqs[0])
                continue;
            else if (hz > freqs[2] || j == mel_fb->fft_size / 2) {
                /* filt_width is the width in DFT points of this filter. */
                mel_fb->filt_width[i] = j - mel_fb->spec_start[i];
                /* filt_start is the start of this filter in the filt_coeffs array. */
                mel_fb->filt_start[i] = n_coeffs;
                n_coeffs += mel_fb->filt_width[i];
                break;
            }
            if (mel_fb->spec_start[i] == -1)
                mel_fb->spec_start[i] = j;
        }
    }

    /* Now go back and allocate the coefficient array. */
    mel_fb->filt_coeffs =
        ckd_malloc(n_coeffs * sizeof(*mel_fb->filt_coeffs));

    /* And now generate the coefficients. */
    n_coeffs = 0;
    for (i = 0; i < mel_fb->num_filters; ++i) {
        float32 freqs[3];

        /* Left, center, right frequencies in Hertz */
        for (j = 0; j < 3; ++j) {
            if (mel_fb->doublewide)
                freqs[j] = fe_melinv(mel_fb, (i + j * 2) * melbw + melmin);
            else
                freqs[j] = fe_melinv(mel_fb, (i + j) * melbw + melmin);
            /* Round them to DFT points if requested */
            if (mel_fb->round_filters)
                freqs[j] = ((int) (freqs[j] / fftfreq + 0.5)) * fftfreq;
        }

        for (j = 0; j < mel_fb->filt_width[i]; ++j) {
            float32 hz, loslope, hislope;

            hz = (mel_fb->spec_start[i] + j) * fftfreq;
            if (hz < freqs[0] || hz > freqs[2]) {
                E_FATAL
                    ("Failed to create filterbank, frequency range does not match. "
                     "Sample rate %f, FFT size %d, lowerf %f < freq %f > upperf %f.\n",
                     mel_fb->sampling_rate, mel_fb->fft_size, freqs[2], hz,
                     freqs[0]);
            }
            loslope = (hz - freqs[0]) / (freqs[1] - freqs[0]);
            hislope = (freqs[2] - hz) / (freqs[2] - freqs[1]);
            if (mel_fb->unit_area) {
                loslope *= 2 / (freqs[2] - freqs[0]);
                hislope *= 2 / (freqs[2] - freqs[0]);
            }
            if (loslope < hislope) {
#ifdef FIXED_POINT
                mel_fb->filt_coeffs[n_coeffs] = fe_log(loslope);
#else
                mel_fb->filt_coeffs[n_coeffs] = loslope;
#endif
            }
            else {
#ifdef FIXED_POINT
                mel_fb->filt_coeffs[n_coeffs] = fe_log(hislope);
#else
                mel_fb->filt_coeffs[n_coeffs] = hislope;
#endif
            }
            ++n_coeffs;
        }
    }

    return FE_SUCCESS;
}

int32
fe_compute_melcosine(melfb_t * mel_fb)
{

    float64 freqstep;
    int32 i, j;

    mel_fb->mel_cosine =
        (mfcc_t **) ckd_calloc_2d(mel_fb->num_cepstra,
                                  mel_fb->num_filters, sizeof(mfcc_t));

    freqstep = M_PI / mel_fb->num_filters;
    /* NOTE: The first row vector is actually unnecessary but we leave
     * it in to avoid confusion. */
    for (i = 0; i < mel_fb->num_cepstra; i++) {
        for (j = 0; j < mel_fb->num_filters; j++) {
            float64 cosine;

            cosine = cos(freqstep * i * (j + 0.5));
            mel_fb->mel_cosine[i][j] = FLOAT2COS(cosine);
        }
    }

    /* Also precompute normalization constants for unitary DCT. */
    mel_fb->sqrt_inv_n = FLOAT2COS(sqrt(1.0 / mel_fb->num_filters));
    mel_fb->sqrt_inv_2n = FLOAT2COS(sqrt(2.0 / mel_fb->num_filters));

    /* And liftering weights */
    if (mel_fb->lifter_val) {
        mel_fb->lifter =
            calloc(mel_fb->num_cepstra, sizeof(*mel_fb->lifter));
        for (i = 0; i < mel_fb->num_cepstra; ++i) {
            mel_fb->lifter[i] = FLOAT2MFCC(1 + mel_fb->lifter_val / 2
                                           * sin(i * M_PI /
                                                 mel_fb->lifter_val));
        }
    }

    return (0);
}

static void
fe_pre_emphasis(int16 const *in, frame_t * out, int32 len,
                float32 factor, int16 prior)
{
    int i;

#if defined(FIXED16)
    int16 fxd_alpha = (int16) (factor * 0x8000);
    int32 tmp1, tmp2;

    tmp1 = (int32) in[0] << 15;
    tmp2 = (int32) prior *fxd_alpha;
    out[0] = (int16) ((tmp1 - tmp2) >> 15);
    for (i = 1; i < len; ++i) {
        tmp1 = (int32) in[i] << 15;
        tmp2 = (int32) in[i - 1] * fxd_alpha;
        out[i] = (int16) ((tmp1 - tmp2) >> 15);
    }
#elif defined(FIXED_POINT)
    fixed32 fxd_alpha = FLOAT2FIX(factor);
    out[0] = ((fixed32) in[0] << DEFAULT_RADIX) - (prior * fxd_alpha);
    for (i = 1; i < len; ++i)
        out[i] = ((fixed32) in[i] << DEFAULT_RADIX)
            - (fixed32) in[i - 1] * fxd_alpha;
#else
    out[0] = (frame_t) in[0] - (frame_t) prior *factor;
    for (i = 1; i < len; i++)
        out[i] = (frame_t) in[i] - (frame_t) in[i - 1] * factor;
#endif
}

static void
fe_short_to_frame(int16 const *in, frame_t * out, int32 len)
{
    int i;

#if defined(FIXED16)
    memcpy(out, in, len * sizeof(*out));
#elif defined(FIXED_POINT)
    for (i = 0; i < len; i++)
        out[i] = (int32) in[i] << DEFAULT_RADIX;
#else                           /* FIXED_POINT */
    for (i = 0; i < len; i++)
        out[i] = (frame_t) in[i];
#endif                          /* FIXED_POINT */
}

void
fe_create_hamming(window_t * in, int32 in_len)
{
    int i;

    /* Symmetric, so we only create the first half of it. */
    for (i = 0; i < in_len / 2; i++) {
        float64 hamm;
        hamm = (0.54 - 0.46 * cos(2 * M_PI * i /
                                  ((float64) in_len - 1.0)));
#ifdef FIXED16
        in[i] = (int16) (hamm * 0x8000);
#else
        in[i] = FLOAT2COS(hamm);
#endif
    }
}

static void
fe_hamming_window(frame_t * in, window_t * window, int32 in_len,
                  int32 remove_dc)
{
    int i;

    if (remove_dc) {
#ifdef FIXED16
        int32 mean = 0;         /* Use int32 to avoid possibility of overflow */
#else
        frame_t mean = 0;
#endif

        for (i = 0; i < in_len; i++)
            mean += in[i];
        mean /= in_len;
        for (i = 0; i < in_len; i++)
            in[i] -= (frame_t) mean;
    }

#ifdef FIXED16
    for (i = 0; i < in_len / 2; i++) {
        int32 tmp1, tmp2;

        tmp1 = (int32) in[i] * window[i];
        tmp2 = (int32) in[in_len - 1 - i] * window[i];
        in[i] = (int16) (tmp1 >> 15);
        in[in_len - 1 - i] = (int16) (tmp2 >> 15);
    }
#else
    for (i = 0; i < in_len / 2; i++) {
        in[i] = COSMUL(in[i], window[i]);
        in[in_len - 1 - i] = COSMUL(in[in_len - 1 - i], window[i]);
    }
#endif
}

static int
fe_spch_to_frame(fe_t * fe, int len)
{
    /* Copy to the frame buffer. */
    if (fe->pre_emphasis_alpha != 0.0) {
        fe_pre_emphasis(fe->spch, fe->frame, len,
                        fe->pre_emphasis_alpha, fe->prior);
        if (len >= fe->frame_shift)
            fe->prior = fe->spch[fe->frame_shift - 1];
        else
            fe->prior = fe->spch[len - 1];
    }
    else
        fe_short_to_frame(fe->spch, fe->frame, len);

    /* Zero pad up to FFT size. */
    memset(fe->frame + len, 0, (fe->fft_size - len) * sizeof(*fe->frame));

    /* Window. */
    fe_hamming_window(fe->frame, fe->hamming_window, fe->frame_size,
                      fe->remove_dc);

    return len;
}

int
fe_read_frame(fe_t * fe, int16 const *in, int32 len)
{
    int i;

    if (len > fe->frame_size)
        len = fe->frame_size;

    /* Read it into the raw speech buffer. */
    memcpy(fe->spch, in, len * sizeof(*in));
    /* Swap and dither if necessary. */
    if (fe->swap)
        for (i = 0; i < len; ++i)
            SWAP_INT16(&fe->spch[i]);
    if (fe->dither)
        for (i = 0; i < len; ++i)
            fe->spch[i] += (int16) ((!(s3_rand_int31() % 4)) ? 1 : 0);

    return fe_spch_to_frame(fe, len);
}

int
fe_shift_frame(fe_t * fe, int16 const *in, int32 len)
{
    int offset, i;

    if (len > fe->frame_shift)
        len = fe->frame_shift;
    offset = fe->frame_size - fe->frame_shift;

    /* Shift data into the raw speech buffer. */
    memmove(fe->spch, fe->spch + fe->frame_shift,
            offset * sizeof(*fe->spch));
    memcpy(fe->spch + offset, in, len * sizeof(*fe->spch));
    /* Swap and dither if necessary. */
    if (fe->swap)
        for (i = 0; i < len; ++i)
            SWAP_INT16(&fe->spch[offset + i]);
    if (fe->dither)
        for (i = 0; i < len; ++i)
            fe->spch[offset + i]
                += (int16) ((!(s3_rand_int31() % 4)) ? 1 : 0);

    return fe_spch_to_frame(fe, offset + len);
}

/**
 * Create arrays of twiddle factors.
 */
void
fe_create_twiddle(fe_t * fe)
{
    int i;

    for (i = 0; i < fe->fft_size / 4; ++i) {
        float64 a = 2 * M_PI * i / fe->fft_size;
#ifdef FIXED16
        fe->ccc[i] = (int16) (cos(a) * 0x8000);
        fe->sss[i] = (int16) (sin(a) * 0x8000);
#elif defined(FIXED_POINT)
        fe->ccc[i] = FLOAT2COS(cos(a));
        fe->sss[i] = FLOAT2COS(sin(a));
#else
        fe->ccc[i] = cos(a);
        fe->sss[i] = sin(a);
#endif
    }
}


/* Translated from the FORTRAN (obviously) from "Real-Valued Fast
 * Fourier Transform Algorithms" by Henrik V. Sorensen et al., IEEE
 * Transactions on Acoustics, Speech, and Signal Processing, vol. 35,
 * no.6.  The 16-bit version does a version of "block floating
 * point" in order to avoid rounding errors.
 */
#if defined(FIXED16)
static int
fe_fft_real(fe_t * fe)
{
    int i, j, k, m, n, lz;
    frame_t *x, xt, max;

    x = fe->frame;
    m = fe->fft_order;
    n = fe->fft_size;

    /* Bit-reverse the input. */
    j = 0;
    for (i = 0; i < n - 1; ++i) {
        if (i < j) {
            xt = x[j];
            x[j] = x[i];
            x[i] = xt;
        }
        k = n / 2;
        while (k <= j) {
            j -= k;
            k /= 2;
        }
        j += k;
    }
    /* Determine how many bits of dynamic range are in the input. */
    max = 0;
    for (i = 0; i < n; ++i)
        if (abs(x[i]) > max)
            max = abs(x[i]);
    /* The FFT has a gain of M bits, so we need to attenuate the input
     * by M bits minus the number of leading zeroes in the input's
     * range in order to avoid overflows.  */
    for (lz = 0; lz < m; ++lz)
        if (max & (1 << (15 - lz)))
            break;

    /* Basic butterflies (2-point FFT, real twiddle factors):
     * x[i]   = x[i] +  1 * x[i+1]
     * x[i+1] = x[i] + -1 * x[i+1]
     */
    /* The quantization error introduced by attenuating the input at
     * any given stage of the FFT has a cascading effect, so we hold
     * off on it until it's absolutely necessary. */
    for (i = 0; i < n; i += 2) {
        int atten = (lz == 0);
        xt = x[i] >> atten;
        x[i] = xt + (x[i + 1] >> atten);
        x[i + 1] = xt - (x[i + 1] >> atten);
    }

    /* The rest of the butterflies, in stages from 1..m */
    for (k = 1; k < m; ++k) {
        int n1, n2, n4;
        /* Start attenuating once we hit the number of leading zeros. */
        int atten = (k >= lz);

        n4 = k - 1;
        n2 = k;
        n1 = k + 1;
        /* Stride over each (1 << (k+1)) points */
        for (i = 0; i < n; i += (1 << n1)) {
            /* Basic butterfly with real twiddle factors:
             * x[i]          = x[i] +  1 * x[i + (1<<k)]
             * x[i + (1<<k)] = x[i] + -1 * x[i + (1<<k)]
             */
            xt = x[i] >> atten;
            x[i] = xt + (x[i + (1 << n2)] >> atten);
            x[i + (1 << n2)] = xt - (x[i + (1 << n2)] >> atten);

            /* The other ones with real twiddle factors:
             * x[i + (1<<k) + (1<<(k-1))]
             *   = 0 * x[i + (1<<k-1)] + -1 * x[i + (1<<k) + (1<<k-1)]
             * x[i + (1<<(k-1))]
             *   = 1 * x[i + (1<<k-1)] +  0 * x[i + (1<<k) + (1<<k-1)]
             */
            x[i + (1 << n2) + (1 << n4)] =
                -x[i + (1 << n2) + (1 << n4)] >> atten;
            x[i + (1 << n4)] = x[i + (1 << n4)] >> atten;

            /* Butterflies with complex twiddle factors.
             * There are (1<<k-1) of them.
             */
            for (j = 1; j < (1 << n4); ++j) {
                frame_t cc, ss, t1, t2;
                int i1, i2, i3, i4;

                i1 = i + j;
                i2 = i + (1 << n2) - j;
                i3 = i + (1 << n2) + j;
                i4 = i + (1 << n2) + (1 << n2) - j;

                /*
                 * cc = real(W[j * n / (1<<(k+1))])
                 * ss = imag(W[j * n / (1<<(k+1))])
                 */
                cc = fe->ccc[j << (m - n1)];
                ss = fe->sss[j << (m - n1)];

                /* There are some symmetry properties which allow us
                 * to get away with only four multiplications here. */
                {
                    int32 tmp1, tmp2;
                    tmp1 = (int32) x[i3] * cc + (int32) x[i4] * ss;
                    tmp2 = (int32) x[i3] * ss - (int32) x[i4] * cc;
                    t1 = (int16) (tmp1 >> 15) >> atten;
                    t2 = (int16) (tmp2 >> 15) >> atten;
                }

                x[i4] = (x[i2] >> atten) - t2;
                x[i3] = (-x[i2] >> atten) - t2;
                x[i2] = (x[i1] >> atten) - t1;
                x[i1] = (x[i1] >> atten) + t1;
            }
        }
    }

    /* Return the residual scaling factor. */
    return lz;
}
#else                           /* !FIXED16 */
static int
fe_fft_real(fe_t * fe)
{
    int i, j, k, m, n;
    frame_t *x, xt;

    x = fe->frame;
    m = fe->fft_order;
    n = fe->fft_size;

    /* Bit-reverse the input. */
    j = 0;
    for (i = 0; i < n - 1; ++i) {
        if (i < j) {
            xt = x[j];
            x[j] = x[i];
            x[i] = xt;
        }
        k = n / 2;
        while (k <= j) {
            j -= k;
            k /= 2;
        }
        j += k;
    }

    /* Basic butterflies (2-point FFT, real twiddle factors):
     * x[i]   = x[i] +  1 * x[i+1]
     * x[i+1] = x[i] + -1 * x[i+1]
     */
    for (i = 0; i < n; i += 2) {
        xt = x[i];
        x[i] = (xt + x[i + 1]);
        x[i + 1] = (xt - x[i + 1]);
    }

    /* The rest of the butterflies, in stages from 1..m */
    for (k = 1; k < m; ++k) {
        int n1, n2, n4;

        n4 = k - 1;
        n2 = k;
        n1 = k + 1;
        /* Stride over each (1 << (k+1)) points */
        for (i = 0; i < n; i += (1 << n1)) {
            /* Basic butterfly with real twiddle factors:
             * x[i]          = x[i] +  1 * x[i + (1<<k)]
             * x[i + (1<<k)] = x[i] + -1 * x[i + (1<<k)]
             */
            xt = x[i];
            x[i] = (xt + x[i + (1 << n2)]);
            x[i + (1 << n2)] = (xt - x[i + (1 << n2)]);

            /* The other ones with real twiddle factors:
             * x[i + (1<<k) + (1<<(k-1))]
             *   = 0 * x[i + (1<<k-1)] + -1 * x[i + (1<<k) + (1<<k-1)]
             * x[i + (1<<(k-1))]
             *   = 1 * x[i + (1<<k-1)] +  0 * x[i + (1<<k) + (1<<k-1)]
             */
            x[i + (1 << n2) + (1 << n4)] = -x[i + (1 << n2) + (1 << n4)];
            x[i + (1 << n4)] = x[i + (1 << n4)];

            /* Butterflies with complex twiddle factors.
             * There are (1<<k-1) of them.
             */
            for (j = 1; j < (1 << n4); ++j) {
                frame_t cc, ss, t1, t2;
                int i1, i2, i3, i4;

                i1 = i + j;
                i2 = i + (1 << n2) - j;
                i3 = i + (1 << n2) + j;
                i4 = i + (1 << n2) + (1 << n2) - j;

                /*
                 * cc = real(W[j * n / (1<<(k+1))])
                 * ss = imag(W[j * n / (1<<(k+1))])
                 */
                cc = fe->ccc[j << (m - n1)];
                ss = fe->sss[j << (m - n1)];

                /* There are some symmetry properties which allow us
                 * to get away with only four multiplications here. */
                t1 = COSMUL(x[i3], cc) + COSMUL(x[i4], ss);
                t2 = COSMUL(x[i3], ss) - COSMUL(x[i4], cc);

                x[i4] = (x[i2] - t2);
                x[i3] = (-x[i2] - t2);
                x[i2] = (x[i1] - t1);
                x[i1] = (x[i1] + t1);
            }
        }
    }

    /* This isn't used, but return it for completeness. */
    return m;
}
#endif                          /* !FIXED16 */

static void
fe_spec_magnitude(fe_t * fe)
{
    frame_t *fft;
    powspec_t *spec;
    int32 j, scale, fftsize;

    /* Do FFT and get the scaling factor back (only actually used in
     * fixed-point).  Note the scaling factor is expressed in bits. */
    scale = fe_fft_real(fe);

    /* Convenience pointers to make things less awkward below. */
    fft = fe->frame;
    spec = fe->spec;
    fftsize = fe->fft_size;

    /* We need to scale things up the rest of the way to N. */
    scale = fe->fft_order - scale;

    /* The first point (DC coefficient) has no imaginary part */
    {
#ifdef FIXED16
        spec[0] = fixlog(abs(fft[0]) << scale) * 2;
#elif defined(FIXED_POINT)
        spec[0] = FIXLN(abs(fft[0]) << scale) * 2;
#else
        spec[0] = fft[0] * fft[0];
#endif
    }

    for (j = 1; j <= fftsize / 2; j++) {
#ifdef FIXED16
        int32 rr = fixlog(abs(fft[j]) << scale) * 2;
        int32 ii = fixlog(abs(fft[fftsize - j]) << scale) * 2;
        spec[j] = fe_log_add(rr, ii);
#elif defined(FIXED_POINT)
        int32 rr = FIXLN(abs(fft[j]) << scale) * 2;
        int32 ii = FIXLN(abs(fft[fftsize - j]) << scale) * 2;
        spec[j] = fe_log_add(rr, ii);
#else
        spec[j] = fft[j] * fft[j] + fft[fftsize - j] * fft[fftsize - j];
#endif
    }
}

static void
fe_mel_spec(fe_t * fe)
{
    int whichfilt;
    powspec_t *spec, *mfspec;

    /* Convenience poitners. */
    spec = fe->spec;
    mfspec = fe->mfspec;
    for (whichfilt = 0; whichfilt < fe->mel_fb->num_filters; whichfilt++) {
        int spec_start, filt_start, i;

        spec_start = fe->mel_fb->spec_start[whichfilt];
        filt_start = fe->mel_fb->filt_start[whichfilt];

#ifdef FIXED_POINT
        mfspec[whichfilt] =
            spec[spec_start] + fe->mel_fb->filt_coeffs[filt_start];
        for (i = 1; i < fe->mel_fb->filt_width[whichfilt]; i++) {
            mfspec[whichfilt] = fe_log_add(mfspec[whichfilt],
                                           spec[spec_start + i] +
                                           fe->mel_fb->
                                           filt_coeffs[filt_start + i]);
        }
#else                           /* !FIXED_POINT */
        mfspec[whichfilt] = 0;
        for (i = 0; i < fe->mel_fb->filt_width[whichfilt]; i++)
            mfspec[whichfilt] +=
                spec[spec_start + i] * fe->mel_fb->filt_coeffs[filt_start +
                                                               i];
#endif                          /* !FIXED_POINT */
    }

}

#define LOG_FLOOR 1e-4

static void
fe_mel_cep(fe_t * fe, mfcc_t * mfcep)
{
    int32 i;
    powspec_t *mfspec;

    /* Convenience pointer. */
    mfspec = fe->mfspec;

    for (i = 0; i < fe->mel_fb->num_filters; ++i) {
#ifndef FIXED_POINT             /* It's already in log domain for fixed point */
        mfspec[i] = log(mfspec[i] + LOG_FLOOR);
#endif                          /* !FIXED_POINT */
    }

    /* If we are doing LOG_SPEC, then do nothing. */
    if (fe->log_spec == RAW_LOG_SPEC) {
        for (i = 0; i < fe->feature_dimension; i++) {
            mfcep[i] = (mfcc_t) mfspec[i];
        }
    }
    /* For smoothed spectrum, do DCT-II followed by (its inverse) DCT-III */
    else if (fe->log_spec == SMOOTH_LOG_SPEC) {
        /* FIXME: This is probably broken for fixed-point. */
        fe_dct2(fe, mfspec, mfcep, 0);
        fe_dct3(fe, mfcep, mfspec);
        for (i = 0; i < fe->feature_dimension; i++) {
            mfcep[i] = (mfcc_t) mfspec[i];
        }
    }
    else if (fe->transform == DCT_II)
        fe_dct2(fe, mfspec, mfcep, FALSE);
    else if (fe->transform == DCT_HTK)
        fe_dct2(fe, mfspec, mfcep, TRUE);
    else
        fe_spec2cep(fe, mfspec, mfcep);

    return;
}

void
fe_spec2cep(fe_t * fe, const powspec_t * mflogspec, mfcc_t * mfcep)
{
    int32 i, j, beta;

    /* Compute C0 separately (its basis vector is 1) to avoid
     * costly multiplications. */
    mfcep[0] = mflogspec[0] / 2;        /* beta = 0.5 */
    for (j = 1; j < fe->mel_fb->num_filters; j++)
        mfcep[0] += mflogspec[j];       /* beta = 1.0 */
    mfcep[0] /= (frame_t) fe->mel_fb->num_filters;

    for (i = 1; i < fe->num_cepstra; ++i) {
        mfcep[i] = 0;
        for (j = 0; j < fe->mel_fb->num_filters; j++) {
            if (j == 0)
                beta = 1;       /* 0.5 */
            else
                beta = 2;       /* 1.0 */
            mfcep[i] += COSMUL(mflogspec[j],
                               fe->mel_fb->mel_cosine[i][j]) * beta;
        }
        /* Note that this actually normalizes by num_filters, like the
         * original Sphinx front-end, due to the doubled 'beta' factor
         * above.  */
        mfcep[i] /= (frame_t) fe->mel_fb->num_filters * 2;
    }
}

void
fe_dct2(fe_t * fe, const powspec_t * mflogspec, mfcc_t * mfcep, int htk)
{
    int32 i, j;

    /* Compute C0 separately (its basis vector is 1) to avoid
     * costly multiplications. */
    mfcep[0] = mflogspec[0];
    for (j = 1; j < fe->mel_fb->num_filters; j++)
        mfcep[0] += mflogspec[j];
    if (htk)
        mfcep[0] = COSMUL(mfcep[0], fe->mel_fb->sqrt_inv_2n);
    else                        /* sqrt(1/N) = sqrt(2/N) * 1/sqrt(2) */
        mfcep[0] = COSMUL(mfcep[0], fe->mel_fb->sqrt_inv_n);

    for (i = 1; i < fe->num_cepstra; ++i) {
        mfcep[i] = 0;
        for (j = 0; j < fe->mel_fb->num_filters; j++) {
            mfcep[i] += COSMUL(mflogspec[j], fe->mel_fb->mel_cosine[i][j]);
        }
        mfcep[i] = COSMUL(mfcep[i], fe->mel_fb->sqrt_inv_2n);
    }
}

void
fe_lifter(fe_t * fe, mfcc_t * mfcep)
{
    int32 i;

    if (fe->mel_fb->lifter_val == 0)
        return;

    for (i = 0; i < fe->num_cepstra; ++i) {
        mfcep[i] = MFCCMUL(mfcep[i], fe->mel_fb->lifter[i]);
    }
}

void
fe_dct3(fe_t * fe, const mfcc_t * mfcep, powspec_t * mflogspec)
{
    int32 i, j;

    for (i = 0; i < fe->mel_fb->num_filters; ++i) {
        mflogspec[i] = COSMUL(mfcep[0], SQRT_HALF);
        for (j = 1; j < fe->num_cepstra; j++) {
            mflogspec[i] += COSMUL(mfcep[j], fe->mel_fb->mel_cosine[j][i]);
        }
        mflogspec[i] = COSMUL(mflogspec[i], fe->mel_fb->sqrt_inv_2n);
    }
}

int32
fe_write_frame(fe_t * fe, mfcc_t * fea)
{
    fe_spec_magnitude(fe);
    fe_mel_spec(fe);
    if (fe->remove_noise)
        fe_remove_noise(fe->noise_stats, fe->mfspec);
    fe_mel_cep(fe, fea);
    fe_lifter(fe, fea);

    return 1;
}

void *
fe_create_2d(int32 d1, int32 d2, int32 elem_size)
{
    return (void *) ckd_calloc_2d(d1, d2, elem_size);
}

void
fe_free_2d(void *arr)
{
    ckd_free_2d((void **) arr);
}
