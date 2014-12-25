__author__ = 'dimaxx'

import matplotlib.pyplot as plt
import numpy
from numpy import *
import config

max_files = 100
each = 5
gas_num = 1
params = ["conc", "temp", "pressure"]
#params = ["conc"]

out_dir = config.read_cfg_path("config.txt")

for param in params:
  for gas in range(gas_num):
    data_folder = out_dir + 'gas' + '%i' % gas + '/'
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

      has_min_max = math.isnan(min_val) == nan or math.isnan(max_val) == nan or \
                              (max_val - min_val) < 1e-6

      if has_min_max:
        print('warning: max = min')
        plt.imshow(D, origin='lower', vmin = max_val - 0.1, vmax= max_val + 0.1,
                   interpolation='nearest')
      else:
        plt.imshow(D, origin='lower', vmin=min_val, vmax=max_val,
                  interpolation='none')

      plt.colorbar()
      plt.title(param + ' ' + s)

      if not has_min_max:
        plt.contour(D, colors='black')
      plt.savefig(data_folder + param + '/pic/' + param + s + '.png', dpi=100)
      plt.close()
      print("%i of %i" % (i, max_files))
      i += each
