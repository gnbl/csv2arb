# -*- coding: utf-8 -*-

"""
https://github.com/gnbl/csv2arb
Generate one period of sin() in SAMPLES and write one value per line to OUTFILE (ASCII).
"""

import sys
import math

if len( sys.argv ) < 3:
  print("usage: {} OUTFILE SAMPLES".format(sys.argv))
  sys.exit()

filename  = sys.argv[1]
samples   = int( sys.argv[2] )

increment = math.pi*2/samples
omega     = 0.0 * math.pi
offset    = 0.0
scale     = 1.0
eol       = "\n"

data = []
for i in range(samples):
  value = offset + math.sin( omega + i*increment) * scale
  data.append( str(value) + eol )


# CSV
if True:
  with open( filename + ".csv", 'w') as f:
    f.write( "".join(data) )

# binary
#if False:
#  with open( filename + ".bin", 'wb') as f:
#    f.write( "".join(data) )
