__author__ = 'dimaxx'

import matplotlib.pyplot as plt
import matplotlib.ticker
import numpy
from numpy import *
import config

max_files = 1000
each = 100
gas_num = 2

params = ["conc", "temp", "pressure"]
cb_text = [r'n, m^-3', r'T, K', r'P, Pa']
#params = ["conc"]

out_dir = config.read_cfg_path("config.txt")

for index in range(0, len(params)):
  for gas in range(gas_num):
    data_folder = out_dir + 'gas' + '%i' % gas + '/'
    i = 0
    while i < max_files + 1:
      s = "%i" % i
      input = numpy.fromfile(data_folder + params[index] + '/data/'+s+'.bin', dtype=float)

      # reading additional info
      NX = int(input[0])
      NY = int(input[1])

      input = input[2:]

      # hack
      a = NX
      NX = NY
      NY = a

      max_val = -float("inf")
      min_val = float("inf")
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
                              (max_val - min_val) < 1e-300



      
      s_inter = 'none'
      if has_min_max:
        print('warning: max = min')
        min_val = max_val - 0.1
        max_val = max_val + 0.1
        s_inter = 'nearest'
      
      #create figure
      plt.figure()
      plt.title(params[index] + ' ' + s)

      im = plt.imshow(D, origin='lower', vmin = min_val, vmax= max_val,
          interpolation=s_inter)

      orientation='vertical'
      if NY > NX:
        orientation='horizontal'
      else:
        orientation='vertical'
      
      cb_formatter = matplotlib.ticker.ScalarFormatter(useOffset=False)
      cb_formatter.set_powerlimits((-4,4))
      cb=plt.colorbar(im, orientation=orientation)
      cb.formatter = cb_formatter
      cb.update_ticks()
      
      cb.set_label(cb_text[index])
      

      if not has_min_max:
        plt.contour(D, colors='black')
      plt.savefig(data_folder + params[index] + '/pic/' + params[index] + s + '.png', dpi=100)
      plt.close()
      print("%i of %i" % (i, max_files))
      i += each
