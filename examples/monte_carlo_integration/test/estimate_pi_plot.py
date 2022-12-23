"""Square of 1 by 1 and circle of radius 0.5 inside it
area of this circle is pi*(0.5)^2 = pi/4
ratio of area of circle to area of square is (pi/4)/1 = pi/4

If use Monte Carlo simulation can est ratio of circle area to square area.
Can randomly place points inside square and they can fall inside or outside of circle
if run this n times and keep a count of whether within circle or just square:

pi / 4 approximately = count inside circle / total count
and therefore:

pi approximately = (count inside circle / total count) * 4
Code below does this, using np.linalg.norm to calc euclidean distance to circle centre
and therefore determine is point distance to centre less than or greater than circle radius"""

import matplotlib.pyplot as plt
import numpy as np

from mt19937 import mt19937

MAX_32 = 4294967295

RADIUS = 0.5

THETA = np.linspace(0, 2 * np.pi, 300)
A = RADIUS + RADIUS * np.cos(THETA)
B = RADIUS + RADIUS * np.sin(THETA)


def plot_estimates(num_iters, estimates):

    if num_iters < 1000000:

        plt.figure(figsize=(9, 9))
        plt.plot(A, B, "r")
        plt.scatter(x_coords, y_coords, edgecolor="black", linewidth=2.5)

        plt.title(f"N: {num_iters} / π: {estimates[-1]}")
        plt.xlabel("X")
        plt.ylabel("Y")

        plt.savefig(f"statics/{num_iters}.png")
        plt.close()

    plt.figure(figsize=(9, 9))
    plt.plot(list(range(num_iters)), estimates)
    plt.plot(
        list(range(num_iters)),
        [np.pi] * num_iters,
        color="r",
        linestyle="--",
    )

    plt.title(f"N: {num_iters} / π: {estimates[-1]}")
    plt.xlabel("Iterations")
    plt.ylabel("π")

    plt.savefig(f"statics/{num_iters}_.png")
    plt.close()


for num_iters in (10, 100, 1000, 10000, 100000, 1000000, 10000000):

    circle, square = 0, 0

    mt = mt19937()
    mt.seed(0)

    estimates = []
    x_coords = []
    y_coords = []

    for _ in range(num_iters):

        x_coords.append(mt.int32b() / MAX_32)
        y_coords.append(mt.int32b() / MAX_32)

        if x_coords[-1] ** 2 + y_coords[-1] ** 2 < RADIUS * 2:
            circle += 1
        else:
            square += 1

        estimates.append((circle / (circle + square)) * 4)

    print(
        f"{num_iters:10d}: {estimates[num_iters // 2]:.6f}, {estimates[-1]:.6f}"
    )

    plot_estimates(num_iters, estimates)
