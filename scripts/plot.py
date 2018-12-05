import os
import numpy as np
import matplotlib.pyplot as plt

root = 'C:/Dev/Projects/Kafedra/output'

# list files
files = [f for f in os.listdir(root) if not os.path.isfile(f)]
for f in files:
    print("[{}] {}".format(files.index(f), f))

fi = int(input())

# do something
folder = root + os.sep + files[fi]
file = open(folder + os.sep + 'progression.txt', 'r')

gases_num = 3


def move_index(gi, i):
    return 1 + gi * gases_num + i


iterations = []
flows0 = []
flows1 = []
flows2 = []
# coefs = []

for row in file:
    data = row.strip().split(' ')
    iterations.append(int(data[0]))

    flows0.append(float(data[move_index(0, 2)]))
    flows1.append(float(data[move_index(1, 2)]))
    # flows2.append(float(data[move_index(2, 2)]))
# coefs.append(float(data[3]))

# average_coef = np.average(coefs[-100:])
# print('Average coef = {}'.format(average_coef))

average_flow0 = np.average(flows0[-100:])
average_flow1 = np.average(flows1[-100:])
# average_flow2 = np.average(flows2[-2000:])
print('Average flow (gas 0) = {}'.format(average_flow0))
print('Average flow (gas 1) = {}'.format(average_flow1))
# print('Average flow (gas 2) = {}'.format(average_flow2))

plt.subplot(211)
plt.plot(iterations, flows0)
plt.plot(iterations, [average_flow0 for i in range(len(iterations))])

plt.subplot(212)
plt.plot(iterations, flows1)
plt.plot(iterations, [average_flow1 for i in range(len(iterations))])

# plt.subplot(313)
# plt.plot(iterations, flows2)
# plt.plot(iterations, [average_flow2 for i in range(len(iterations))])

# plt.plot(iterations, [average_coef for i in range(len(iterations))])

plt.show()
