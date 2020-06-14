from os import path
import numpy as np
from matplotlib import pyplot as plt
import pathlib

if __name__ == '__main__':
    loc = pathlib.Path(__file__).parent.absolute()
    data = np.genfromtxt(path.join(loc, "../data/carlo.csv"), delimiter=",")
    grid_size = data[:, 0]
    time = data[:, 1]

    plt.plot(grid_size, time)
    plt.grid()
    plt.title("Monte carlo simulation scaling")
    plt.xlabel("Grid size")
    plt.ylabel("Simulation time")
    plt.savefig(path.join(loc, "../images/carlo.png"))
