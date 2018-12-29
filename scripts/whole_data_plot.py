import os
import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt

root = 'C:/Dev/Projects/Kafedra/output'
filename = 'wholeData.csv'


x = []
PKr = []
PXe = []

PSr = []
PBa = []

fields = []
is_first_line = True
file = open(root + os.sep + filename, 'r')
for row in file:
    data = row.strip().split(',')

    if is_first_line:
        is_first_line = False
        fields = data.copy()
        for i in range(len(fields)):
            fields[i] = fields[i].strip('\"')
    else:
        x.append(float(data[fields.index('Points:0')]))
        PKr.append(float(data[fields.index('Pressure_1')]))
        PXe.append(float(data[fields.index('Pressure_2')]))
        PSr.append(float(data[fields.index('Pressure_4')]))
        PBa.append(float(data[fields.index('Pressure_6')]))

font = {
    'family': 'monospace',
    'size': 16
}
mpl.rc('font', **font)

plt.figure(figsize=(12, 5))


plt.subplot(1, 2, 1)
plt.ticklabel_format(style='sci',scilimits=(0,0),axis='y')
plt.plot(x, PKr, ls='-', c='black', label='Kr')
plt.plot(x, PXe, ls='--', c='black', label='Xe')
plt.legend()

plt.subplot(1, 2, 2)
plt.ticklabel_format(style='sci',scilimits=(0,0),axis='y')
plt.plot(x, PSr, ls='-', c='black', label='Sr')
plt.plot(x, PBa, ls='--', c='black', label='Ba')
plt.legend()

plt.subplots_adjust(left=0.05, right=0.95, top=0.9, bottom=0.1)
plt.show()
# plt.savefig(root + os.sep + 'wholeData.png', dpi=144)
# plt.close()
