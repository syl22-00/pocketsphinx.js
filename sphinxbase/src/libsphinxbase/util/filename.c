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
 * filename.c -- File and path name operations.
 *
 * **********************************************
 * CMU ARPA Speech Project
 *
 * Copyright (c) 1999 Carnegie Mellon University.
 * ALL RIGHTS RESERVED.
 * **********************************************
 * 
 * HISTORY
 * $Log: filename.c,v $
 * Revision 1.5  2005/06/22 03:01:07  arthchan2003
 * Added  keyword
 *
 * Revision 1.3  2005/03/30 01:22:48  archan
 * Fixed mistakes in last updates. Add
 *
 * 
 * 30-Oct-1997	M K Ravishankar (rkm@cs.cmu.edu) at Carnegie Mellon University.
 * 		Started.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "sphinxbase/filename.h"

#ifdef _MSC_VER
#pragma warning (disable: 4996)
#endif


/* Strip off all leading pathname components */
void
path2basename(const char *path, char *base)
{
    int32 i, l;

    l = strlen(path);
#if defined(_WIN32) || defined(__CYGWIN__)
    for (i = l - 1; (i >= 0) && !(path[i] == '/' || path[i] == '\\'); --i);
#else
    for (i = l - 1; (i >= 0) && !(path[i] == '/'); --i);
#endif
    strcpy(base, path + i + 1);
}

/* Return all leading pathname components */
void
path2dirname(const char *path, char *dir)
{
    int32 i, l;

    l = strlen(path);
#if defined(_WIN32) || defined(__CYGWIN__)
    for (i = l - 1; (i >= 0) && !(path[i] == '/' || path[i] == '\\'); --i);
#else
    for (i = l - 1; (i >= 0) && !(path[i] == '/'); --i);
#endif
    if (i <= 0)
        dir[0] = '\0';
    else {
        memcpy(dir, path, i);
        dir[i] = '\0';
    }
}


/* Strip off the shortest trailing .xyz suffix */
void
strip_fileext(const char *path, char *root)
{
    int32 i, l;

    l = strlen(path);
    for (i = l - 1; (i >= 0) && (path[i] != '.'); --i);
    if (i < 0)
        strcpy(root, path);     /* Didn't find a . */
    else {
        strncpy(root, path, i);
    }
}

/* Test if this path is absolute. */
int
path_is_absolute(const char *path)
{
#if defined(_WIN32) && !defined(_WIN32_WCE) /* FIXME: Also SymbianOS */
    return /* Starts with drive letter : \ or / */
        (strlen(path) >= 3
         &&
         ((path[0] >= 'A' && path[0] <= 'Z')
          || (path[0] >= 'a' && path[0] <= 'z'))
         && path[1] == ':'
         && (path[2] == '/' || path[2] == '\\'));
#elif defined(_WIN32_WCE)
    return path[0] == '\\' || path[0] == '/';
#else /* Assume Unix */
    return path[0] == '/';
#endif
}
