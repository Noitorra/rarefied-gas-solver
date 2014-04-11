__author__ = 'dimaxx'

import matplotlib.pyplot as plt
import numpy
from numpy import *

NX = 64 #// 46 18
NY = 128
max_files = 2
gas_num = 2

font = {'family' : 'serif',
        'weight' : 'normal',
        'size'   : 16,
        }

for gas in range(gas_num):
  data_folder = 'C:/Users/3D/Google Диск/out/gas' + '%i' % gas + '/'
  NX = 2
  NY = 1004
  #.reshape(NX, NY)
  valueAStream = numpy.fromfile(data_folder+'average_stream1003.bin', dtype=float).reshape(NY, NX)
  print(valueAStream)

  print(valueAStream[:,0])
  time = numpy.arange(NY)
  print(time)
  #plt.plot(time, valueAStream[:,0])
  plt.plot(time, valueAStream[:,0], 'k')
  plt.xlabel('time dt/t', fontdict=font)
  plt.ylabel('Ql', fontdict=font, rotation='horizontal')
  plt.savefig(data_folder+'average_stream1003Left.png', dpi=100)
  plt.close()
  plt.plot(time, valueAStream[:,1], 'k')
  plt.xlabel('time dt/t', fontdict=font)
  plt.ylabel('Qr', fontdict=font, rotation='horizontal')
  plt.savefig(data_folder+'average_stream1003Right.png', dpi=100)
  plt.close()
