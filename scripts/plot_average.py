__author__ = 'Kisame'

import numpy as np
from matplotlib import pyplot as plt
import matplotlib.ticker
import os, config, math

"""
    some concepts of this script:
    1) need average data in area
    2) this must be related to iterations
    3) arguments we need: param, area, iteration_start, iteration_end, iteration_step, ....
"""

def calculate_average(data, area):
    # get area part of data
    aver_data = data[area[0]: area[1], area[2]: area[3]]
    # check if all elements is zeros
    all_zero = np.all(aver_data == 0.0)
    # make 0.0 elements equal to nan, so we could avoid to sum them
    if not all_zero:
        aver_data[aver_data == 0.0] = np.nan
    # find average value
    aver_val = np.nanmean(aver_data)
    return aver_val

def calc_arr_bounds(a):
    # find min and max
    min_a = np.nanmin(a)
    max_a = np.nanmax(a)
    avr_a = (max_a + min_a) / 2
    dlt_a = (max_a - min_a) / 2
    # coeff
    cef_a = 0.01
    if avr_a != 0.0:
        if dlt_a / avr_a < cef_a:
            dlt_a = avr_a * cef_a
            min_a = avr_a - dlt_a
            max_a = avr_a + dlt_a
    return min_a, max_a

def calc_lin_average(y):
    return np.nanmean(y)

def plot_average(out_dir, gas, param, cb_text, area_start, area_end, iter_start, iter_end, iter_step):
    # construct file names
    dir_name = '{0}gas{1}/{2}/data'.format(out_dir, gas, param)
    # special condition for streams
    param_flow = param == 'flow'
    # variable for average values
    X = []
    if not param_flow:
        Y = []
    else:
        U = []
        V = []
        M = []
    for i in range(iter_start, iter_end + 1, iter_step):
        file_name = '{0}/{1}.bin'.format(dir_name, i)
        # open file and collect data
        file_data = np.fromfile(file_name, dtype=float)
        # get NX NY
        NX = int(file_data[0])
        NY = int(file_data[1])
        file_data = file_data[2:]
        # deviation
        if not param_flow:
            file_data = file_data.reshape(NY, NX)
            aver_val = calculate_average(file_data, [area_start[1] * NY, area_end[1] * NY,
                                                     area_start[0] * NX, area_end[0] * NX])
            # add value to massive
            Y.append(aver_val)
        else:
            file_data_x = file_data[0:len(file_data):2].reshape(NY, NX)  # x part of stream
            file_data_y = file_data[1:len(file_data):2].reshape(NY, NX)  # y part of stream
            # calc average
            aver_val_x = calculate_average(file_data_x, [area_start[1] * NY, area_end[1] * NY,
                                                         area_start[0] * NX, area_end[0] * NX])
            aver_val_y = calculate_average(file_data_y, [area_start[1] * NY, area_end[1] * NY,
                                                         area_start[0] * NX, area_end[0] * NX])
            # combine to module
            aver_val_m = np.sqrt(np.square(aver_val_x) + np.square(aver_val_y))
            # put values into massive
            U.append(aver_val_x)
            V.append(aver_val_y)
            M.append(aver_val_m)
        # append iterations to massive
        X.append(i)

    plt.figure()
    plt.title('Gas: {0} Parameter: {1}'.format(gas, param))
    plt.ylabel(cb_text)

    y_formatter = matplotlib.ticker.ScalarFormatter(useOffset=False)
    y_formatter.set_powerlimits((-4,4))
    plt.axes().yaxis.set_major_formatter(y_formatter)

    if not param_flow:
        min_y, max_y = calc_arr_bounds(Y)
        plt.plot(X, Y, label=param)
        plt.axis([iter_start, iter_end, min_y, max_y])

        # average
        y_inx = int(4.0/5.0*len(Y))
        y_avr = calc_lin_average(Y[y_inx:len(Y)])
        plt.axhline(y=y_avr, xmin=0, xmax=1, hold=None)
        plt.text((iter_end - iter_start)*0.1, y_avr, '{:.2e}'.format(y_avr))
    else:
        plt.plot(X, U, '.', label='flow_x')
        #plt.plot(X, V, label='flow_y')
        #plt.plot(X, M, label='flow_m')

        # average
        y_inx = int(4.0/5.0*len(U))
        y_avr = calc_lin_average(U[y_inx:len(U)])
        plt.axhline(y=y_avr, xmin=0, xmax=1, hold=None)
        plt.text((iter_end - iter_start)*0.01, y_avr, '{:.2e}'.format(y_avr))
    #plt.legend()
    save_dir = '{0}globals/'.format(out_dir)
    if not os.path.exists(save_dir):
        os.mkdir(save_dir)
    save_file = '{0}{1}{2}.png'.format(save_dir, param, gas)
    # print(save_dir, save_file)
    plt.savefig(save_file, dpi=100)
    plt.close()

"""
    for streams we need 3 figure: X, Y, MODULE, maybe change output? .... yes i think
    so we will have: NX, NY, [x, y, z]
    program needs to read this shit? but we dont need Z stream, so fuck it
    so we need only X, Y and sqrt(X*X + Y*Y) maybe on same graph need to think twice
    same principe with zeroing naning! and other shit
"""
out_dirs = config.read_cfg_path("config.txt")

iter_start = 0
iter_end = 30000
iter_step = 1000
gas_num = 7

#params = ['conc', 'temp', 'pressure', 'flow']
#params = ['flow']
#cb_texts = [r'n, m^-3', r'T, K', r'P, Pa', r'$(m^2s)^-1$']
#cb_texts = [r'$(m^2s)^-1$']
#areas_start = [[0.0, 0.0], [0.0, 0.0], [0.0, 0.2], [0.8, 0.0]]
#areas_end   = [[1.0, 1.0], [1.0, 1.0], [0.5, 0.9], [1.0, 1.0]]

#areas_start = [[0.0, 0.0], [0.0, 0.0], [0.3, 0.2], [0.8, 0.0]]
#areas_end   = [[1.0, 1.0], [1.0, 1.0], [0.7, 0.8], [1.0, 1.0]]

# close
params = ['pressure', 'flow']
cb_texts = [r'P, Pa', r'$(m^2s)^-1$']

areas_start = [[0.3, 0.5], [0.0, 0.0]]
areas_end   = [[0.7, 1.0], [1.0, 1.0]]



out_num = len(out_dirs)
par_num = len(params)

for out_i in range(0, out_num):
    for gas_i in range(0, gas_num):
        for par_i in range(0, par_num):
            plot_average(out_dirs[out_i], gas_i, params[par_i], cb_texts[par_i], areas_start[par_i],
                         areas_end[par_i], iter_start, iter_end, iter_step)
            max_done = out_num * gas_num * par_num
            cur_done = (out_i * gas_num + gas_i) * par_num + par_i + 1
            percent = cur_done / max_done * 100
            print('Progress: [{}/{}][{}/{}][{}/{}][{:.2f}%]'
                  .format(out_i + 1, out_num, gas_i + 1, gas_num, par_i + 1, par_num, percent))
