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

from mt19937 import mt19937

MAX_32 = 4294967295

RADIUS = 0.5 * 2

for num_iters in (10, 100, 1000, 10000, 100000, 1000000, 10000000):

    mt = mt19937()
    mt.seed(0)

    x_coords = 0
    y_coords = 0

    circle = 0
    square = 0

    estimates = 0.0

    for _ in range(num_iters):

        x_coords = mt.int32b()
        y_coords = mt.int32b()
        x_coords = x_coords / MAX_32
        y_coords = y_coords / MAX_32
        dist = x_coords * x_coords + y_coords * y_coords

        if dist < RADIUS:
            circle += 1
        else:
            square += 1

        estimates = 4 * (circle / (circle + square))

    print(f"{num_iters:10d}: {estimates:.6f}")
