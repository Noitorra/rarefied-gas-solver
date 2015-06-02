__author__ = 'Kisame'

import numpy as np
import matplotlib.pyplot as plt

import config

params = ["conc", "temp", "pressure", "flow"]

out_dir = config.read_cfg_path("config.txt")

"""
    some concepts of this script:
    1) need average data in area
    2) this must be related to iterations
    3) arguments we need: param, area, iteration_start, iteration_end, iteration_step, ....
"""

def plot_average(gas, param, area_start, area_end, iter_start, iter_end, iter_step):
    # construct file names
    dir_name = '{0}gas{1}/{2}/data'.format(out_dir, gas, param)
    # variable for average values
    iter_num = np.ceil((iter_end - iter_start) / iter_step)
    iter_num = int(iter_num)
    X = []
    Y = []
    for i in range(iter_start, iter_end + 1, iter_step):
        file_name = '{0}/{1}.bin'.format(dir_name, i)
        # open file and collect data
        file_data = np.fromfile(file_name, dtype=float)
        # get NX NY
        NX = int(file_data[0])
        NY = int(file_data[1])
        file_data = file_data[2:].reshape(NY, NX)
        # get area part of data
        aver_data = file_data[area_start[1] * NY: area_end[1] * NY, area_start[0] * NX: area_end[0] * NX]
        # check if all elements is zeros
        all_zero = np.all(aver_data == 0.0)
        # make 0.0 elements equal to nan, so we could avoid to sum them
        if not all_zero:
            aver_data[aver_data == 0.0] = np.nan
        # find average value
        aver_val = np.nanmean(aver_data)
        # add value to massive
        X.append(i)
        Y.append(aver_val)

    plt.figure()
    plt.plot(X, Y)
    plt.show()

plot_average(0, 'pressure', [0.0, 0.0], [1.0, 1.0], 0, 8000, 1000)
