import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt


def plot_flows(n, times, flows, avg_flows, in_flows, save_file):
    font = {
        'family': 'Times New Roman',
        'size': 16
    }
    mpl.rc('font', **font)

    plt.figure(figsize=(12, 6))

    plt.subplot(1, 2, 1)
    plt.ticklabel_format(style='sci', scilimits=(0, 3), axis='both', useMathText=True)
    plt.gca().set_xlabel('$t, с$')
    plt.gca().set_ylabel('$Q_{out}, с^{-1}м^{-2}$')
    for i in range(n):
        plt.plot(times[i], flows[i], '-', color='black')
        plt.text(times[i][-1], flows[i][-1], ' {}'.format(i + 1), verticalalignment='center')
    plt.title('а)')

    z = np.polyfit(in_flows, avg_flows, 1)
    f = np.poly1d(z)

    plt.subplot(1, 2, 2)
    plt.ticklabel_format(style='sci', scilimits=(0, 3), axis='both', useMathText=True)
    plt.plot(in_flows, avg_flows, 'o', color='black')
    plt.plot(in_flows, f(in_flows), '--', color='black', label='{}'.format(f).replace('\n', ''))
    plt.title('б)')
    plt.gca().set_xlabel('$Q_{in}, с^{-1}м^{-2}$')
    plt.legend()

    plt.tight_layout()
    plt.savefig(save_file, dpi=144)
    plt.close()


def plot_pressure(n, times, pressure, avg_pressure, holes, save_file):
    font = {
        'family': 'Times New Roman',
        'size': 16
    }
    mpl.rc('font', **font)

    plt.figure(figsize=(12, 6))

    plt.subplot(1, 2, 1)
    plt.ticklabel_format(style='sci', scilimits=(0, 3), axis='both', useMathText=True)
    plt.gca().set_xlabel('$t, с$')
    plt.gca().set_ylabel('$P_{hole}, Па$')
    for i in range(n):
        plt.plot(times[i], pressure[i], '-', color='black')
        plt.text(times[i][-1], pressure[i][-1], ' {}'.format(i + 1), verticalalignment='center')
    plt.title('а)')

    z = np.polyfit(holes, avg_pressure, 1)
    f = np.poly1d(z)

    plt.subplot(1, 2, 2)
    plt.ticklabel_format(style='sci', scilimits=(0, 3), axis='both', useMathText=True)
    plt.plot(holes, avg_pressure, 'o', color='black')
    plt.plot(holes, f(holes), '--', color='black', label='{}'.format(f).replace('\n', ''))
    plt.title('б)')
    plt.gca().set_xlabel('$R_{hole}, мм$')
    plt.legend()

    plt.tight_layout()
    plt.savefig(save_file, dpi=144)
    plt.close()


def plot_var(avg_pressures, avg_flows, pressures, save_file):
    font = {
        'family': 'Times New Roman',
        'size': 16
    }
    mpl.rc('font', **font)

    plt.figure(figsize=(12, 6))

    plt.subplot(1, 2, 1)
    plt.ticklabel_format(style='sci', scilimits=(0, 3), axis='both', useMathText=True)
    plt.plot(pressures, avg_pressures, 'o', color='black')
    plt.plot(pressures, avg_pressures, '--', color='black')
    plt.title('а)')
    plt.gca().set_xlabel('$P_{1}, Па$')
    plt.gca().set_ylabel('$P_{к}, Па$')

    plt.subplot(1, 2, 2)
    plt.ticklabel_format(style='sci', scilimits=(0, 3), axis='both', useMathText=True)
    plt.plot(pressures, avg_flows, 'o', color='black')
    plt.plot(pressures, avg_flows, '--', color='black')
    plt.title('б)')
    plt.gca().set_xlabel('$P_{1}, Па$')
    plt.gca().set_ylabel('$Q_{к}, с^{-1}м^{-2}$')

    plt.tight_layout()
    plt.savefig(save_file, dpi=144)
    plt.close()


def plot_data_line(points, pressures, temperatures, save_file):
    font = {
        'family': 'Times New Roman',
        'size': 16
    }
    mpl.rc('font', **font)

    plt.figure(figsize=(12, 15))

    plt.subplot(3, 2, 1)
    plt.ticklabel_format(style='sci', scilimits=(0, 3), axis='both', useMathText=True)
    plt.plot(points, pressures[0], '-', color='black')
    plt.title('а)')
    plt.gca().set_xlabel('$L, мм$')
    plt.gca().set_ylabel('$P_{Cs}, Па$')

    plt.subplot(3, 2, 2)
    plt.ticklabel_format(style='sci', scilimits=(0, 3), axis='both', useMathText=True)
    plt.plot(points, temperatures[0], '-', color='black')
    plt.title('б)')
    plt.gca().set_xlabel('$L, мм$')
    plt.gca().set_ylabel('$T_{Cs}, К$')

    plt.subplot(3, 2, 3)
    plt.ticklabel_format(style='sci', scilimits=(0, 3), axis='both', useMathText=True)
    plt.plot(points, pressures[1], '-', color='black')
    plt.title('в)')
    plt.gca().set_xlabel('$L, мм$')
    plt.gca().set_ylabel('$P_{Kr}, Па$')

    plt.subplot(3, 2, 4)
    plt.ticklabel_format(style='sci', scilimits=(0, 3), axis='both', useMathText=True)
    plt.plot(points, temperatures[1], '-', color='black')
    plt.title('г)')
    plt.gca().set_xlabel('$L, мм$')
    plt.gca().set_ylabel('$T_{Kr}, К$')

    plt.subplot(3, 2, 5)
    plt.ticklabel_format(style='sci', scilimits=(0, 3), axis='both', useMathText=True)
    plt.plot(points, pressures[2], '-', color='black')
    plt.title('д)')
    plt.gca().set_xlabel('$L, мм$')
    plt.gca().set_ylabel('$P_{Xe}, Па$')

    plt.subplot(3, 2, 6)
    plt.ticklabel_format(style='sci', scilimits=(0, 3), axis='both', useMathText=True)
    plt.plot(points, temperatures[2], '-', color='black')
    plt.title('е)')
    plt.gca().set_xlabel('$L, мм$')
    plt.gca().set_ylabel('$T_{Xe}, К$')

    plt.tight_layout()
    plt.savefig(save_file, dpi=144)
    plt.close()
