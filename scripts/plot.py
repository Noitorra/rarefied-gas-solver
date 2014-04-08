__author__ = 'dimaxx'

import matplotlib.pyplot as plt
import numpy
from numpy import *

NX = 10
NY = 10
max_files = 5
gas_num = 2

for gas in range(gas_num):
  data_folder = '../out/gas' + '%i' % gas + '/'
  for i in range(max_files):
    s = "%i" % i
    D = numpy.fromfile(data_folder+'den/'+s+'.bin', dtype=float).reshape(NX, NY)
    plt.imshow(D, vmin=0.993, vmax=1.007, interpolation='nearest')
    plt.colorbar()
    plt.contour(D, colors='black')
    plt.savefig(data_folder+'den/pic/'+s+'.png', dpi=100)
    plt.close()
    print("%i of %i" % (i, max_files))
  for i in range(max_files):
    s = "%i" % i
    D = numpy.fromfile(data_folder+'temp/'+s+'.bin', dtype=float).reshape(NX, NY)
    plt.imshow(D, interpolation='nearest')
    plt.colorbar()
    plt.contour(D, colors='black')
    plt.savefig(data_folder+'temp/pic/'+s+'.png', dpi=100)
    plt.close()
    print("%i of %i" % (i, max_files))