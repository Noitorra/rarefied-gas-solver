__author__ = 'dimaxx'

import matplotlib.pyplot as plt
import numpy
from numpy import *

max_files = 5
each = 1
gas_num = 2
params = ["conc", "temp", "pressure"]
#params = ["conc"]

"""
Here is ULTRA param finder in config.txt it searches for path="some/path/to/out/dir"
and sets out_dir variable to it.
"""

out_dir = '../out/'

with open("config.txt", "r") as cfg_file:
    str_list = cfg_file.readlines()
    for str_index in range(len(str_list)):
        str_line = str_list[str_index].replace('\n', '').split("=")
        if len(str_line) == 2:
            str_param = str_line[0]
            str_value = str_line[1].replace('"', '')
            if str_param == "path":
                out_dir = str_value
        else:
            print("Wrong config line:" + str_line)

"""
"""

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
