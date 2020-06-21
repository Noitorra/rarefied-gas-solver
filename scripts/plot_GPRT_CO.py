import os
import csv
import numpy as np
import plot_helper as ph


def main():
    root = './data_GPRT_CO'
    files = ['r1.4.csv', 'r1.7.csv', 'r2.0.csv', 'r2.3.csv', 'r2.6.csv']
    holes = [1.4, 1.7, 2.0, 2.3, 2.6]
    time_fixes = [1, 1, 1, 1, 1]
    iter_fix = 5000

    n = len(files)
    times = [[] for _ in range(n)]
    pressure_cesium = [[] for _ in range(n)]
    pressure_monoxide = [[] for _ in range(n)]

    for i in range(n):
        fh = open(root + os.sep + files[i], 'r')
        reader = csv.DictReader(fh, delimiter=',')
        for row in reader:
            if int(row['Time']) > 85:
                continue
            times[i].append(int(row['Time']) * iter_fix * time_fixes[i])
            pressure_cesium[i].append(float(row['avg(Pressure_0)']))
            pressure_monoxide[i].append(float(row['avg(Pressure_1)']))
        fh.close()

    avg_pressure_cesium = [np.average(pressure_cesium[i][-10:]) for i in range(n)]
    avg_pressure_monoxide = [np.average(pressure_monoxide[i][-10:]) for i in range(n)]

    ph.plot_pressure(n, times, pressure_cesium, avg_pressure_cesium, holes, root + os.sep + 'Cs.png')
    ph.plot_pressure(n, times, pressure_monoxide, avg_pressure_monoxide, holes, root + os.sep + 'CO.png')


if __name__ == '__main__':
    main()

