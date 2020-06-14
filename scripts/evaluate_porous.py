from os import path
import numpy as np
from matplotlib import pyplot as plt
import pathlib

if __name__ == '__main__':
    loc = pathlib.Path(__file__).parent.absolute()
    data = np.genfromtxt(path.join(loc, "../data/porous.csv"), delimiter=",")
    blocked_probability = data[:, 0]
    pass_probability = data[:, 1]

    plt.plot(blocked_probability, pass_probability)
    plt.grid()
    plt.title("Porous material")
    plt.xlabel("Infill probability")
    plt.ylabel("Pass through probability")
    plt.savefig(path.join(loc, "../images/porous.png"))
