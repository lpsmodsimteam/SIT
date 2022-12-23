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

MAX = 4294967295

for num_iters in (10, 100, 1000, 10000, 100000, 1000000, 10000000):

    circle, square = 0, 0

    mt = mt19937()
    mt.seed(0)

    estimates = []
    x_coords = []
    y_coords = []
    radius = 0.5

    for i in range(num_iters):
        x_coords.append(mt.int32b() / MAX)
        y_coords.append(mt.int32b() / MAX)
        if (
            x_coords[-1] * x_coords[-1] + y_coords[-1] * y_coords[-1]
            < radius * 2
        ):
            circle += 1
        else:
            square += 1

        estimates.append((circle / (circle + square)) * 4)

    print(estimates[1], estimates[-1])

    if num_iters != 10000000:

        theta = np.linspace(0, 2 * np.pi, 300)
        a = radius + radius * np.cos(theta)
        b = radius + radius * np.sin(theta)

        plt.figure(figsize=(9, 9))
        plt.plot(a, b, "r")
        plt.scatter(x_coords, y_coords, edgecolor="black", linewidth=2.5)

        plt.title(f"N: {num_iters} / π: {estimates[-1]}")
        plt.xlabel("X")
        plt.ylabel("Y")

        plt.savefig(f"statics/{num_iters}.png")

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
