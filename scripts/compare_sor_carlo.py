import pathlib
from os import path

import numpy as np
from matplotlib import pyplot as plt

if __name__ == '__main__':
    loc = pathlib.Path(__file__).parent.absolute()
    carlo_data = np.genfromtxt(path.join(loc, "../data/carlo.csv"), delimiter=",")
    carlo_grid_size = carlo_data[:, 0]
    carlo_time = carlo_data[:, 1]
    carlo_time = carlo_time / max(carlo_time)

    sor_data = np.genfromtxt(path.join(loc, "../data/sor_time.csv"), delimiter=",")
    sor_grid_size = sor_data[:, 0]
    sor_time = sor_data[:, 1]
    sor_time = sor_time / max(sor_time)

    plt.title("SOR and carlo simulation times compared")
    plt.plot(carlo_grid_size, carlo_time, label="Carlo")
    plt.plot(carlo_grid_size, sor_time, label="SOR")
    plt.grid()
    plt.xlabel("Grid size")
    plt.ylabel("Normalized simulation time")
    plt.savefig(path.join(loc, "../images/carlo_sor_compare.png"))
