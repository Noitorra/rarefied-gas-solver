__author__ = 'Kisame'

import numpy as np
import matplotlib.pyplot as plt
import os, config, math

params = ["conc", "temp", "pressure", "flow"]

out_dir = config.read_cfg_path("config.txt")

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


def plot_average(gas, param, area_start, area_end, iter_start, iter_end, iter_step):
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
    if not param_flow:
        min_y, max_y = calc_arr_bounds(Y)
        plt.plot(X, Y, label=param)
        plt.axis([iter_start, iter_end, min_y, max_y])
    else:
        plt.plot(X, U, label='flow_x')
        plt.plot(X, V, label='flow_y')
        plt.plot(X, M, label='flow_m')
    plt.legend()
    save_dir = '{0}globals/'.format(out_dir)
    if not os.path.exists(save_dir):
        os.mkdir(save_dir)
    save_file = '{0}{1}{2}.png'.format(save_dir, param, gas)
    #print(save_dir, save_file)
    plt.savefig(save_file, dpi=100)
    plt.close()

"""
    for streams we need 3 figure: X, Y, MODULE, maybe change output? .... yes i think
    so we will have: NX, NY, [x, y, z]
    program needs to read this shit? but we dont need Z stream, so fuck it
    so we need only X, Y and sqrt(X*X + Y*Y) maybe on same graph need to think twice
    same principe with zeroing naning! and other shit
"""

iter_start = 0
iter_end = 2000
iter_step = 100

params = ['conc', 'temp', 'pressure', 'flow']
areas_start = [[0.0, 0.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0]]
areas_end   = [[1.0, 1.0], [1.0, 1.0], [1.0, 1.0], [1.0, 1.0]]
num_gases = 1

params_len = len(params)
for gas in range(0, num_gases):
    for param_index in range(0, params_len):
        plot_average(gas, params[param_index], areas_start[param_index],
                     areas_end[param_index], iter_start, iter_end, iter_step)
        percent = (gas * params_len + param_index + 1) / (num_gases * params_len) * 100
        print('Progress: [{0} / {1}][{2} / {3}][{4}%]'.format(gas + 1, num_gases, param_index + 1,
                                                              params_len, percent))
