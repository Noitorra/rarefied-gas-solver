import os
import csv
import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt


def plot_flows(n, times, flows, avg_flows, in_flows, save_file):
    font = {
        'family': 'Times New Roman',
        'size': 16
    }
    mpl.rc('font', **font)

    plt.figure(figsize=(12, 6))

    plt.subplot(1, 2, 1)
    plt.ticklabel_format(style='sci', scilimits=(0, 3), axis='both', useMathText=True)
    plt.gca().set_xlabel('$t, с$')
    plt.gca().set_ylabel('$Q_{out}, с^{-1}м^{-2}$')
    for i in range(n):
        plt.plot(times[i], flows[i], '-', color='black')
        plt.text(times[i][-1], flows[i][-1], ' {}'.format(i + 1), verticalalignment='center')

    z = np.polyfit(in_flows, avg_flows, 1)
    f = np.poly1d(z)

    plt.subplot(1, 2, 2)
    plt.ticklabel_format(style='sci', scilimits=(0, 3), axis='both', useMathText=True)
    plt.plot(in_flows, avg_flows, 'o', color='black')
    plt.plot(in_flows, f(in_flows), '--', color='black', label='{}'.format(f).replace('\n', ''))
    plt.gca().set_xlabel('$Q_{in}, с^{-1}м^{-2}$')
    plt.legend()

    plt.tight_layout()
    plt.savefig(save_file, dpi=144)
    plt.close()


def main():
    root = './data_KrXe'
    files = ['x0.25.csv', 'x0.5.csv', 'x1.csv', 'x2.csv', 'x4.csv']
    in_flows_Kr = [0.11e15, 0.22e15, 0.44e15, 0.88e15, 1.76e15]
    in_flows_Xe = [0.05e15, 0.1e15, 0.2e15, 0.4e15, 0.8e15]
    time_fix = 1.27176e-06
    iter_fix = 5000

    n = len(files)
    times = [[] for _ in range(n)]
    flows_Kr = [[] for _ in range(n)]
    flows_Xe = [[] for _ in range(n)]

    for i in range(n):
        fh = open(root + os.sep + files[i], 'r')
        reader = csv.DictReader(fh, delimiter=',')
        for row in reader:
            if int(row['Time']) > 100:
                continue
            times[i].append(int(row['Time']) * iter_fix * time_fix)
            flows_Kr[i].append(float(row['avg(Flow_1 (0))']))
            flows_Xe[i].append(float(row['avg(Flow_2 (0))']))
        fh.close()

    avg_flows_Kr = [np.average(flows_Kr[i][-20:]) for i in range(n)]
    avg_flows_Xe = [np.average(flows_Xe[i][-20:]) for i in range(n)]

    plot_flows(n, times, flows_Kr, avg_flows_Kr, in_flows_Kr, root + os.sep + 'Kr.png')
    plot_flows(n, times, flows_Xe, avg_flows_Xe, in_flows_Xe, root + os.sep + 'Xe.png')


if __name__ == '__main__':
    main()

