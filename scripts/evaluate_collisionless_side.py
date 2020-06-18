import pathlib
from os import path
import numpy as np
from matplotlib import pyplot as plt
from matplotlib import animation
import matplotlib

matplotlib.use("Agg")

if __name__ == '__main__':
    loc = pathlib.Path(__file__).parent.absolute()
    side_data = np.genfromtxt(path.join(loc, '../data/collisionless_side_speeds.csv'), delimiter=",")
    data = np.genfromtxt(path.join(loc, "../data/collisionless_trajectories.csv"), delimiter=",")

    j = -1
    v_x = data[j, 4::6]
    v_y = data[j, 5::6]
    v_z = data[j, 6::6]

    fig, axs = plt.subplots(2, 2, figsize=(11, 7))
    axs[0, 0].hist(v_x, density=True, bins=30, label="Volume $v_x$")
    axs[0, 0].hist(side_data[:, 0], density=True, bins=30, label="Side $v_x$")

    axs[0, 0].set_ylabel('f [-]')
    axs[0, 0].set_xlabel('$v_x$ [m$\\cdot$s$^{-1}$]')
    axs[0, 0].ticklabel_format(axis="x", style="sci", scilimits=(0, 0))
    axs[0, 0].ticklabel_format(axis="y", style="sci", scilimits=(0, 0))

    axs[0, 0].legend()

    axs[0, 1].hist(v_y, density=True, bins=30, label="Volume $v_y$")
    axs[0, 1].hist(side_data[:, 1], density=True, bins=30, label="Side $v_y$")

    axs[0, 1].set_ylabel('f [-]')
    axs[0, 1].set_xlabel('$v_y$ [m$\\cdot$s$^{-1}$]')
    axs[0, 1].ticklabel_format(axis="x", style="sci", scilimits=(0, 0))
    axs[0, 1].ticklabel_format(axis="y", style="sci", scilimits=(0, 0))

    axs[0, 1].legend()

    axs[1, 0].hist(v_z, density=True, bins=30, label="Volume $v_z$")
    axs[1, 0].hist(side_data[:, 2], density=True, bins=30, label="Side $v_z$")

    axs[1, 0].set_ylabel('f [-]')
    axs[1, 0].set_xlabel('$v_z$ [m$\\cdot$s$^{-1}$]')
    axs[1, 0].ticklabel_format(axis="x", style="sci", scilimits=(0, 0))
    axs[1, 0].ticklabel_format(axis="y", style="sci", scilimits=(0, 0))

    axs[1, 0].legend()

    speed = np.sqrt(np.power(v_x, 2) + np.power(v_y, 2) + np.power(v_z, 2))
    axs[1, 1].hist(speed, density=True, bins=30, label="Volume")

    speed = np.sqrt(np.power(side_data[:, 0], 2) + np.power(side_data[:, 1], 2) + np.power(side_data[:, 2], 2))
    axs[1, 1].hist(speed, density=True, bins=30, label="Side")

    T = 11600
    m = 9.10938356e-31
    k = 1.38064852e-23

    v = np.linspace(0, 17.5e5)
    boltzman_distribution = np.power(m / (2 * np.pi * k * T), 3 / 2) * 4 * np.pi * v * v * np.exp(
        -m * v * v / (2 * k * T))

    axs[1, 1].set_ylabel('f [-]')
    axs[1, 1].set_xlabel('$|v|$ [m$\\cdot$s$^{-1}$]')
    axs[1, 1].ticklabel_format(axis="x", style="sci", scilimits=(0, 0))
    axs[1, 1].ticklabel_format(axis="y", style="sci", scilimits=(0, 0))

    axs[1, 1].plot(v, boltzman_distribution, label="$f(T=11600$ K$)$")

    fig.suptitle("Comparison of volumetric and side distribution")
    axs[1, 1].legend()

    plt.tight_layout(rect=[0, 0.03, 1, 0.95])
    plt.savefig(path.join(loc, '../images/collisionless_distribution_compare.png'))
