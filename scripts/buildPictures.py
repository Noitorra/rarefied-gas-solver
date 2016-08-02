__author__ = 'dimaxx'

import matplotlib.pyplot as plt
import numpy
from numpy import *

NX = 10
NY = 10
max_files = 2
gas_num = 2

font = {'family' : 'serif',
        'weight' : 'normal',
        'size'   : 16,
        }

for gas in range(gas_num):
  data_folder = '/Users/prohor/Code/RarefiedGasSolver/out/gas' + '%i' % gas + '/'
  NX = 10
  NY = 10
  #.reshape(NX, NY)
  valueAStream = numpy.fromfile(data_folder+'average_stream.bin', dtype=float).reshape(NY, NX)
  print(valueAStream)

  print(valueAStream[:,0])
  time = numpy.arange(NY)
  print(time)
  #plt.plot(time, valueAStream[:,0])
  plt.plot(time, valueAStream[:,0], 'k')
  plt.xlabel('time dt/t', fontdict=font)
  plt.ylabel('Ql', fontdict=font, rotation='horizontal')
  plt.savefig(data_folder+'average_streamLeftUp.png', dpi=100)
  plt.close()
  plt.plot(time, valueAStream[:,1], 'k')
  plt.xlabel('time dt/t', fontdict=font)
  plt.ylabel('Qr', fontdict=font, rotation='horizontal')
  plt.savefig(data_folder+'average_streamRightUp.png', dpi=100)
  plt.close()
  if NX > 2:
    plt.plot(time, valueAStream[:,2], 'k')
    plt.xlabel('time dt/t', fontdict=font)
    plt.ylabel('Ql', fontdict=font, rotation='horizontal')
    plt.savefig(data_folder+'average_streamLeftDown.png', dpi=100)
    plt.close()
    plt.plot(time, valueAStream[:,3], 'k')
    plt.xlabel('time dt/t', fontdict=font)
    plt.ylabel('Qr', fontdict=font, rotation='horizontal')
    plt.savefig(data_folder+'average_streamRightDown.png', dpi=100)
    plt.close()
