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

    # sets all 0.0 elements to nan
    input[input == 0.0] = nan
    # searching for max and min with numpy builtin functions
    max_val = numpy.amax(input)
    min_val = numpy.amin(input)

    #if abs(max_val) > 1e10 or abs(min_val) > 1e10:
    #    input = input / 1e10

    D = input.reshape(NX, NY)

    has_min_max = math.isnan(min_val) == nan or math.isnan(max_val) == nan or \
        (max_val - min_val) < 1e-300

    """
    print('min_val = ', min_val)
    print('max_val = ', max_val)
    print('max_val - min_val = ', max_val - min_val)
    """

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

    im = plt.imshow(D, origin='lower', interpolation=s_inter)

    orientation = 'vertical'
    if NY > NX:
        orientation = 'horizontal'

    if not has_min_max:
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

    # get max and min vector
    UV = numpy.vstack((U, V))
    UV_max = numpy.amax(UV, axis=1)
    UV_min = numpy.amin(UV, axis=1)
    # get sqrt(x^x + y^y)
    UV_max = numpy.square(UV_max)
    UV_max = numpy.sum(UV_max)
    UV_max = numpy.sqrt(UV_max)

    UV_min = numpy.square(UV_min)
    UV_min = numpy.sum(UV_min)
    UV_min = numpy.sqrt(UV_min)

    UV_average = (UV_max + UV_min) / 2

    U = U.reshape(NX, NY)
    V = V.reshape(NX, NY)

    # create figure
    plt.figure()
    # title
    plt.title(title)

    Q = plt.quiver(U, V, color='r', pivot='mid', units='inches')
    plt.quiverkey(Q, 0.5, 0.92, UV_average, '{}'.format(UV_average), labelpos='E',
                  fontproperties={'weight': 'bold'})
    l,r,b,t = plt.axis()
    dx, dy = r-l, t-b
    plt.axis([l-0.05*dx, r+0.05*dx, b-0.05*dy, t+0.05*dy])

    plt.savefig(pngpath, dpi=100)
    plt.close()
    return 0

# main program

max_files = 20000
each = 1000
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
