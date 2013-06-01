#!/usr/bin/env python

import sphinxbase
import sys

lm = sphinxbase.NGramModel("../test/unit/test_ngram/100.arpa.DMP")
for ng in lm.mgrams(0):
    print ng.log_prob, ng.log_bowt
lm.write("100.arpa.DMP")
lm.casefold(sphinxbase.UPPER)
lm.write("100.arpa")

hc = sphinxbase.HuffCode((("foo", 42), ("bar", 4), ("baz", 5), ("quux", 6), ("argh", 225), ("hurf", 15001), ("burf", 3), ("blatz", 2), ("unf", 87), ("woof", 1003)))
hc.dump(sys.stdout)
data, bits = hc.encode(("hurf", "burf", "blatz", "unf", "woof"))
dstr = "".join([("%02x" % ord(b)) for b in data])
print "encoding", ("hurf", "burf", "blatz", "unf", "woof"), "=>", (dstr, bits)
print "decoded to", hc.decode(data)

hc.write("foo.huff")
hc = sphinxbase.HuffCode(infile="foo.huff")
data, bits = hc.encode(("hurf", "burf", "blatz", "unf", "woof"))
dstr = "".join([("%02x" % ord(b)) for b in data])
print "encoding", ("hurf", "burf", "blatz", "unf", "woof"), "=>", (dstr, bits)
print "decoded to", hc.decode(data)

hc.attach("foo.bin", "wb")
hc.encode_to_file(("hurf", "burf", "blatz", "unf", "woof"))
hc.encode_to_file(("burf", "blatz", "woof", "unf", "woof"))
hc.detach()

hc.attach("foo.bin", "rb")
syms = []
while True:
    sym = hc.decode_from_file()
    if sym == None:
        break
    syms.append(sym)
print "decoded", syms
hc.detach()
