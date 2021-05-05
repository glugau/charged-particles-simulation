# Charged particles simulation

This is a very simple charged particle simulation made using C++, OpenGL and GLFW.

To use this, simply launch the executable, and you will be able to interact with the simulation. At the top left of the window is a placeholder particle. The size indicates the mass, and the color indicates the charge (blue = negative, red = positive).

This idea is "inspired" (stolen) form this glorious YouTube video: https://www.youtube.com/watch?v=06pH9i9yARQ

I also strongly recommend watching The Cherno's OpenGL tutorial series. I made this program to exercise a bit before advancing further into the series.

## Input
You can interact with the simulation in many different ways:
- Up and down arrows control the mass of the next particle
- Left and right arrows control the charge
- To spawn a particle, you must click and drag with your cursor. The particle will spawn at the initial click position, and its velocity will be proportional to the distance between the initial click position and the release position.
- Do despawn a particle, you can right click on it. If the particle is too small/fast to click, you can use the delete key to despawn the youngest particle in the simulation.

## Note
If you want to experiment, you can edit values in constants.h. This file allows you to change the maximum/minimum charge/mass as well as a few other variables. This will require you to rebuild the project.
