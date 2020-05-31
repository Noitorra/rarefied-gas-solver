import os
import csv
import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt
import math

font = {
    'family': 'Times New Roman',
    'size': 16
}
mpl.rc('font', **font)

root = 'c:/Users/dmitr/Desktop/'
filename = 'slice_flow_x1.csv'
filename_edges = 'slice_edges.csv'

X = []
Y = []
U1 = []
V1 = []
U2 = []
V2 = []

EDGES_X = []
EDGES_Y = []

file = open(root + os.sep + filename, 'r')
csvreader = csv.DictReader(file, delimiter=',')
for row in csvreader:
    x = float(row['Center:0'])
    y = float(row['Center:1'])
    z = float(row['Center:2'])
    X.append(z)
    Y.append(math.sqrt(x * x + y * y))

    flow_1_x = float(row['Flow_1:0'])
    flow_1_y = float(row['Flow_1:1'])
    flow_1_z = float(row['Flow_1:2'])
    U1.append(flow_1_z)
    V1.append(math.sqrt(flow_1_x * flow_1_x + flow_1_y * flow_1_y))

    flow_2_x = float(row['Flow_2:0'])
    flow_2_y = float(row['Flow_2:1'])
    flow_2_z = float(row['Flow_2:2'])
    U2.append(flow_2_z)
    V2.append(math.sqrt(flow_2_x * flow_2_x + flow_2_y * flow_2_y))
file.close()

file = open(root + os.sep + filename_edges, 'r')
csvreader = csv.DictReader(file, delimiter=',')
for row in csvreader:
    x = float(row['Points:0'])
    y = float(row['Points:1'])
    z = float(row['Points:2'])

    EDGES_X.append(z)
    EDGES_Y.append(math.sqrt(x * x + y * y))
file.close()

# reverse U
U1 = [-u for u in U1]
U2 = [-u for u in U2]

X = np.array(X)
Y = np.array(Y)
U1 = np.array(U1)
V1 = np.array(V1)
U2 = np.array(U2)
V2 = np.array(V2)
EDGES_X = np.array(EDGES_X)
EDGES_Y = np.array(EDGES_Y)

# get average U1 V1
M1 = [math.sqrt(U1[i] * U1[i] + V1[i] * V1[i]) for i in range(len(U1))]
M1_a = np.average(M1)

# get average U2 V2
M2 = [math.sqrt(U2[i] * U2[i] + V2[i] * V2[i]) for i in range(len(U2))]
M2_a = np.average(M2)

plt.figure(figsize=(6, 6))

plt.subplot(211)
plt.plot(EDGES_X, EDGES_Y, color='black')
qq = plt.quiver(X, Y, U1, V1, angles='uv', pivot='middle', color='red')
plt.quiverkey(qq, 0.2, 0.9, M1_a, '{:.2e}'.format(M1_a) + r'$ \frac{1}{\mathrm{м^2с}}$', labelpos='S', coordinates='figure')
plt.gca().invert_xaxis()
plt.gca().get_xaxis().set_visible(False)

plt.subplot(212)
plt.plot(EDGES_X, EDGES_Y, color='black')
qq = plt.quiver(X, Y, U2, V2, angles='uv', pivot='middle', color='red')
plt.quiverkey(qq, 0.2, 0.43, M2_a, '{:.2e}'.format(M2_a) + r'$ \frac{1}{\mathrm{м^2с}}$', labelpos='S', coordinates='figure')
plt.gca().invert_xaxis()

plt.subplots_adjust(left=0.1, right=0.93, top=0.95, bottom=0.05, hspace=0.1)
plt.savefig(root + os.sep + filename + '.png', dpi=144)
plt.show()
