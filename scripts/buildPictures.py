__author__ = 'dimaxx'

import matplotlib.pyplot as plt
import numpy
from numpy import *

NX = 64 #// 46 18
NY = 128
max_files = 2
gas_num = 1

for gas in range(gas_num):
  data_folder = 'd:/Projects/C++/RarefiedGasSolver/out/gas' + '%i' % gas + '/'
  """
  for i in range(max_files):
    s = "%i" % i
    valueC = numpy.fromfile(data_folder+'den/'+s+'.bin', dtype=float).reshape(NX, NY)
    # print(valueC)
    plt.imshow(valueC, vmin=0.5, vmax=1.5, interpolation='nearest')
    plt.colorbar()
    plt.contour(valueC, colors='black')
    # plt.show()
    plt.savefig(data_folder+'den/pic/'+s+'.png', dpi=100)
    plt.close()
    print("%i of %i" % (i, max_files))
    """
"""
  for i in range(max_files):
    s = "%i" % i
    valueT = numpy.fromfile(data_folder+'temp/'+s+'.bin', dtype=float).reshape(NX, NY)
    for x in range(NX):
      for y in range(NY):
        if valueT[x][y] == 0.0:
          valueT[x][y] = nan;
    # print(valueT)
    plt.imshow(valueT, interpolation='nearest')
    plt.colorbar()
    #plt.contour(valueT, colors='black')
    plt.savefig(data_folder+'temp/pic/'+s+'.png', dpi=100)
    plt.close()
    print("%i of %i" % (i, max_files))
"""


NX = 2
NY = 10
#.reshape(NX, NY)
valueAStream = numpy.fromfile(data_folder+'average_stream1.bin', dtype=float)
valueAStream = valueAStream.reshape(NY, NX)
print(valueAStream)

print(valueAStream[:,0])

time = numpy.arange(NY)
print(time)
#plt.plot(time, valueAStream[:,0])
plt.plot(time, valueAStream[:,0], 'k')
plt.show()

plt.plot(time, valueAStream[:,1], 'k')
plt.show()
"""
plt.imshow(valueT, interpolation='nearest')
plt.colorbar()
#plt.contour(valueT, colors='black')
plt.savefig(data_folder+'temp/pic/'+s+'.png', dpi=100)
plt.close()
print("%i of %i" % (i, max_files))
"""