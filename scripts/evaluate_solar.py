import pathlib
from os import path
import numpy as np
from matplotlib import pyplot as plt

if __name__ == '__main__':
    loc = pathlib.Path(__file__).parent.absolute()
    data = np.genfromtxt(path.join(loc, "../data/solar_energy.csv"), delimiter=",")

    time = data[:, 0]
    energy = data[:, 1]

    plt.plot(time, energy, "-")
    plt.xlabel('$t$ [s]')
    plt.ylabel('$\\Delta E$ [J]')

    plt.ticklabel_format(axis="x", style="sci", scilimits=(0,0))
    plt.ticklabel_format(axis="y", style="sci", scilimits=(0,0))
    plt.title("Solar system energy")
    plt.savefig(path.join(loc, '../images/solar_energy.png'))
    plt.clf()

    data = np.genfromtxt(path.join(loc, "../data/solar_trajectories.csv"), delimiter=",")
    t = data[:, 0]
    sun_data = data[:, 1:7]
    earth_data = data[:, 7:13]
    moon_data = data[:, 13:19]

    plt.title("Solar system simulation")
    plt.plot(sun_data[:, 0], sun_data[:, 1], label="Sun")
    plt.plot(earth_data[:, 0], earth_data[:, 1], label="Earth")
    plt.plot(moon_data[:, 0], moon_data[:, 1], label="Moon")
    plt.legend()
    plt.xlabel("$x$ [km]")
    plt.ylabel("$y$ [km]")
    plt.grid()
    plt.savefig(path.join(loc, "../images/solar.png"))
    plt.clf()

    plt.title("Solar system simulation zoom")
    plt.plot(sun_data[:, 0], sun_data[:, 1], label="Sun")
    plt.plot(earth_data[:, 0], earth_data[:, 1], label="Earth")
    plt.plot(moon_data[:, 0], moon_data[:, 1], label="Moon")
    plt.legend()
    plt.xlabel("$x$ [km]")
    plt.ylabel("$y$ [km]")
    plt.grid()
    plt.xlim([-1.53e8, -1.47e8])
    plt.ylim([-0.4e8, 0.4e8])
    plt.savefig(path.join(loc, "../images/solar_zoom.png"))
    plt.clf()