README: 

This projects demonstrates a robot(green triangle) that maneuvers through a series of obstacles(red polygons), avoiding any collisions, to reach the desired destination in the shortest path using Breadth First Search(BFS) graph algorithm.

REQUIREMENTS:

1. Gcc compiler and xlib interface installed.

STEPS TO RUN:

1. The program can be compiled with : g++ main.cpp helpers.cpp -L/usr/X11R6/lib -lX11

2. The output file is generated with : ./a.out inputfile <-(pass in the inputfile here)


OUTPUT: 

1. The yellow triangle with green outline shows the start position of the robot

2. The red polygons are the obstacles

3. The robot manuvers the obstacles and reaches the destination which is a yellow polygon with no green outline.