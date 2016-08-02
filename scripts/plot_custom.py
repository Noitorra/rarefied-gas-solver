__author__ = 'Dmitry'


import numpy as np
from matplotlib import pyplot as plt
import matplotlib.ticker
import os, config, math

#X = [150, 300, 450]
X = [1, 5, 10]

#Y1 = [1.08e12, 1.43e14, 1.57e14]
#Y2 = [-1.03e12, 1.66e14, 1.85e14]

#Y1 = [1.87e-7, 1.4e-7, 9.5e-8]
#Y2 = [2.41e-7, 1.66e-7, 1.09e-7]

#Y1 = [4.77e-7, 3.02e-6, 6.03e-6]
#Y2 = [8.15e-7, 3.92e-6, 7.83e-6]

Y1 = [1.87e-7, 9.14e-7, 1.83e-6]
Y2 = [2.41e-7, 1.19e-6, 2.37e-6]

plt.figure()

y_formatter = matplotlib.ticker.ScalarFormatter(useOffset=False)
y_formatter.set_powerlimits((-4,4))
plt.axes().yaxis.set_major_formatter(y_formatter)

plt.plot(X, Y1, label='Kr')
plt.plot(X, Y2, label='Xe')

#plt.xlabel('P, Pa')
#plt.ylabel(r'q, $(m^2s)^-1$')

#plt.xlabel('P, Pa')
plt.xlabel('Nq')
plt.ylabel('P, Pa')

plt.legend(loc=2)

plt.show()