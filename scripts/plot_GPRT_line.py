import os
import csv
import numpy as np
import plot_helper as ph


def main():
    root = './data_GPRT'
    file = 'x1_line.csv'

    points = []
    pressures = [[] for _ in range(3)]
    temperatures = [[] for _ in range(3)]

    fh = open(root + os.sep + file, 'r')
    reader = csv.DictReader(fh, delimiter=',')
    for row in reader:
        if int(row['Time']) > 100:
            continue
        points.append(float(row['Points:2']))
        pressures[0].append(float(row['Pressure_0']))
        pressures[1].append(float(row['Pressure_1']))
        pressures[2].append(float(row['Pressure_2']))
        temperatures[0].append(float(row['Temperature_0']))
        temperatures[1].append(float(row['Temperature_1']))
        temperatures[2].append(float(row['Temperature_2']))
    fh.close()

    ph.plot_data_line(points, pressures, temperatures, root + os.sep + 'line_data.png')


if __name__ == '__main__':
    main()