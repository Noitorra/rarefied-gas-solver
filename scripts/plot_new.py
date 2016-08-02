__author__ = 'Dmitry Sherbakov'

import matplotlib.pyplot as plt
import matplotlib.ticker
import numpy as np
from numpy import *
import config

"""
    file structure:
        out_dir + gas0 + conc + data
"""

def plot_plain(binpath, pngpath, title, value):
    data = np.fromfile(binpath, dtype=float)

    # reading additional info
    NX = int(data[0])
    NY = int(data[1])

    data = data[2:]

    # hack
    a = NX
    NX = NY
    NY = a

    # sets all 0.0 elements to nan
    data[data == 0.0] = nan
    # searching for max and min with numpy builtin functions
    max_val = np.nanmax(data)
    min_val = np.nanmin(data)
    avr_val = (max_val + min_val) / 2
    delta_val = (max_val - min_val) / 2
    #print('max = {0} min = {1} avr = {2} delta = {3}'.format(max_val, min_val, avr_val, delta_val))

    # compare delta with some const
    koeff_val = 0.1
    data_isnan = False
    if (avr_val == 0.0 or isnan(avr_val)) and (delta_val == 0.0 or isnan(delta_val)):
        delta_val = koeff_val
        avr_val = 0.0
        max_val = avr_val + delta_val
        min_val = avr_val - delta_val
        data_isnan = True
    else:
        if delta_val / avr_val < koeff_val:
            delta_val = koeff_val * avr_val
            max_val = avr_val + delta_val
            min_val = avr_val - delta_val

    #print('max = {0} min = {1} avr = {2} delta = {3}'.format(max_val, min_val, avr_val, delta_val))

    # if all data is nan, then it is actually 0.0
    if data_isnan:
        data[isnan(data)] = 0.0

    D = data.reshape(NX, NY)

    # create figure
    plt.figure()
    # title
    plt.title(title)

    im = plt.imshow(D, origin='lower', interpolation='none', vmin=min_val, vmax=max_val)

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

    if not data_isnan:
        CS = plt.contour(D, colors='black')
        plt.clabel(CS, inline=1, fontsize=10, fmt='%g')

    plt.savefig(pngpath, dpi=100)
    plt.close()
    return 0

def plot_flow(binpath, pngpath, title, value):
    data = np.fromfile(binpath, dtype=float)

    # reading additional info
    NX = int(data[0])
    NY = int(data[1])

    data = data[2:]

    # hack
    a = NX
    NX = NY
    NY = a

    U = data[0:len(data):2]
    V = data[1:len(data):2]

    # get max and min vector
    UV = np.vstack((U, V))
    UV_max = np.amax(UV, axis=1)
    UV_min = np.amin(UV, axis=1)
    # get sqrt(x^x + y^y)
    UV_max = np.square(UV_max)
    UV_max = np.sum(UV_max)
    UV_max = np.sqrt(UV_max)

    UV_min = np.square(UV_min)
    UV_min = np.sum(UV_min)
    UV_min = np.sqrt(UV_min)

    UV_average = (UV_max + UV_min) / 2

    U = U.reshape(NX, NY)
    V = V.reshape(NX, NY)

    # create figure
    plt.figure(figsize=(6 * NY / NX, 6))
    # title
    plt.title(title)

    Q = plt.quiver(U, V, color='r', pivot='mid', units='xy')
    plt.quiverkey(Q, 0.5, 0.95, UV_average, '{0:.3e} {1}'.format(UV_average, value), labelpos='W')
    l,r,b,t = plt.axis()
    l,r,b,t = 0.0, NY, 0.0, NX
    dx, dy = r-l, t-b
    plt.axis([l-0.05*dx, r+0.05*dx, b-0.05*dy, t+0.05*dy])

    plt.savefig(pngpath, dpi=150)
    plt.close()
    return 0

# main program

iter_start = 0
iter_end = 500
iter_step = 100
gas_num = 2

params = ["conc", "temp", "pressure", "flow"]
cb_text = [r'n, m^-3', r'T, K', r'P, Pa', r'$(m^2s)^-1$']

out_dirs = config.read_cfg_path("config.txt")

out_num = len(out_dirs)
par_num = len(params)
itr_num = (iter_end - iter_start)

for out_i in range(0, out_num):
    for par_i in range(0, par_num):
        for gas_i in range(gas_num):
            data_folder = out_dirs[out_i] + 'gas' + '%i' % gas_i + '/'
            for i in range(iter_start, iter_end + 1, iter_step):
                s = "%i" % i

                if params[par_i] == "flow":
                    # FLOW
                    plot_flow(data_folder + params[par_i] + '/data/'+s+'.bin',
                               data_folder + params[par_i] + '/pic/' + params[par_i] + s + '.png',
                               params[par_i] + ' ' + s, cb_text[par_i])
                else:
                    # CONC PRESSSURE TEMP
                    plot_plain(data_folder + params[par_i] + '/data/'+s+'.bin',
                               data_folder + params[par_i] + '/pic/' + params[par_i] + s + '.png',
                               params[par_i] + ' ' + s, cb_text[par_i])
                # we have here 4 values: out_i, par_i, gas_i, i
                max_done = out_num * par_num * gas_num * itr_num
                cur_done = ((out_i * par_num + par_i) * gas_num + gas_i) * itr_num + i
                percent = cur_done / max_done * 100
                print("Progress: [{} / {}][{} / {}][{} / {}][{} / {}][{:.2f}%]"
                      .format(out_i + 1, out_num, par_i + 1, par_num, gas_i + 1, gas_num, i, itr_num, percent))
