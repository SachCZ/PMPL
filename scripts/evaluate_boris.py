import pathlib
from os import path
import numpy as np
from matplotlib import pyplot as plt

if __name__ == '__main__':
    loc = pathlib.Path(__file__).parent.absolute()

    data = np.genfromtxt(path.join(loc, "../data/boris_trajectories.csv"), delimiter=",")
    t = data[:, 0]
    particle_data = data[:, 1:7]

    plt.plot(particle_data[:, 0], particle_data[:, 1], ".", label="Boris")

    E = 1e8
    B = 1
    Q = 1.60217662e-19
    m = 9.10938356e-31
    v0x = 2e8
    v0y = 0

    wc = Q * B / m
    vd = E / B
    analytic_x = 1 / wc * (v0x * np.sin(wc * t) - (v0y + vd) * np.cos(wc * t)) + 1 / wc * (vd + v0y)
    analytic_y = 1 / wc*(v0x*np.cos(wc*t) + (v0y + vd)*np.sin(wc*t)) - vd*t - 1/wc*v0x
    plt.plot(analytic_x, analytic_y, label="Analytic")

    plt.title("Boris")
    plt.xlabel("$x$ [m]")
    plt.ylabel("$y$ [m]")
    plt.grid()
    plt.legend()
    plt.savefig(path.join(loc, "../images/boris.png"))
    plt.clf()

