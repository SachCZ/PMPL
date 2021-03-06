import pathlib
from os import path
import numpy as np
from matplotlib import pyplot as plt
from matplotlib import animation
import matplotlib

matplotlib.use("Agg")

if __name__ == '__main__':
    loc = pathlib.Path(__file__).parent.absolute()
    data = np.genfromtxt(path.join(loc, "../data/hot_trajectories.csv"), delimiter=",")
    t = data[:, 0]

    fig = plt.figure()
    ax1 = plt.axes(xlim=(0, 1), ylim=(0, 1))

    line, = ax1.plot([], [], ".")

    plt.xlabel('$x$ [m]')
    plt.ylabel('$y$ [m]')
    title = ax1.text(.3, 1.05, '', transform=ax1.transAxes, va='center')


    def init():
        line.set_data([], [])
        title.set_text("Hot background gas, time 0 s")
        return line, title


    def animate(j):
        x = data[j, 1::(6 * 100)]
        y = data[j, 2::(6 * 100)]
        line.set_data(x, y)
        title.set_text("Hot background gas, time {} s".format(data[j, 0]))
        return line, title


    anim = animation.FuncAnimation(fig, animate, init_func=init, frames=len(data), interval=50, blit=True)

    Writer = animation.writers['ffmpeg']
    writer = Writer(fps=30, metadata=dict(artist='Me'), bitrate=1800)
    anim.save(path.join(loc, '../images/hot_trajectories.mp4'), writer=writer)
    plt.clf()

    j = -1
    v_x = data[j, 4::6]
    v_y = data[j, 5::6]
    v_z = data[j, 6::6]

    speed = np.sqrt(np.power(v_x, 2) + np.power(v_y, 2) + np.power(v_z, 2))

    plt.ylabel('f [-]')
    plt.xlabel('$|v|$ [m$\\cdot$s$^{-1}$]')
    plt.hist(speed, density=True, bins=30, label="Samples")

    T = 11600
    m = 9.10938356e-31
    k = 1.38064852e-23

    v = np.linspace(0, 17.5e5)
    boltzman_distribution = np.power(m / (2 * np.pi * k * T), 3 / 2) * 4 * np.pi * v * v * np.exp(
        -m * v * v / (2 * k * T))

    plt.plot(v, boltzman_distribution, label="$f(T=11600$ K$)$")
    plt.legend()
    plt.ticklabel_format(axis="x", style="sci", scilimits=(0,0))
    plt.ticklabel_format(axis="y", style="sci", scilimits=(0,0))
    plt.title("Hot gas speed distribution")
    plt.savefig(path.join(loc, '../images/hot_distribution.png'))
    plt.clf()

    data = np.genfromtxt(path.join(loc, "../data/hot_energy.csv"), delimiter=",")

    time = data[:, 0]
    energy = data[:, 1]

    plt.plot(time, energy, "-")
    plt.xlabel('$t$ [s]')
    plt.ylabel('$\\Delta E$ [J]')

    plt.ticklabel_format(axis="x", style="sci", scilimits=(0,0))
    plt.ticklabel_format(axis="y", style="sci", scilimits=(0,0))
    plt.title("Hot gas energy")
    plt.savefig(path.join(loc, '../images/hot_energy.png'))
    plt.clf()

