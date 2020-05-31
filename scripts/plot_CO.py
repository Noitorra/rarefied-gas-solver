import os
import csv
import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt


def plot_pressure(n, times, pressure, avg_pressure, holes, save_file):
    font = {
        'family': 'Times New Roman',
        'size': 16
    }
    mpl.rc('font', **font)

    plt.figure(figsize=(12, 6))

    plt.subplot(1, 2, 1)
    plt.ticklabel_format(style='sci', scilimits=(0, 3), axis='both', useMathText=True)
    plt.gca().set_xlabel('$t, с$')
    plt.gca().set_ylabel('$P_{hole}, Па$')
    for i in range(n):
        plt.plot(times[i], pressure[i], '-', color='black')
        plt.text(times[i][-1], pressure[i][-1], ' {}'.format(i + 1), verticalalignment='center')

    z = np.polyfit(holes, avg_pressure, 1)
    f = np.poly1d(z)

    plt.subplot(1, 2, 2)
    plt.ticklabel_format(style='sci', scilimits=(0, 3), axis='both', useMathText=True)
    plt.plot(holes, avg_pressure, 'o', color='black')
    plt.plot(holes, f(holes), '--', color='black', label='{}'.format(f).replace('\n', ''))
    plt.gca().set_xlabel('$R_{hole}, мм$')
    plt.legend()

    plt.tight_layout()
    plt.savefig(save_file, dpi=144)
    plt.close()


def main():
    root = './data_CO'
    files = ['r1.4.csv', 'r1.7.csv', 'r2.0.csv', 'r2.3.csv', 'r2.6.csv']
    holes = [1.4, 1.7, 2.0, 2.3, 2.6]
    time_fixes = [1, 1, 1, 1, 1]
    iter_fix = 5000

    n = len(files)
    times = [[] for _ in range(n)]
    pressure_Cs = [[] for _ in range(n)]
    pressure_CO = [[] for _ in range(n)]

    for i in range(n):
        fh = open(root + os.sep + files[i], 'r')
        reader = csv.DictReader(fh, delimiter=',')
        for row in reader:
            times[i].append(int(row['Time']) * iter_fix * time_fixes[i])
            pressure_Cs[i].append(float(row['avg(Pressure_0)']))
            pressure_CO[i].append(float(row['avg(Pressure_1)']))
        fh.close()

    avg_pressure_Cs = [np.average(pressure_Cs[i][-10:]) for i in range(n)]
    avg_pressure_CO = [np.average(pressure_CO[i][-10:]) for i in range(n)]

    plot_pressure(n, times, pressure_Cs, avg_pressure_Cs, holes, root + os.sep + 'Cs.png')
    plot_pressure(n, times, pressure_CO, avg_pressure_CO, holes, root + os.sep + 'CO.png')


if __name__ == '__main__':
    main()

