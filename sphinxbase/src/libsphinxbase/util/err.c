/* -*- c-basic-offset: 4; indent-tabs-mode: nil -*- */
/* ====================================================================
 * Copyright (c) 1999-2004 Carnegie Mellon University.  All rights
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
/**
 * @file err.c
 * @brief Somewhat antiquated logging and error interface.
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#include "sphinxbase/err.h"

#ifdef SPHINX_DEBUG
static int sphinx_debug_level;
int
err_set_debug_level(int level)
{
    int prev = sphinx_debug_level;
    sphinx_debug_level = level;
    return prev;
}
int
err_get_debug_level(void)
{
    return sphinx_debug_level;
}
#else
int
err_set_debug_level(int level)
{
    return 0;
}

int
err_get_debug_level(void)
{
    return 0;
}
#endif

#if defined(HAVE_PTHREAD_H)
#include <pthread.h>
static pthread_key_t logfp_index;
static pthread_once_t logfp_index_once = PTHREAD_ONCE_INIT;

void
logfp_index_alloc(void)
{
    pthread_key_create(&logfp_index, NULL);
}

FILE *
err_get_logfp(void)
{
    FILE *logfp;

    pthread_once(&logfp_index_once, logfp_index_alloc);
    logfp = (FILE *)pthread_getspecific(logfp_index);

    if (logfp == NULL)
        return stderr;
    else if (logfp == (FILE*) -1)
	return NULL;
    else
        return logfp;
}

static void
internal_set_logfp(FILE *fh)
{
    if (fh == NULL)
	fh = (FILE*) -1;

    pthread_setspecific(logfp_index, (void *)fh);
}

#elif defined(_WIN32) || defined(__CYGWIN__) /* Use Windows TLS on Cygwin */
#include <windows.h>
static DWORD logfp_index; /** TLS index for log file */
static LONG logfp_index_once = 0; /** True if we have initialized TLS */

void
logfp_index_alloc(void)
{
    logfp_index = TlsAlloc();
}

FILE *
err_get_logfp(void)
{
    FILE *logfp;

    if (InterlockedExchange(&logfp_index_once, 1) == 0)
        logfp_index_alloc();
    logfp = (FILE *)TlsGetValue(logfp_index);

    if (logfp == NULL)
        return stderr;
    else if (logfp == (FILE*) -1)
	return NULL;
    else
        return logfp;
}

static void
internal_set_logfp(FILE *fh)
{
    if (fh == NULL)
	fh = (FILE*) -1;

    TlsSetValue(logfp_index, (void *)fh);
}

#else
FILE *logfp = NULL;

FILE *
err_get_logfp(void)
{
    if (logfp == NULL)
        return stderr;
    else if (logfp == (FILE*) -1)
	return NULL;
    else
        return logfp;
}

static void
internal_set_logfp(FILE *fh)
{
    if (fh == NULL)
	fh = (FILE*) -1;

    logfp = fh;
}

#endif
 
FILE *
err_set_logfp(FILE *newfp)
{
    FILE *oldfp;

    oldfp = err_get_logfp();
    internal_set_logfp(newfp);

    return oldfp;
}

int
err_set_logfile(char const *file)
{
    FILE *newfp, *oldfp;

    if ((newfp = fopen(file, "a")) == NULL)
        return -1;
    oldfp = err_get_logfp();
    internal_set_logfp(newfp);
    if (oldfp != NULL && oldfp != stdout && oldfp != stderr)
        fclose(oldfp);
    return 0;
}


void
_E__pr_info_header_wofn(char const *msg)
{
    FILE *logfp;

    logfp = err_get_logfp();
    if (logfp == NULL)
        return;
    /* make different format so as not to be parsed by emacs compile */
    fprintf(logfp, "%s:\t", msg);
    fflush(logfp);
}

void
_E__pr_header(char const *f, long ln, char const *msg)
{
    char const *fname;
    FILE *logfp;

    logfp = err_get_logfp();
    if (logfp == NULL)
        return;
    fname = strrchr(f,'\\');
    if (fname == NULL)
        fname = strrchr(f,'/');
    fprintf(logfp, "%s: \"%s\", line %ld: ", msg, fname == NULL ? f : fname + 1, ln);
    fflush(logfp);
}

void
_E__pr_info_header(char const *f, long ln, char const *msg)
{
    char const *fname;
    FILE *logfp;

    logfp = err_get_logfp();
    if (logfp == NULL)
        return;
    fname = strrchr(f,'\\');
    if (fname == NULL)
        fname = strrchr(f,'/');
    /* make different format so as not to be parsed by emacs compile */
    fprintf(logfp, "%s: %s(%ld): ", msg, fname == NULL ? f : fname + 1, ln);
    fflush(logfp);
}

void
_E__pr_warn(char const *fmt, ...)
{
    va_list pvar;
    FILE *logfp;

    logfp = err_get_logfp();
    if (logfp == NULL)
        return;
    va_start(pvar, fmt);
    vfprintf(logfp, fmt, pvar);
    va_end(pvar);

    fflush(logfp);
}

void
_E__pr_info(char const *fmt, ...)
{
    va_list pvar;
    FILE *logfp;

    logfp = err_get_logfp();
    if (logfp == NULL)
        return;
    va_start(pvar, fmt);
    vfprintf(logfp, fmt, pvar);
    va_end(pvar);

    fflush(logfp);
}

void
_E__die_error(char const *fmt, ...)
{
    va_list pvar;
    FILE *logfp;

    logfp = err_get_logfp();
    if (logfp) {
        va_start(pvar, fmt);
        vfprintf(logfp, fmt, pvar);
        va_end(pvar);
        fflush(logfp);
    }

#if defined(__ADSPBLACKFIN__) && !defined(__linux__)
    while(1);
#else 
	exit(-1);
#endif
}

void
_E__fatal_sys_error(char const *fmt, ...)
{
    va_list pvar;
    FILE *logfp;
    int local_errno = errno;

    logfp = err_get_logfp();
    if (logfp) {
        va_start(pvar, fmt);
        vfprintf(logfp, fmt, pvar);
        va_end(pvar);

        fprintf(logfp, ": %s\n", strerror(local_errno));
        fflush(logfp);
    }


#if defined(__ADSPBLACKFIN__) && !defined(__linux__)
    while(1);
#else 
	exit(-1);
#endif

}

void
_E__sys_error(char const *fmt, ...)
{
    va_list pvar;
    FILE *logfp;
    int local_errno = errno;

    logfp = err_get_logfp();
    if (logfp == NULL)
        return;

    va_start(pvar, fmt);
    vfprintf(logfp, fmt, pvar);
    va_end(pvar);

    fprintf(logfp, ": %s\n", strerror(local_errno));
    fflush(logfp);
}

void
_E__abort_error(char const *fmt, ...)
{
    va_list pvar;
    FILE *logfp;

    logfp = err_get_logfp();
    if (logfp) {
        va_start(pvar, fmt);
        vfprintf(logfp, fmt, pvar);
        va_end(pvar);
        fflush(logfp);
    }

#if defined(__ADSPBLACKFIN__) && !defined(__linux__)
while(1);
#elif defined(_WIN32_WCE)
exit(-1);
#else
abort();
#endif

}
