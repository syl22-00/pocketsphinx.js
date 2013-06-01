/* -*- c-basic-offset: 4; indent-tabs-mode: nil -*- */
/**
 * @file test_filename.c Test file name operations
 * @author David Huggins-Daines <dhuggins@cs.cmu.edu>
 */

#include "filename.h"
#include "test_macros.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int
main(int argc, char *argv[])
{
    char const testname[] = "/foo/bar/baz/quux.argh";
    char const testname2[] = "foo/bar/baz";
    char const testname3[] = "/foo";
    char testout[32];

    path2basename(testname, testout);
    TEST_EQUAL(0, strcmp("quux.argh", testout));

    path2dirname(testname, testout);
    TEST_EQUAL(0, strcmp("/foo/bar/baz", testout));

    path2dirname(testname2, testout);
    TEST_EQUAL(0, strcmp("foo/bar", testout));

    path2dirname(testname3, testout);
    TEST_EQUAL(0, strcmp("", testout));

    return 0;
}
