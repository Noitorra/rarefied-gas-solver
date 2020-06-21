import os
import csv
import numpy as np
import plot_helper as ph


def main():
    root = './data_GPRT'
    files = ['x0.25.csv', 'x0.4.csv', 'x0.5.csv', 'x0.7.csv', 'x1.csv', 'x2.csv', 'x3.csv', 'x4.csv']
    in_flows_krypton = [4.63E+15, 7.40E+15, 9.25E+15, 1.30E+16, 1.85E+16, 3.70E+16, 5.55E+16, 7.40E+16]
    in_flows_xenon = [7.21E+14, 1.15E+15, 1.44E+15, 2.02E+15, 2.88E+15, 5.77E+15, 8.65E+15, 1.15E+16]
    time_fix = 6.24478e-07
    iter_fix = 10000

    n = len(files)
    times = [[] for _ in range(n)]
    flows_krypton = [[] for _ in range(n)]
    flows_xenon = [[] for _ in range(n)]

    for i in range(n):
        fh = open(root + os.sep + files[i], 'r')
        reader = csv.DictReader(fh, delimiter=',')
        for row in reader:
            if int(row['Time']) > 100:
                continue
            times[i].append(int(row['Time']) * iter_fix * time_fix)
            flows_krypton[i].append(float(row['avg(Flow_1 (2))']))
            flows_xenon[i].append(float(row['avg(Flow_2 (2))']))
        fh.close()

    avg_flows_krypton = [np.average(flows_krypton[i][-20:]) for i in range(n)]
    avg_flows_xenon = [np.average(flows_xenon[i][-20:]) for i in range(n)]

    ph.plot_flows(n, times, flows_krypton, avg_flows_krypton, in_flows_krypton, root + os.sep + 'Kr.png')
    ph.plot_flows(n, times, flows_xenon, avg_flows_xenon, in_flows_xenon, root + os.sep + 'Xe.png')


if __name__ == '__main__':
    main()
