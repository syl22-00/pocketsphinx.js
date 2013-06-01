#!/usr/bin/env python

import sphinxbase
import sys

hc = sphinxbase.HuffCode(((1, 42), (2, 4), (3, 5), (4, 6), (5, 225), (6, 15001), (7, 3), (8, 2), (9, 87), (10, 1003)))
hc.dump(sys.stdout)
data, bits = hc.encode((1,2,3,4,5))
dstr = "".join([("%02x" % ord(b)) for b in data])
print "encoding", (1,2,3,4,5), "=>", (dstr, bits)
print "decoded to", hc.decode(data)
