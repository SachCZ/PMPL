import pathlib
from os import path
import numpy as np
from matplotlib import pyplot as plt

if __name__ == '__main__':
    loc = pathlib.Path(__file__).parent.absolute()

    data = np.genfromtxt(path.join(loc, "../data/boris_relativistic_trajectories.csv"), delimiter=",")
    t = data[:, 0]
    particle_data = data[:, 1:7]

    plt.plot(particle_data[:, 0], particle_data[:, 1], ".", label="Boris relativistic")

    E = 1e8
    B = 1
    Q = 1.60217662e-19
    m = 9.10938356e-31
    v0x = 0
    v0y = 1e8

    wc = Q * B / m
    vd = E / B
    analytic_x = 1 / wc * (v0x * np.sin(wc * t) - (v0y + vd) * np.cos(wc * t)) + 1 / wc * (vd + v0y)
    analytic_y = 1 / wc*(v0x*np.cos(wc*t) + (v0y + vd)*np.sin(wc*t)) - vd*t - 1/wc*v0x
    plt.plot(analytic_x, analytic_y, label="Analytic non relativistic")

    plt.title("Relativistic Boris simulation")
    plt.xlabel("$x$ [m]")
    plt.ylabel("$y$ [m]")
    plt.grid()
    plt.legend()
    plt.savefig(path.join(loc, "../images/boris_relativistic.png"))
    plt.clf()

    speed = np.sqrt(np.power(particle_data[:, 3], 2) + np.power(particle_data[:, 4], 2) + np.power(particle_data[:, 5], 2))

    plt.title("Relativistic Boris particle speed")
    plt.xlabel("$t$ [s]")
    plt.ylabel("$|v|$ [m$\\cdot$s$^{-1}$]")
    plt.plot(t, speed)
    plt.savefig(path.join(loc, "../images/boris_relativistic_speed.png"))
    plt.clf()

