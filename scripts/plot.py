__author__ = 'dimaxx'

import matplotlib.pyplot as plt
import numpy
from numpy import *

max_files = 5
each = 1
gas_num = 2
params = ["conc", "temp", "pressure"]
#params = ["conc"]

for param in params:
  for gas in range(gas_num):
    data_folder = '../out/gas' + '%i' % gas + '/'
    i = 0
    while i < max_files:
      s = "%i" % i
      input = numpy.fromfile(data_folder + param + '/data/'+s+'.bin', dtype=float)

      # reading additional info
      NX = int(input[0])
      NY = int(input[1])

      input = input[2:]

      # hack
      a = NX
      NX = NY
      NY = a

      max_val = float(-1e6)
      min_val = float(1e6)
      D = input.reshape(NX, NY)
      for x in range(NX):
        for y in range(NY):
          v = D[x][y]
          if v == 0.0:
            D[x][y] = nan;
          else:
            max_val = max(max_val, v)
            min_val = min(min_val, v)

      if math.isnan(min_val) or math.isnan(max_val) == nan:
        print 'max or min are nan!'
        plt.imshow(D, origin='lower', vmin=0.82, vmax=1.1, interpolation='nearest')
      else:
        plt.imshow(D, origin='lower', vmin=min_val, vmax=max_val,
                  interpolation='none')

      plt.colorbar()
      plt.title(param + ' ' + s)

      plt.contour(D, colors='black')
      plt.savefig(data_folder + param + '/pic/' + param + s + '.png', dpi=100)
      plt.close()
      print("%i of %i" % (i, max_files))
      i += each
