__author__ = 'dimaxx'

import matplotlib.pyplot as plt
import numpy
from numpy import *

max_files = 3
each = 1
gas_num = 2
params = ["conc", "temp", "pressure"]

for param in params:
  for gas in range(gas_num):
    data_folder = '../out/gas' + '%i' % gas + '/'
    i = 0
    while i < max_files:
      s = "%i" % i
      input = numpy.fromfile(data_folder + param + '/'+s+'.bin', dtype=float)

      # reading additional info
      NX = int(input[0])
      NY = int(input[1])

      input = input[2:]

      # hack
      a = NX
      NX = NY
      NY = a

      D = input.reshape(NX, NY)
      for x in range(NX):
        for y in range(NY):
          if D[x][y] == 0.0:
            D[x][y] = nan;
      plt.imshow(D, vmin=0.82, vmax=1.1, interpolation='nearest')
      plt.colorbar()
      plt.contour(D, colors='black')
      plt.savefig(data_folder+'conc/pic/'+s+'.png', dpi=100)
      plt.close()
      print("%i of %i" % (i, max_files))
      i += each
