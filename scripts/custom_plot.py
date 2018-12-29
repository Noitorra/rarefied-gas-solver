import os
import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt

root = 'C:/Dev/Projects/Kafedra/output'
filenames = ['resultsKr.txt', 'resultsXe.txt']
flows = [1.01e16, 1.73e+16]

font = {
    'family': 'monospace',
    'size': 16
}

mpl.rc('font', **font)

plt.figure(figsize=(12, 5))
for i in range(len(filenames)):
    file = open(root + os.sep + filenames[i], 'r')

    x = []
    y = []

    for row in file:
        data = row.strip().split(' ')
        x.append(float(data[0]))
        y.append(float(data[1]))

    z = np.polyfit(x, y, 1)
    f = np.poly1d(z)
    print("Func = {}".format(f))
    print("q = {}".format((f - flows[i]).roots))

    plt.subplot(1, len(filenames), 1 + i)
    plt.plot(x, y, '+')
    plt.plot(x, f(x), '-')

plt.subplots_adjust(left=0.05, right=0.95, top=0.9, bottom=0.1)
plt.show()
# plt.savefig(root + os.sep + 'flows.png', dpi=144)
# plt.close()
