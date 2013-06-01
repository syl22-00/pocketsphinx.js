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
/*
 * err_wince.c -- Package for checking and catching common errors, printing out
 *		errors nicely, etc.
 * WinCE has no standard library worth the name so we need this
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <assert.h>

#include "sphinxbase/err.h"
#include "sphinxbase/ckd_alloc.h"

int cst_verrmsg(const char *fmt, va_list args)
{
    char msg[256];
    WCHAR *wmsg;
    size_t len;

    _vsnprintf(msg,256,fmt,args);

    len = mbstowcs(NULL,msg,0) + 1;
    wmsg = ckd_calloc(len,sizeof(*wmsg));
    mbstowcs(wmsg,msg,len);

    OutputDebugStringW(wmsg);
    ckd_free(wmsg);
    return 0;
}

int cst_errmsg(const char *fmt, ...)
{
    va_list args;

    va_start(args,fmt);
    cst_verrmsg(fmt, args);
    va_end(args);
    return 0;
}

void
_E__pr_info_header_wofn(char const *msg)
{
    cst_errmsg("%s:\t", msg);
}

void
_E__pr_header(char const *f, long ln, char const *msg)
{
    cst_errmsg("%s: \"%s\", line %ld: ", msg, f, ln);
}

void
_E__pr_info_header(char const *f, long ln, char const *msg)
{
    cst_errmsg("%s: %s(%ld): ", msg, f, ln);
}

void
_E__pr_warn(char const *fmt, ...)
{
    va_list pvar;

    va_start(pvar, fmt);
    cst_verrmsg(fmt, pvar);
    va_end(pvar);
}

void
_E__pr_info(char const *fmt, ...)
{
    va_list pvar;

    va_start(pvar, fmt);
    cst_verrmsg(fmt, pvar);
    va_end(pvar);
}

void
_E__die_error(char const *fmt, ...)
{
    va_list pvar;

    va_start(pvar, fmt);
    cst_verrmsg(fmt, pvar);
    va_end(pvar);
    exit(-1);
}

void
_E__fatal_sys_error(char const *fmt, ...)
{
    LPVOID msg_buf;
    DWORD error;
    va_list pvar;

    error = GetLastError();
    va_start(pvar, fmt);
    cst_verrmsg(fmt, pvar);
    va_end(pvar);

    OutputDebugStringW(L"; ");
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                  FORMAT_MESSAGE_FROM_SYSTEM | 
                  FORMAT_MESSAGE_IGNORE_INSERTS,
                  NULL,
                  error,
                  0, // Default language
                  (LPTSTR) &msg_buf,
                  0,
                  NULL);
    OutputDebugString(msg_buf);
    LocalFree(msg_buf);

    exit(error);
}

void
_E__sys_error(char const *fmt, ...)
{
    LPVOID msg_buf;
    DWORD error;
    va_list pvar;

    error = GetLastError();
    va_start(pvar, fmt);
    cst_verrmsg(fmt, pvar);
    va_end(pvar);

    OutputDebugStringW(L"; ");
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                  FORMAT_MESSAGE_FROM_SYSTEM | 
                  FORMAT_MESSAGE_IGNORE_INSERTS,
                  NULL,
                  error,
                  0, // Default language
                  (LPTSTR) &msg_buf,
                  0,
                  NULL);
    OutputDebugString(msg_buf);
    LocalFree(msg_buf);
}

void
_E__abort_error(char const *fmt, ...)
{
    va_list pvar;

    va_start(pvar, fmt);
    cst_verrmsg(fmt, pvar);
    va_end(pvar);

    assert(2+2 == 5);
}
