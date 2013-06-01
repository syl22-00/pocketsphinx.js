# Copyright (c) 2008 Carnegie Mellon University. All rights
# reserved.
#
# You may copy, modify, and distribute this code under the same terms
# as PocketSphinx or Python, at your convenience, as long as this
# notice is not removed.
#
# Author: David Huggins-Daines <dhuggins@cs.cmu.edu>

cdef class LogMath:
    """
    Log-space math class.
    
    This class provides fast logarithmic math functions in base
    1.000+epsilon, useful for fixed point speech recognition.

    @param base: The base B in which computation is to be done.
    @type base: float
    @param shift: Log values are shifted right by this many bits.
    @type shift: int
    @param use_table Whether to use an add table or not
    @type use_table: bool
    """
    def __init__(self, base=1.0001, shift=0, use_table=1):
        self.lmath = logmath_init(base, shift, use_table)

    def __dealloc__(self):
        """
        Destructor for LogMath class.
        """
        logmath_free(self.lmath)

    def get_zero(self):
        """
        Get the log-zero value.

        @return: Smallest number representable by this object.
        @rtype: int
        """
        return logmath_get_zero(self.lmath)

    def add(self, a, b):
        """
        Add two numbers in log-space.

        @param a: Logarithm A.
        @type a: int
        @param b: Logarithm B.
        @type b: int
        @return: log(exp(a)+exp(b))
        @rtype: int
        """
        return logmath_add(self.lmath, a, b)

    def log(self, x):
        """
        Return log-value of a number.

        @param x: Number (in linear space)
        @type x: float
        @return: Log-value of x.
        @rtype: int
        """
        return logmath_log(self.lmath, x)

    def exp(self, x):
        """
        Return linear of a log-value

        @param x: Logarithm X (in this object's base)
        @type x: int
        @return: Exponent (linear value) of X.
        @rtype: float
        """
        return logmath_exp(self.lmath, x)

    def log_to_ln(self, x):
        """
        Return natural logarithm of a log-value.

        @param x: Logarithm X (in this object's base)
        @type x: int
        @return: Natural log equivalent of x.
        @rtype: float
        """
        return logmath_log_to_ln(self.lmath, x)

    def log_to_log10(self, x):
        """
        Return logarithm in base 10 of a log-value.

        @param x: Logarithm X (in this object's base)
        @type x: int
        @return: log10 equivalent of x.
        @rtype: float
        """
        return logmath_log_to_log10(self.lmath, x)

    def ln_to_log(self, x):
        """
        Return log-value of a natural logarithm.

        @param x: Logarithm X (in base e)
        @type x: float
        @return: Log-value equivalent of x.
        @rtype: int
        """
        return logmath_ln_to_log(self.lmath, x)

    def log10_to_log(self, x):
        """
        Return log-value of a base 10 logarithm.

        @param x: Logarithm X (in base 10)
        @type x: float
        @return: Log-value equivalent of x.
        @rtype: int
        """
        return logmath_log10_to_log(self.lmath, x)

# Unfortunately, Cython doesn't actually export enums to Python...
AUTO = NGRAM_AUTO
ARPA = NGRAM_ARPA
DMP = NGRAM_DMP
UPPER = NGRAM_UPPER
LOWER = NGRAM_LOWER

cdef class NGramModel:
    """
    N-Gram language model class.

    This class provides access to N-Gram language models stored on
    disk.  These can be in ARPABO text format or Sphinx DMP format.
    Methods are provided for scoring N-Grams based on the model
    and looking up words in the model.

    @param file: Path to an N-Gram model file.
    @type file: string
    @param lw: Language weight to apply to model probabilities.
    @type lw: float
    @param wip: Word insertion penalty to add to model probabilities
    @type wip: float
    @param uw: Weight to give unigrams when interpolating with uniform distribution.
    @type uw: float
    """
    def __init__(self, file=None, lw=1.0, wip=1.0, uw=1.0, lmctl=None):
        self.lmath = logmath_init(1.0001, 0, 0)
        if file:
            self.lm = ngram_model_read(NULL, file, NGRAM_AUTO, self.lmath)
            ngram_model_apply_weights(self.lm, lw, wip, uw)
        elif lmctl:
            self.lm = ngram_model_set_read(NULL, lmctl, self.lmath)
        else:
            self.lm = NULL
        self.lw = lw
        self.wip = wip
        self.uw = uw

    cdef set_lm(NGramModel self, ngram_model_t *lm):
        ngram_model_retain(lm)
        ngram_model_free(self.lm)
        self.lm = lm

    cdef set_lmath(NGramModel self, logmath_t *lmath):
        logmath_retain(lmath)
        logmath_free(self.lmath)
        self.lmath = lmath

    def set_add(NGramModel self, NGramModel lm, name, float weight=1.0, int reuse_widmap=0):
        """
        Adds an language model to the lmset

        @param lm: language model to add
        @type lm: sphinxbase.NGramModel
        @param name: name of the language model 
        @type name: string
        @param weight: language model weight (defaults to 1.0)
        @type weight: float
        @param reuse_widmap: whether to reuse the word ip mapping
        @type reuse_widmap: int
        """
        ngram_model_set_add(self.lm, lm.lm, name, weight, reuse_widmap)

    def set_select(NGramModel self, name):
        """
        Instructs the LMSet to switch to the LM specified by the name param

        @param name: the name associated with the language model
        @type name: string
        """
        ngram_model_set_select(self.lm, name)

    def __dealloc__(self):
        """
        Destructor for N-Gram model class.
        """
        logmath_free(self.lmath)
        ngram_model_free(self.lm)

    def apply_weights(self, lw=1.0, wip=1.0, uw=1.0):
        """
        Change the language model weights applied in L{score}.
        
        @param lw: Language weight to apply to model probabilities.
        @type lw: float
        @param wip: Word insertion penalty to add to model probabilities
        @type wip: float
        @param uw: Weight to give unigrams when interpolating with uniform distribution.
        @type uw: float
        """
        self.lw = lw
        self.wip = wip
        self.uw = uw
        ngram_model_apply_weights(self.lm, lw, wip, uw)

    def get_size(self):
        """
        Get the order of this model (i.e. the 'N' in 'N-gram')

        @return: Order of this model
        @rtype: int
        """
        return ngram_model_get_size(self.lm)

    def get_counts(self):
        """
        Get the counts of each size of N-gram.

        @return: Counts of 1, 2, ..., N grams
        @rtype: tuple(int)
        """
        cdef const_int_ptr counts
        counts = ngram_model_get_counts(self.lm)
        return tuple([counts[i] for i in range(ngram_model_get_size(self.lm))])

    def unknown_wid(self):
        """
        Get the ID for an unknown word.

        In the case of a closed-vocabulary language model this will be -1.

        @return: Word ID for the unknown word.
        @rtype: int
        """
        return ngram_unknown_wid(self.lm)

    def zero(self):
        """
        Get the log-zero value for this language model.

        @return: Log value used to represent zero.
        @rtype: float
        """
        return logmath_log_to_ln(self.lmath, ngram_zero(self.lm))

    def wid(self, word):
        """
        Get the internal ID for a word.
        
        @param word: Word in question
        @type word: string
        @return: Internal ID for word, or -1 if not present
        @rtype: int
        """
        return ngram_wid(self.lm, word)

    def word(self, wid):
        """
        Get the string corresponding to an internal word ID.
        
        @param word: Word ID in question
        @type word: int
        @return: String for word, or None if not present
        @rtype: string
        """
        return ngram_word(self.lm, wid)

    # Note that this and prob() are almost exactly the same...
    def score(self, word, *args):
        """
        Get the score for an N-Gram.

        The argument list consists of the history words (as
        null-terminated strings) of the N-Gram, in reverse order.
        Therefore, if you wanted to get the N-Gram score for 'a whole
        joy', you would call::

         score, n_used = model.score('joy', 'whole', 'a')

        This function returns a tuple, consisting of the score and the
        number of words used in computing it (i.e. the effective size
        of the N-Gram).  The score is returned in logarithmic form,
        using base e.

        If one of the words is not in the LM's vocabulary, the result
        will depend on whether this is an open or closed vocabulary
        language model.  For an open-vocabulary model, unknown words
        are all mapped to the unigram <UNK> which has a non-zero
        probability and also participates in higher-order N-Grams.
        Therefore, you will get a score of some sort in this case.

        For a closed-vocabulary model, unknown words are impossible
        and thus have zero probability.  Therefore, if C{word} is
        unknown, this function will return a 'zero' log-probability,
        i.e. a large negative number.
        """
        cdef int32 wid
        cdef int32 *hist
        cdef int32 n_hist
        cdef int32 n_used
        cdef int32 score
        wid = ngram_wid(self.lm, word)
        n_hist = len(args)
        hist = <int32 *>ckd_calloc(n_hist, sizeof(int32))
        for i from 0 <= i < n_hist:
            spam = args[i]
            hist[i] = ngram_wid(self.lm, spam)
        score = ngram_ng_score(self.lm, wid, hist, n_hist, &n_used)
        ckd_free(hist)
        return logmath_log_to_ln(self.lmath, score), n_used

    def prob(self, word, *args):
        """
        Get the log-probability for an N-Gram.

        This works effectively the same way as L{score}, except that
        any weights (language weight, insertion penalty) applied to
        the language model are ignored and the 'raw' probability value
        is returned.
        """
        cdef int32 wid
        cdef int32 *hist
        cdef int32 n_hist
        cdef int32 n_used
        cdef int32 score
        wid = ngram_wid(self.lm, word)
        n_hist = len(args)
        hist = <int32 *>ckd_calloc(n_hist, sizeof(int32))
        for i from 0 <= i < n_hist:
            spam = args[i]
            hist[i] = ngram_wid(self.lm, spam)
        score = ngram_ng_prob(self.lm, wid, hist, n_hist, &n_used)
        ckd_free(hist)
        return logmath_log_to_ln(self.lmath, score), n_used

    def mgrams(self, m):
        """
        Return an iterator over each N-gram of order m+1.

        This allows Pythonic iteration over the parameters of an
        N-Gram model.

        @param m: Order of requested N-grams minus one
        @type m: int
        @return: Iterator over M+1-grams
        @rtype: NGramIter
        """
        cdef NGramIter itor
        itor = NGramIter(self, m)
        itor.itor = ngram_model_mgrams(self.lm, m)
        return itor

    def ngram(self, word, *args):
        """
        Return an N-Gram iterator pointing to a given N-gram.

        This allows you to iterate over its successors among other
        things.

        @param word: Head word of requested N-gram.
        @type word: str
        @param args: History words of requested N-gram
        @type args: str
        @return: Iterator pointing to N-gram.
        """
        cdef NGramIter itor
        cdef int32 wid
        cdef int32 *hist
        cdef int32 n_hist
        wid = ngram_wid(self.lm, word)
        n_hist = len(args)
        hist = <int32 *>ckd_calloc(n_hist, sizeof(int32))
        for i from 0 <= i < n_hist:
            spam = args[i]
            hist[i] = ngram_wid(self.lm, spam)
        itor = NGramIter(self, n_hist)
        # We do set_iter here, because we're returning something the
        # user is immediately going to do stuff with.
        itor.set_iter(ngram_ng_iter(self.lm, wid, hist, n_hist))
        ckd_free(hist)
        return itor

    def add_word(self, word, weight=1.0):
        return ngram_model_add_word(self.lm, word, weight)

    def recode(self, frum, too):
        cdef int rv
        rv = ngram_model_recode(self.lm, frum, too)
        if rv == -1:
            raise ValueError, "Recode from %s to %s failed" % (frum, too)

    def casefold(self, kase):
        cdef int rv
        rv = ngram_model_casefold(self.lm, kase)
        if rv == -1:
            raise ValueError, "Casefolding failed"

    def write(self, file_name, format=NGRAM_AUTO):
        cdef int rv
        rv = ngram_model_write(self.lm, file_name, format)
        if rv == -1:
            raise ValueError, "Write %s to file failed" % file_name

cdef class NGramIter:
    """
    N-Gram language model iterator class.

    This class provides access to the individual N-grams stored in a
    language model.
    """
    def __cinit__(self, NGramModel lm, int m):
        self.itor = NULL
        self.lm = lm
        self.m = m
        self.first_item = True

    def __iter__(self):
        self.first_item = True
        return self

    cdef set_iter(NGramIter self, ngram_iter_t *itor):
        cdef int32 prob, bowt
        cdef const_int32_ptr wids

        if itor == NULL:
            raise StopIteration
        self.itor = itor
        if self.first_item:
            self.first_item = False
        wids = ngram_iter_get(itor, &prob, &bowt)
        self.log_prob = logmath_log_to_ln(self.lm.lmath, prob)
        self.log_bowt = logmath_log_to_ln(self.lm.lmath, bowt)
        self.words = []
        for i in range(0, self.m+1):
            self.words.append(ngram_word(self.lm.lm, wids[i]))

    def __next__(self):
        if self.first_item:
            self.set_iter(self.itor)
        else:
            self.set_iter(ngram_iter_next(self.itor))
        return self

    def successors(self):
        """
        Get an iterator over N+1-gram successors of an N-gram.
        """
        cdef NGramIter itor
        itor = NGramIter(self.lm, self.m + 1)
        itor.itor = ngram_iter_successors(self.itor)
        return itor

def binstr(str val, int nbits):
    """
    Silly function to format a string as a binary string
    """
    cdef int i
    outstr = ""
    for c in val:
        cval = ord(c)
        cnb = min(8, nbits)
        for i in range(0,cnb):
            outstr += "%d" % ((cval & (1 << 7-i)) != 0)
        nbits -= 8
    return outstr

def bincw(int cw, int nbits):
    """
    Silly function to format an int as a binary string
    """
    cdef int i
    outstr = ""
    for i in range(0,nbits):
        outstr = "%s" % (cw & 1) + outstr
        cw >>= 1
    return outstr

# FIXME: Due to the style of IO in huff_code API this part of the code
# is not compatible with Python 3. This needs to be converted to
# the new Python io module.

cdef class HuffCode:
    """
    Huffman coding class.

    You can either construct a Huffman code from an alphabet of
    symbols with frequencies, or read one from a file.  Either the
    alphabet or infile argument (but not both) must be passed to the
    constructor.

    @param alphabet: Alphabet of (symbol, frequency) pairs
    @type alphabet: [(str, int)]
    @param infile: File handle or filename to read from
    @type infile: file | str
    """
    def __init__(self, alphabet=None, infile=None):
        cdef char **symbols
        cdef int *frequencies
        cdef int nsym

        if alphabet == None and infile == None:
            raise ValueError, "One of alphabet or infile must be passed to constructor"
        if alphabet != None and infile != None:
            raise ValueError, "Only one of alphabet or infile must be passed to constructor"

        self.fh = None
        if infile:
            self.read(infile)
            return

        nsym = len(alphabet)
        frequencies = <int *>ckd_calloc(nsym, sizeof(int))
        symbols = <char **>ckd_calloc(nsym, sizeof(char *))
        # Need to create separate Python objects for each string,
        # otherwise we get random duplicates of the codewords...
        bogus = []
        for i, spam in enumerate(alphabet):
            sym, freq = spam
            bogus.append(repr(sym))
            frequencies[i] = freq
            symbols[i] = bogus[-1]
        self.hc = huff_code_build_str(symbols, frequencies, nsym)
        ckd_free(frequencies)
        ckd_free(symbols)

    def read(self, infile):
        if not isinstance(infile, file):
            infile = file(infile, "rb")
        huff_code_free(self.hc)
        self.hc = huff_code_read(PyFile_AsFile(infile))

    def write(self, outfile):
        if not isinstance(outfile, file):
            outfile = file(outfile, "wb")
        huff_code_write(self.hc, PyFile_AsFile(outfile))

    def dump(self, outfile):
        if not isinstance(outfile, file):
            outfile = file(outfile, "w")
        huff_code_dump(self.hc, PyFile_AsFile(outfile))

    def encode(self, seq):
        """
        Encode a sequence of symbols to a byte array, returning that
        array and the bit offset of the next codeword in the last
        byte (i.e. 8 minutes the number of extra zero bits)
        """
        cdef unsigned int cw
        cdef int cwlen, nbits = 0, nbytes, offset, i
        cdef unsigned char buf = 0
        cdef char *output
        
        for sym in seq:
            sss = repr(sym)
            cwlen = huff_code_encode_str(self.hc, sss, &cw)
            nbits += cwlen
        nbytes = int((nbits + 7) / 8)
        offset = 0
        output = <char *>PyMem_Malloc(nbytes + 1)
        output[nbytes] = 0
        i = 0
        nbits = 0
        for sym in seq:
            sss = repr(sym)
            cwlen = huff_code_encode_str(self.hc, sss, &cw)
            #print "sym: %s cw: %s buf: %s output: %s" \
            #      % (sym, bincw(cw, cwlen), bincw(buf >> (8-offset), offset),
            #         binstr(output, nbits))
            #print "cwlen",cwlen
            # Do one byte at a time while full bytes are available
            while cwlen >= 8:
                # Fill low bits of buf with high bits of cw
                buf |= (cw >> (cwlen - (8 - offset))) & ((1 << (8 - offset)) - 1)
                # Append buf to output
                output[i] = buf
                i += 1
                nbits += 8
                # Fill high bits of buf with low bits of this byte
                cwlen -= 8
                buf = (cw >> cwlen) & ((1 << offset) - 1)
                buf <<= (8-offset)
                #print "cwlen",cwlen
            # Now cwlen will be less than 8, but it might still be
            # more than the available space in buf.
            if cwlen >= (8 - offset):
                # Fill low bits of buf with (8-offset) highest bits of cw
                buf |= (cw >> (cwlen - (8 - offset))) & ((1 << (8 - offset)) - 1)
                # Append buf to output
                output[i] = buf
                i += 1
                nbits += 8
                # cwlen is down to the remaining bits
                cwlen -= (8 - offset)
                # Offset is now zero since we just completed and emptied buf
                offset = 0
                # buf is zero, because we just emptied it without putting stuff in
                buf = 0
                #print "cwlen",cwlen
                # Any remaining  bits will be taken care of below (we hope)
            # Add remaining high bits of cw to low bits of buf
            #print "cwlen",cwlen
            buf |= ((cw & ((1 << cwlen) - 1)) << (8 - offset - cwlen))
            offset += cwlen
            #print "after buf: %s output: %s" \
            #      % (bincw(buf >> (8-offset), offset), binstr(output, nbits))
        if offset > 0:
            # Append buf to output
            output[i] = buf
            nbits += offset
            i += 1
        #print "output:", binstr(output, nbits)
        outstr = PyString_FromStringAndSize(output, nbytes)
        PyMem_Free(output)
        return (outstr, offset)

    def decode(self, data):
        """
        Decode a sequence of symbols from a string, returning the
        sequence and the bit offset of the next codeword in the last
        byte (i.e. 8 minutes the number of remaining bits)
        """
        cdef int offset
        cdef const_char_ptr dptr
        cdef const_char_ptr strval
        cdef size_t dlen

        dlen = len(data)
        offset = 0
        dptr = data
        output = []
        while True:
            strval = huff_code_decode_str(self.hc, &dptr, &dlen, &offset)
            if strval == NULL:
                break
            output.append(strval)
        if dlen > 1:
            raise ValueError, "Invalid data at position %d" % (len(data) - dlen)
        return (output, offset)

    def attach(self, fh, char *mode):
        if not isinstance(fh, file):
            fh = file(fh, mode)
        self.fh = fh
        huff_code_attach(self.hc, PyFile_AsFile(fh), mode)

    def detach(self):
        huff_code_detach(self.hc)
        self.fh = None

    def encode_to_file(self, seq):
        if self.fh == None:
            raise RuntimeError, "No file is attached"
        for sym in seq:
            strsym = repr(sym)
            huff_code_encode_str(self.hc, strsym, NULL)

    def decode_from_file(self):
        cdef const_char_ptr sym
        if self.fh == None:
            raise RuntimeError, "No file is attached"
        sym = huff_code_decode_str(self.hc, NULL, NULL, NULL)
        if sym == NULL:
            return None
        else:
            return sym

    def __dealloc__(self):
        if self.fh:
            self.detach()
        huff_code_free(self.hc)
