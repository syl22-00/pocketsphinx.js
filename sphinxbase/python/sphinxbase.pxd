# Copyright (c) 2008 Carnegie Mellon University. All rights
# reserved.
#
# You may copy, modify, and distribute this code under the same terms
# as PocketSphinx or Python, at your convenience, as long as this
# notice is not removed.
#
# Author: David Huggins-Daines <dhuggins@cs.cmu.edu>

# C declarations
ctypedef float float32
ctypedef int int32
ctypedef double float64

cdef extern from *:
     ctypedef int* const_int_ptr "const int*"
     ctypedef int32* const_int32_ptr "const int32*"
     ctypedef char* const_char_ptr "const char*"

# Get Python stuff to access stdio
cdef extern from "Python.h":
    ctypedef struct FILE
    FILE *PyFile_AsFile(object)
    void *PyMem_Malloc(size_t n)
    void PyMem_Free(void *p)
    object PyString_FromStringAndSize(char *v, Py_ssize_t len)

cdef extern from "sphinxbase/logmath.h":
    ctypedef struct logmath_t
    logmath_t *logmath_init(float64 base, int shift, int use_table)
    logmath_t *logmath_retain(logmath_t *lmath)
    int logmath_free(logmath_t *lmath)

    int logmath_log(logmath_t *lmath, float64 p)
    float64 logmath_exp(logmath_t *lmath, int p)

    int logmath_ln_to_log(logmath_t *lmath, float64 p)
    float64 logmath_log_to_ln(logmath_t *lmath, int p)

    int logmath_log10_to_log(logmath_t *lmath, float64 p)
    float64 logmath_log_to_log10(logmath_t *lmath, int p)

    int logmath_add(logmath_t *lmath, int p, int q)

    int logmath_get_zero(logmath_t *lmath)

cdef extern from "sphinxbase/cmd_ln.h":
    ctypedef struct cmd_ln_t
    ctypedef struct arg_t
    cmd_ln_t *cmd_ln_parse_r(cmd_ln_t *inout_cmdln, arg_t * defn,
                             int argc, char **argv, int strict)
    void cmd_ln_free_r(cmd_ln_t *cmdln)
    float32 cmd_ln_float32_r(cmd_ln_t *cmdln, char *key)
    int32 cmd_ln_int32_r(cmd_ln_t *cmdln, char *key)
    int cmd_ln_boolean_r(cmd_ln_t *cmdln, char *key)
    char *cmd_ln_str_r(cmd_ln_t *cmdln, char *key)

cdef extern from "sphinxbase/ckd_alloc.h":
    void *ckd_calloc(int n, int size)
    char *ckd_salloc(char *str)
    void ckd_free(void *ptr)

cdef extern from "sphinxbase/fsg_model.h":
    ctypedef struct fsg_model_t

cdef extern from "sphinxbase/ngram_model.h":
    ctypedef struct ngram_model_t
    ctypedef enum ngram_file_type_t:
        NGRAM_AUTO
        NGRAM_ARPA
        NGRAM_DMP
        NGRAM_DMP32
    ctypedef enum ngram_case_t:
        NGRAM_UPPER
        NGRAM_LOWER
    ngram_model_t *ngram_model_read(cmd_ln_t *config,
                                    char *file_name,
                                    ngram_file_type_t file_type,
                                    logmath_t *lmath)
    ngram_model_t *ngram_model_set_read(cmd_ln_t *config,
                                        char *lmctlfile,
                                        logmath_t *lmath)
    ngram_model_t *ngram_model_retain(ngram_model_t *model)
    int ngram_model_free(ngram_model_t *model)

    int ngram_model_recode(ngram_model_t *model, char *frum, char *too)
    int ngram_model_casefold(ngram_model_t *model, ngram_case_t kase)
    int ngram_model_write(ngram_model_t *model, char *file_name,
                          ngram_file_type_t format)

    int ngram_model_apply_weights(ngram_model_t *model,
                                  float32 lw, float32 wip, float32 uw)
    float32 ngram_model_get_weights(ngram_model_t *model, int32 *out_log_wip,
                                    int32 *out_log_uw)

    int32 ngram_wid(ngram_model_t *model, char *word)
    char *ngram_word(ngram_model_t *model, int32 wid)
    int32 ngram_unknown_wid(ngram_model_t *model)
    int32 ngram_zero(ngram_model_t *model)

    int32 ngram_ng_score(ngram_model_t *model, int32 wid,
                         int32 *history, int32 n_hist, int32 *n_used)
    int32 ngram_ng_prob(ngram_model_t *model, int32 wid,
                        int32 *history, int32 n_hist, int32 *n_used)

    int32 ngram_model_add_word(ngram_model_t *model,
                               char *word, float32 weight)

    int32 ngram_model_get_size(ngram_model_t *model)
    const_int32_ptr ngram_model_get_counts(ngram_model_t *model)
    ngram_model_t *ngram_model_set_select(ngram_model_t *set, 
                                          char *name)
    ngram_model_t *ngram_model_set_add(ngram_model_t *set,
                                       ngram_model_t *model,
                                       char *name,
                                       float32 weight,
                                       int reuse_widmap)
    ctypedef struct ngram_iter_t
    ngram_iter_t *ngram_ng_iter(ngram_model_t *model, int32 wid,
                                int32 *history, int32 n_hist)
    ngram_iter_t *ngram_model_mgrams(ngram_model_t *model, int m)
    int32 *ngram_iter_get(ngram_iter_t *itor,
                          int32 *out_score,
                          int32 *out_bowt)
    ngram_iter_t *ngram_iter_next(ngram_iter_t *itor)
    ngram_iter_t *ngram_iter_successors(ngram_iter_t *itor)
    void ngram_iter_free(ngram_iter_t *itor)

cdef extern from "sphinxbase/huff_code.h":
    ctypedef struct huff_code_t
    huff_code_t *huff_code_build_int(int *values, int *frequencies, int nvals)
    huff_code_t *huff_code_build_str(char **values, int *frequencies, int nvals)
    huff_code_t *huff_code_read(FILE *infh) except NULL
    int huff_code_write(huff_code_t *hc, FILE *outfh) except -1
    int huff_code_dump(huff_code_t *hc, FILE *dumpfh) except -1
    huff_code_t *huff_code_retain(huff_code_t *hc)
    int huff_code_free(huff_code_t *hc)
    FILE *huff_code_attach(huff_code_t *hc, FILE *fh, char *mode)
    FILE *huff_code_detach(huff_code_t *hc)
    int huff_code_encode_int(huff_code_t *hc, int sym, unsigned int *outcw)
    int huff_code_encode_str(huff_code_t *hc, char *sym, unsigned int *outcw)
    int huff_code_decode_int(huff_code_t *hc, int *outval,
                             char** inout_data, size_t *inout_dlen,
                             int *inout_offset)
    const_char_ptr huff_code_decode_str(huff_code_t *hc,
                               char** inout_data, size_t *inout_dlen,
                               int *inout_offset)

# Extension classes
cdef class NGramModel:
    cdef ngram_model_t *lm
    cdef logmath_t *lmath
    cdef readonly float lw, wip, uw

    cdef set_lm(NGramModel self, ngram_model_t *lm)
    cdef set_lmath(NGramModel self, logmath_t *lmath)

cdef class LogMath:
    cdef logmath_t *lmath

cdef class NGramIter:
    cdef NGramModel lm
    cdef ngram_iter_t *itor
    cdef int first_item
    cdef int m
    cdef readonly float log_prob, log_bowt
    cdef readonly object words

    cdef set_iter(NGramIter self, ngram_iter_t *itor)

cdef class HuffCode:
    cdef huff_code_t *hc
    cdef object fh
