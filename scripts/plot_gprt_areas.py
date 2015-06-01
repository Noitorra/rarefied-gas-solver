__author__ = 'Dmitry Sherbakov'

import matplotlib.pyplot as plt
import matplotlib.ticker
import numpy
from numpy import *
import config
import os
import shutil

"""
    file structure:
        out_dir + gas0 + conc + data
"""

def load_file(binpath):
    input = numpy.fromfile(binpath, dtype=float)

    # reading additional info
    NX = int(input[0])
    NY = int(input[1])

    input = input[2:]
    #for numb in range(0, len(input)):
    #    print(input[numb])

    ## hack
    #a = NX
    #NX = NY
    #NY = a

    U = input[0:len(input):2]
    V = input[1:len(input):2]

    U = U.reshape(NX, NY)
    V = V.reshape(NX, NY)

    #print(NX, NY)

    return U, V, NX, NY

def find_range(p, size):
    if p < 0:
        size = -abs(size)
    # print ("p = " + str(p))
    # print ("size = " + str(size))

    end = p + size
    start = min(p, end)
    end = max(p, end)

    return range(start, end)

def add_point(U, V, area):
    area.points.append(0.0)

    size
    start_x = min(area.p[0], area.p[0] + area.size[0])

    n = 0
    for x in find_range(area.p[0], area.size[0]):
        for y in find_range(area.p[1], area.size[1]):
            #print(x, y)
            U[x][y] = 0.0
            V[x][y] = 0.0
            if area.axes == 'x':
                area.points[-1] += U[x][y]
                U[x][y] = 1e23
            else:
                area.points[-1] += V[x][y]
                V[x][y] = 1e23
            n += 1

    area.points[-1] /= n

def find_rounded_delta(list):
    v = max(list)
    # print("max = " + str(v))
    digit_q = int(math.log(v, 10))
    # print("digit_q = " + str(digit_q))
    ret = round(v, -digit_q + 1)
    # ret = pow(10, digit_q)
    # print("ret = " + str(ret))
    return ret / 10


def save_img(png_path, area, gas):
    plt.figure()

    gas_name = gases[gas]

    # title
    plt.title(area.name.title() + " " + gas_name)
    plt.xlabel('Iterations')
    plt.ylabel('Average Flow')

    time = []
    i = 0
    while i < max_files + 1:
        time.append(i)
        i += each


    #ax = plt.gca()
    #ax.set_xticks(numpy.arange(min(time), max(time), find_rounded_delta(time)))
    #ax.set_yticks(numpy.arange(min(area.points), max(area.points), find_rounded_delta(area.points)))
    #plt.grid()

    im = plt.plot(time, area.points, marker='+', color='b')

    # plt.show()
    plt.savefig(png_path, dpi=100)
    plt.close()

class Area:
    def __init__(self, name, p, size, axes):
        self.name = name
        self.p = p
        self.size = size
        self.axes = axes
    name = "noname"
    p = array([0, 0])
    size = array([0, 0])
    axes = 'no_axes'
    points = []

    def reset(self):
        self.points = []


# main program

max_files = 12000
each = 200
gas_num = 2
gases = ["Cs", "Xe"]

out_dir = config.read_cfg_path("config.txt")

areas = []
area_tmp = Area("out", array([-1, 0]), array([1, 150]), 'x')
areas.append(area_tmp)
area_tmp = Area("in", array([0, 0]), array([1, 150]), 'x')
areas.append(area_tmp)
area_tmp = Area("liquid top", array([6, -1]), array([1, 55]), 'x')
areas.append(area_tmp)
area_tmp = Area("liquid bottom", array([6, 150]), array([1, 50]), 'x')
areas.append(area_tmp)
area_tmp = Area("liquid out", array([-1, 110]), array([6, 1]), 'y')
areas.append(area_tmp)


for gas in range(gas_num):
    data_folder = out_dir + 'gas' + '%i' % gas + '/'
    save_folder = out_dir + 'gas' + '%i' % 0 + '/'  # save all pictures to one folder
    i = 0

    for area in areas:
        area.reset()

    while i < max_files + 1:
        s = "%i" % i

        U, V, NX, NY = load_file(data_folder + 'flow' + '/data/' + s + '.bin')

        for area in areas:
            add_point(U, V, area)


        #print("%i of %i" % (i, max_files))
        i += each

    for area in areas:
        save_img(save_folder + 'flow' + '/area_pic/' + area.name + " " + gases[gas] + '.png', area, gas)

    # plot flow to check areas

    # hack
    a = NX
    NX = NY
    NY = a

    U = U.reshape(NX, NY)
    V = V.reshape(NX, NY)
    
    if gas == 0:
        scale_v = 5e24
    else:
        scale_v = 1e18

    Q = plt.quiver(U, V, color='r', scale=scale_v, width=0.001)
    plt.show()
