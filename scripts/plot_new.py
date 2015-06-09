__author__ = 'Dmitry Sherbakov'

import matplotlib.pyplot as plt
import matplotlib.ticker
import numpy
from numpy import *
import config
import os
import shutil

"""
    file structure:
        out_dir + gas0 + conc + data
"""

def plot_plain(binpath, pngpath, title, value):
    input = numpy.fromfile(binpath, dtype=float)

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
            D[x][y] = nan
          else:
            max_val = max(max_val, v)
            min_val = min(min_val, v)

    has_min_max = math.isnan(min_val) == nan or math.isnan(max_val) == nan or \
        (max_val - min_val) < 1e-300

    s_inter = 'none'
    if has_min_max:
        print('warning: max = min')
        min_val -= 0.1
        max_val += 0.1
        s_inter = 'nearest'

    # create figure
    if not long_plot:
        plt.figure()
    else:
        plt.figure(figsize = (20, 8))
        
    # title
    plt.title(title)

    im = plt.imshow(D, origin='lower', vmin=min_val, vmax=max_val, interpolation=s_inter, aspect='auto')

    orientation = 'vertical'
    if NY > NX:
        orientation = 'horizontal'

    cb_formatter = matplotlib.ticker.ScalarFormatter(useOffset=False)
    cb_formatter.set_powerlimits((-4,4))
    cb = plt.colorbar(im, orientation=orientation)
    cb.formatter = cb_formatter
    cb.update_ticks()

    # label
    cb.set_label(value)

    if not has_min_max:
        plt.contour(D, colors='black')
    plt.savefig(pngpath, dpi=100)
    plt.close()
    return 0

def plot_flow(binpath, pngpath, title, value, gas):
    input = numpy.fromfile(binpath, dtype=float)

    # reading additional info
    NX = int(input[0])
    NY = int(input[1])

    input = input[2:]
    #for numb in range(0, len(input)):
    #    print(input[numb])

    # hack
    a = NX
    NX = NY
    NY = a

    U = input[0:len(input):2]
    V = input[1:len(input):2]

    U = U.reshape(NX, NY)
    V = V.reshape(NX, NY)

    # create figure
    if not long_plot:
        plt.figure()
    else:
        plt.figure(figsize = (20, 8))
        
    # title
    plt.title(title)

    if gas == 0:
        scale_v = 3e24
        sample_v = 1e23
        #scale_v = 1e24
        #sample_v = 1e23
        sample_t = r'$1 \times 10^{23} \frac{1}{m^2 s}$'
    else:
        #scale_v = 1e18
        #sample_v = 5e16
        scale_v = 1e16
        sample_v = 4e14
        sample_t = r'$4 \times 10^{14} \frac{1}{m^2 s}$'

    each_x = 1
    each_y = 1

    Q = plt.quiver(U[::each_y, ::each_x], V[::each_y, ::each_x], color='r', scale=scale_v, width=0.001)
    qk = plt.quiverkey(Q, 0.9, 0.92, sample_v, sample_t, labelpos='E', fontproperties={'weight': 'bold'})
    l,r,b,t = plt.axis()
    dx, dy = r-l, t-b
    plt.axis([l-0.05*dx, r+0.05*dx, b-0.05*dy, t+0.05*dy])

    plt.savefig(pngpath, dpi=100)
    plt.close()
    return 0

# main program

max_files = 500
each = 500
gas_num = 3
long_plot = True

params = ["conc", "temp", "pressure", "flow"]
cb_text = [r'n, m^-3', r'T, K', r'P, Pa', r'FLOW']
#params = ["flow"]

out_dir = config.read_cfg_path("config.txt")

# clear directories

for param in params:
  for gas in range(gas_num):
    data_folder = out_dir + 'gas' + '%i' % gas + '/' + param
    folder = data_folder + '/pic'

    for the_file in os.listdir(folder):
      file_path = os.path.join(folder, the_file)
      try:
          if os.path.isfile(file_path):
              os.unlink(file_path)
          elif os.path.isdir(file_path): shutil.rmtree(file_path)
      except Exception(e):
          print(e)

for index in range(0, len(params)):
    for gas in range(gas_num):
        data_folder = out_dir + 'gas' + '%i' % gas + '/'
        i = 0
        while i < max_files + 1:
            s = "%i" % i

            if params[index] == "flow":
                # FLOW
                plot_flow(data_folder + params[index] + '/data/'+s+'.bin',
                           data_folder + params[index] + '/pic/' + params[index] + s + '.png',
                           params[index] + ' ' + s, cb_text[index], gas)
            else:
                # CONC PRESSSURE TEMP
                plot_plain(data_folder + params[index] + '/data/'+s+'.bin',
                           data_folder + params[index] + '/pic/' + params[index] + s + '.png',
                           params[index] + ' ' + s, cb_text[index])

            print("%i of %i" % (i, max_files))
            i += each
