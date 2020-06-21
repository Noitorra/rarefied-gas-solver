import os
import csv
import numpy as np
import plot_helper as ph


def main():
    root = './data_MEZ'
    files = ['x0.25.csv', 'x0.5.csv', 'x1.csv', 'x2.csv', 'x4.csv']
    in_flows_krypton = [0.11e15, 0.22e15, 0.44e15, 0.88e15, 1.76e15]
    in_flows_xenon = [0.05e15, 0.1e15, 0.2e15, 0.4e15, 0.8e15]
    time_fix = 1.27176e-06
    iter_fix = 5000

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
            flows_krypton[i].append(float(row['avg(Flow_1 (0))']))
            flows_xenon[i].append(float(row['avg(Flow_2 (0))']))
        fh.close()

    avg_flows_krypton = [np.average(flows_krypton[i][-20:]) for i in range(n)]
    avg_flows_xenon = [np.average(flows_xenon[i][-20:]) for i in range(n)]

    ph.plot_flows(n, times, flows_krypton, avg_flows_krypton, in_flows_krypton, root + os.sep + 'Kr.png')
    ph.plot_flows(n, times, flows_xenon, avg_flows_xenon, in_flows_xenon, root + os.sep + 'Xe.png')


if __name__ == '__main__':
    main()

