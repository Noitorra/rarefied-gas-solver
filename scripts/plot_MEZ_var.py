import os
import csv
import numpy as np
import plot_helper as ph


def main():
    root = './data_MEZ_var'
    files_up = ['up150.csv', 'up200.csv', 'up250.csv', 'up300.csv', 'up350.csv', 'up400.csv', 'up450.csv']
    files_right = ['right150.csv', 'right200.csv', 'right250.csv', 'right300.csv', 'right350.csv', 'right400.csv',
                   'right450.csv']
    pressures = [150, 200, 250, 300, 350, 400, 450]

    n = len(files_up)
    pressures_krypton = [[] for _ in range(n)]
    pressure_xenon = [[] for _ in range(n)]

    for i in range(n):
        fh = open(root + os.sep + files_up[i], 'r')
        reader = csv.DictReader(fh, delimiter=',')
        for row in reader:
            if int(row['Time']) > 100:
                continue
            pressures_krypton[i].append(float(row['avg(Pressure_1)']))
            pressure_xenon[i].append(float(row['avg(Pressure_2)']))
        fh.close()

    avg_pressures_krypton = [np.average(pressures_krypton[i][-20:]) for i in range(n)]
    avg_pressures_xenon = [np.average(pressure_xenon[i][-20:]) for i in range(n)]

    n = len(files_right)
    flows_krypton = [[] for _ in range(n)]
    flows_xenon = [[] for _ in range(n)]

    for i in range(n):
        fh = open(root + os.sep + files_up[i], 'r')
        reader = csv.DictReader(fh, delimiter=',')
        for row in reader:
            if int(row['Time']) > 100:
                continue
            flows_krypton[i].append(float(row['avg(Flow_1 (1))']))
            flows_xenon[i].append(float(row['avg(Flow_2 (1))']))
        fh.close()

    avg_flows_krypton = [np.average(flows_krypton[i][-20:]) for i in range(n)]
    avg_flows_xenon = [np.average(flows_xenon[i][-20:]) for i in range(n)]

    ph.plot_var(avg_pressures_krypton, avg_flows_krypton, pressures, root + os.sep + 'Kr.png')
    ph.plot_var(avg_pressures_xenon, avg_flows_xenon, pressures, root + os.sep + 'Xe.png')


if __name__ == '__main__':
    main()

