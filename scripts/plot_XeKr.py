import os
import csv
import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt
from collections import OrderedDict

root = 'c:/Dev/Projects/Kafedra/output_from_cluster/GPRT_CsKrXe'

font = {
    'family': 'Times New Roman',
    'size': 16
}
mpl.rc('font', **font)

plt.figure(figsize=(12, 6))

filenames = ['Flow_0.25.csv', 'Flow_0.5.csv', 'Flow_1.csv', 'Flow_2.csv', 'Flow_3.csv']

plt.subplot(1, 2, 1)
plt.ticklabel_format(style='sci', scilimits=(0,0), axis='x', useMathText=True)
plt.ticklabel_format(style='sci', scilimits=(0,0), axis='y', useMathText=True)
for i in range(len(filenames)):
    x = []
    yKr = []
    yXe = []

    file = open(root + os.sep + filenames[i], 'r')
    csvreader = csv.DictReader(file, delimiter=',')

    for row in csvreader:
        x.append(int(row['Time']) * 10000 * 1.39112e-7)
        yKr.append(float(row['avg(Flow_1 (2))']))
        yXe.append(float(row['avg(Flow_2 (2))']))
    file.close()

    plt.plot(x, yKr, '-', color='black', label='Kr')
    plt.plot(x, yXe, '--', color='black', label='Xe')
handles, labels = plt.gca().get_legend_handles_labels()
by_label = OrderedDict(zip(labels, handles))
plt.legend(by_label.values(), by_label.keys(), loc='upper left')

filenames = ['resultsKr.txt', 'resultsXe.txt']

plt.subplot(1, 2, 2)
plt.ticklabel_format(style='sci', scilimits=(0, 0), axis='x', useMathText=True)
plt.ticklabel_format(style='sci', scilimits=(0, 0), axis='y', useMathText=True)
for i in range(len(filenames)):
    x = []
    y = []

    file = open(root + os.sep + filenames[i], 'r')
    for row in file:
        data = row.strip().split(' ')
        x.append(float(data[0]))
        y.append(float(data[1]))
    file.close()

    z = np.polyfit(x, y, 1)
    f = np.poly1d(z)
    print("Dependency: {}".format(f))

    plt.plot(x, y, 'x', color='black')
    plt.plot(x, f(x), '-' if i == 0 else '--', color='black', label='Kr' if i == 0 else 'Xe')
plt.legend(loc='lower right')

plt.subplots_adjust(left=0.05, right=0.95, top=0.85, bottom=0.15)
plt.show()
# plt.savefig(root + os.sep + 'flows.png', dpi=144)
# plt.close()
