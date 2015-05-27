__author__ = 'Dmitry Sherbakov'

import matplotlib.pyplot as plt
import matplotlib.ticker
import numpy
from numpy import *
import config

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
    plt.figure()
    # title
    plt.title(title)

    im = plt.imshow(D, origin='lower', vmin=min_val, vmax=max_val, interpolation=s_inter)

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

def plot_flow(binpath, pngpath, title, value):
    input = numpy.fromfile(binpath, dtype=float)

    # reading additional info
    NX = int(input[0])
    NY = int(input[1])

    input = input[2:]

    # hack
    a = NX
    NX = NY
    NY = a

    U = input[0:len(input):2]
    V = input[1:len(input):2]

    U = U.reshape(NX, NY)
    V = V.reshape(NX, NY)

    # create figure
    plt.figure()
    # title
    plt.title(title)

    Q = plt.quiver(U, V)
    #qk = plt.quiverkey(Q, 0.5, 0.92, 2, r'$2 \frac{m}{s}$', labelpos='W', fontproperties={'weight': 'bold'})
    #l,r,b,t = plt.axis()
    #dx, dy = r-l, t-b
    #plt.axis([l-0.05*dx, r+0.05*dx, b-0.05*dy, t+0.05*dy])

    plt.savefig(pngpath, dpi=100)
    plt.close()
    return 0

# main program

max_files = 2000
each = 100
gas_num = 1

params = ["conc", "temp", "pressure", "flow"]
cb_text = [r'n, m^-3', r'T, K', r'P, Pa', r'FLOW']
# params = ["conc"]

out_dir = config.read_cfg_path("config.txt")

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
                           params[index] + ' ' + s, cb_text[index])
            else:
                # CONC PRESSSURE TEMP
                plot_plain(data_folder + params[index] + '/data/'+s+'.bin',
                           data_folder + params[index] + '/pic/' + params[index] + s + '.png',
                           params[index] + ' ' + s, cb_text[index])

            print("%i of %i" % (i, max_files))
            i += each
