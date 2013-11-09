/* -*- c-basic-offset: 4; indent-tabs-mode: nil -*- */
/* ====================================================================
 * Copyright (c) 2013 Carnegie Mellon University.  All rights 
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

/* This noise removal algorithm is inspired by the following papers
 * Computationally Efficient Speech Enchancement by Spectral Minina Tracking
 * by G. Doblinger
 *
 * Power-Normalized Cepstral Coefficients (PNCC) for Robust Speech Recognition
 * by C. Kim.
 *
 * For the recent research and state of art see papers about IMRCA and
 * A Minimum-Mean-Square-Error Noise Reduction Algorithm On Mel-Frequency
 * Cepstra For Robust Speech Recognition by Dong Yu and others
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "sphinxbase/prim_type.h"
#include "sphinxbase/ckd_alloc.h"

#include "fe_noise.h"

struct noise_stats_s {
    /* Smoothed power */
    powspec_t *power;
    /* Noise estimate */
    powspec_t *noise;
    /* Signal floor estimate */
    powspec_t *floor;
    /* Peak for temporal masking */
    powspec_t *peak;

    /* Initialize it next time */
    uint8 undefined;
    /* Number of items to process */
    uint32 num_filters;
};

/* Noise supression constants */
#define SMOOTH_WINDOW 4

#define LAMBDA_POWER 0.7
#define LAMBDA_A 0.999
#define LAMBDA_B 0.5
#define LAMBDA_T 0.85
#define MU_T 0.2
#define EXCITATION_THRESHOLD 2.0
#define LOG_FLOOR 1e-4
#define MAX_GAIN 20

#define EPS 1e-10

static void
fe_low_envelope(powspec_t * buf, powspec_t * floor_buf, int32 num_filt)
{
    int i;

    for (i = 0; i < num_filt; i++) {
        if (buf[i] >= floor_buf[i]) {
            floor_buf[i] =
                LAMBDA_A * floor_buf[i] + (1 - LAMBDA_A) * buf[i];
        }
        else {
            floor_buf[i] =
                LAMBDA_B * floor_buf[i] + (1 - LAMBDA_B) * buf[i];
        }
    }
}

/* temporal masking */
static void
fe_temp_masking(powspec_t * buf, powspec_t * peak, int32 num_filt)
{
    powspec_t cur_in;
    int i;

    for (i = 0; i < num_filt; i++) {
        cur_in = buf[i];

        peak[i] *= LAMBDA_T;
        if (buf[i] < LAMBDA_T * peak[i])
            buf[i] = peak[i] * MU_T;

        if (cur_in > peak[i])
            peak[i] = cur_in;
    }
}

/* spectral weight smoothing */
static void
fe_weight_smooth(powspec_t * buf, powspec_t * coefs, int32 num_filt)
{
    int i, j;
    int l1, l2;
    float32 coef;

    for (i = 0; i < num_filt; i++) {
        l1 = ((i - SMOOTH_WINDOW) > 0) ? (i - SMOOTH_WINDOW) : 0;
        l2 = ((i + SMOOTH_WINDOW) <
              (num_filt - 1)) ? (i + SMOOTH_WINDOW) : (num_filt - 1);
        coef = 0;
        for (j = l1; j <= l2; j++) {
            coef += coefs[j];
        }
        coef = coef / (l2 - l1 + 1);
        buf[i] *= coef;
    }
}

noise_stats_t *
fe_init_noisestats(int num_filters)
{
    noise_stats_t *noise_stats;

    noise_stats = (noise_stats_t *) ckd_calloc(1, sizeof(noise_stats_t));

    noise_stats->power =
        (powspec_t *) ckd_calloc(num_filters, sizeof(powspec_t));
    noise_stats->noise =
        (powspec_t *) ckd_calloc(num_filters, sizeof(powspec_t));
    noise_stats->floor =
        (powspec_t *) ckd_calloc(num_filters, sizeof(powspec_t));
    noise_stats->peak =
        (powspec_t *) ckd_calloc(num_filters, sizeof(powspec_t));

    noise_stats->undefined = TRUE;
    noise_stats->num_filters = num_filters;

    return noise_stats;
}

void
fe_reset_noisestats(noise_stats_t * noise_stats)
{
    noise_stats->undefined = TRUE;
}

void
fe_free_noisestats(noise_stats_t * noise_stats)
{
    ckd_free(noise_stats->power);
    ckd_free(noise_stats->noise);
    ckd_free(noise_stats->floor);
    ckd_free(noise_stats->peak);
    ckd_free(noise_stats);
}

void
fe_remove_noise(noise_stats_t * noise_stats, powspec_t * mfspec)
{
    powspec_t *signal;
    powspec_t *gain;
    int32 i, num_filts;

    num_filts = noise_stats->num_filters;

    signal = (powspec_t *) ckd_calloc(num_filts, sizeof(powspec_t));
    gain = (powspec_t *) ckd_calloc(num_filts, sizeof(powspec_t));

    if (noise_stats->undefined) {
        for (i = 0; i < num_filts; i++) {
            noise_stats->power[i] = mfspec[i];
            noise_stats->noise[i] = mfspec[i];
            noise_stats->floor[i] = mfspec[i] / MAX_GAIN;
            noise_stats->peak[i] = 0.0;
        }
        noise_stats->undefined = FALSE;
    }

    /* Calculate smoothed power */
    for (i = 0; i < num_filts; i++) {
        noise_stats->power[i] =
            LAMBDA_POWER * noise_stats->power[i] + (1 -
                                                   LAMBDA_POWER) *
            mfspec[i];
    }

    /* Noise estimation */
    fe_low_envelope(noise_stats->power, noise_stats->noise, num_filts);

    for (i = 0; i < num_filts; i++) {
        signal[i] = noise_stats->power[i] - noise_stats->noise[i];
        if (signal[i] < 0)
            signal[i] = 0;
    }

    fe_low_envelope(signal, noise_stats->floor, num_filts);

    fe_temp_masking(signal, noise_stats->peak, num_filts);

    for (i = 0; i < num_filts; i++) {
        //zero (or close to it) region should be substituted with floor envelope
        if (signal[i] < noise_stats->floor[i])
            signal[i] = noise_stats->floor[i];
        //non-excitation segment
        if (signal[i] < EXCITATION_THRESHOLD * noise_stats->noise[i])
            signal[i] = noise_stats->floor[i];
    }

    for (i = 0; i < num_filts; i++) {
        gain[i] = signal[i] / (noise_stats->power[i] + EPS);
        if (gain[i] > MAX_GAIN)
            gain[i] = MAX_GAIN;
        if (gain[i] < 1.0 / MAX_GAIN)
            gain[i] = 1.0 / MAX_GAIN;
    }

    /* Weight smoothing and time frequency normalization */
    fe_weight_smooth(mfspec, gain, num_filts);

    ckd_free(signal);
    ckd_free(gain);
}
