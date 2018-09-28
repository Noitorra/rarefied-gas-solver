import numpy as np
import matplotlib.pyplot as plt

filename = 'C:/Dev/Projects/Kafedra/output/2018-09-25_11-39-42/progression.txt'
file = open(filename, 'r')

iterations = []
flows = []

for row in file:
    data = row.strip().split(' ')
    iteration = int(data[0])
    flow = float(data[2])
    iterations.append(iteration)
    flows.append(flow)


plt.plot(iterations, flows)
plt.show()