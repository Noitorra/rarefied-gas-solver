import os
import csv
import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt
from collections import OrderedDict

root = 'c:/Dev/Projects/Kafedra/output_from_cluster/GPRT_CsCO_Series'

x = []
yCs = []
yCO = []

holes = [26, 20, 14]
average_Cs = []
average_CO = []
filenames = ['Hole_26.csv', 'Hole_20.csv', 'Hole_14.csv']
for i in range(len(filenames)):
    xi = []
    yCsi = []
    yCOi = []
    average_Csi = []
    average_COi = []

    file = open(root + os.sep + filenames[i], 'r')
    csvreader = csv.DictReader(file, delimiter=',')

    for row in csvreader:
        if int(row['Time']) > 90:
            continue
        xi.append(int(row['Time']) * 10000 * 1.39112e-7)
        yCsi.append(float(row['avg(Pressure_0)']))
        yCOi.append(float(row['avg(Pressure_1)']))
        if int(row['Time']) > 60:
            average_Csi.append(yCsi[-1])
            average_COi.append(yCOi[-1])
        continue
    file.close()

    x.append(xi)
    yCs.append(yCsi)
    yCO.append(yCOi)
    average_Cs.append(np.average(average_Csi))
    average_CO.append(np.average(average_COi))

font = {
    'family': 'monospace',
    'size': 16
}
mpl.rc('font', **font)

plt.figure(figsize=(12, 10))

plt.subplot(2, 2, 1)
plt.ticklabel_format(style='sci', scilimits=(0, 3), axis='x', useMathText=True)
plt.ticklabel_format(style='sci', scilimits=(0, 3), axis='y', useMathText=True)
for i in range(len(x)):
    plt.plot(x[i], yCs[i], '-', color='black', label='Cs')
handles, labels = plt.gca().get_legend_handles_labels()
by_label = OrderedDict(zip(labels, handles))
plt.legend(by_label.values(), by_label.keys(), loc='lower right')

plt.subplot(2, 2, 2)
plt.ticklabel_format(style='sci', scilimits=(0, 3), axis='x', useMathText=True)
plt.ticklabel_format(style='sci', scilimits=(0, 3), axis='y', useMathText=True)
for i in range(len(x)):
    plt.plot(x[i], yCO[i], '-', color='black', label='CO')
handles, labels = plt.gca().get_legend_handles_labels()
by_label = OrderedDict(zip(labels, handles))
plt.legend(by_label.values(), by_label.keys(), loc='lower right')

plt.subplot(2, 2, 3)
plt.ticklabel_format(style='sci', scilimits=(0, 3), axis='x', useMathText=True)
plt.ticklabel_format(style='sci', scilimits=(0, 3), axis='y', useMathText=True)
plt.plot(holes, average_Cs, 'x', color='black', label='Cs')

plt.subplot(2, 2, 4)
plt.ticklabel_format(style='sci', scilimits=(0, 3), axis='x', useMathText=True)
plt.ticklabel_format(style='sci', scilimits=(0, 3), axis='y', useMathText=True)
plt.plot(holes, average_CO, 'x', color='black', label='CO')


# plt.subplots_adjust(left=0.05, right=0.95, top=0.9, bottom=0.2)
plt.show()
# plt.savefig(root + os.sep + 'wholeData.png', dpi=144)
# plt.close()
