import json
import pathlib
from os import path

import numpy as np
from matplotlib import pyplot as plt

if __name__ == '__main__':
    loc = pathlib.Path(__file__).parent.absolute()
    with open(path.join(loc, "../data/sor.json")) as json_file:
        data = json.load(json_file)
        X, Y = np.meshgrid(data["x"], data["y"])
        Z = data["phi"]

        plt.pcolor(X, Y, Z)
        plt.colorbar()

        plt.xlabel("x")
        plt.ylabel("y")
        plt.title("$e^{-2x}\\cos(2y)$ by SOR")
        plt.savefig(path.join(loc, "../images/sor.png"))
        plt.clf()

    data = np.genfromtxt(path.join(loc, "../data/sor_optimization.csv"), delimiter=",")
    if data:
        plt.plot(data[:, 0], data[:, 1])

        optimal_omega = 1
        min_steps = 100000000
        for steps, omega in zip(data[:, 1], data[:,0]):
            if steps < min_steps:
                min_steps = steps
                optimal_omega = omega

        print(optimal_omega)

        plt.xlabel("$\\omega$")
        plt.ylabel("steps")
        plt.grid()
        plt.title("SOR optimization")
        plt.savefig(path.join(loc, "../images/sor_optimization.png"))

    data = np.genfromtxt(path.join(loc, "../data/sor_time.csv"), delimiter=",")
    grid_size = data[:, 0]
    time = data[:, 1]

    plt.plot(grid_size, time)
    plt.grid()
    plt.title("SOR simulation scaling")
    plt.xlabel("Grid size")
    plt.ylabel("Simulation time")
    plt.savefig(path.join(loc, "../images/sor_time.png"))