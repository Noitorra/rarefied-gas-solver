import math
import os
import csv
import numpy as np
import plot_helper as ph


def main():
    root_integral = './data_Termos1D_integral'
    root_nointegral = './data_Termos1D_nointegral'
    iterations = [i for i in range(0, 50)]
    files_integral = ['temp_integral_{}.csv'.format(i) for i in iterations]
    files_nointegral = ['temp_nointegral_{}.csv'.format(i) for i in iterations]
    iter_fix = 100
    temp1 = 300.0
    temp2 = 400.0
    length = 1.0

    n = len(files_integral)
    times = []
    errors_integral = []
    errors_nointegral = []

    for i in range(n):
        times.append(i * iter_fix)

        # read integral data
        points = []
        temps = []
        fh = open(root_integral + os.sep + files_integral[i], 'r')
        reader = csv.DictReader(fh, delimiter=',')
        for row in reader:
            points.append(float(row['Points:0']))
            temps.append(float(row['Temperature_0']))
        fh.close()

        m = len(temps)
        error = 0.0
        for j in range(m - 1):
            error += math.pow((temps[j] - (temp1 + (temp2 - temp1) * points[j + 1] / length)), 2) / (m - 1)
        error /= math.sqrt(error)
        errors_integral.append(error)

        # read nointegral data
        points = []
        temps = []
        fh = open(root_nointegral + os.sep + files_nointegral[i], 'r')
        reader = csv.DictReader(fh, delimiter=',')
        for row in reader:
            points.append(float(row['Points:0']))
            temps.append(float(row['Temperature_0']))
        fh.close()

        m = len(temps)
        error = 0.0
        for j in range(m - 1):
            error += math.pow((temps[j] - math.sqrt(temp1 * temp2)), 2) / (m - 1)
        error /= math.sqrt(error)
        errors_nointegral.append(error)

    ph.plot_temp_error_1d(times, errors_integral, errors_nointegral, root_integral + os.sep + 'errors.png')


if __name__ == '__main__':
    main()
