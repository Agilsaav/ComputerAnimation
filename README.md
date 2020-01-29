# Computer Animation
 
The Particles Simulation project, the String System Simulation project and the Animation Project have been developed using Visual Studio 2019 in Windows.

## Particles Simulation

Small physics engine which simulate the interaction of the particles with planes, triangles and spheres.
The computations can be made using three simulation solvers: 
#### Euler Solver (Key X)
#### Semi-Euler (Key C)
#### Verlet (Key V)
The solver can be changed in real time using the key indicated for each one. 
It also implements three initializations modes which can be chaned also in real time: 
#### Fountain (Key 1)
#### Waterfall (Key 2)
#### Explosion (Key 3)

## String System Simulation

Integration of String System Simulations in one dimension and two dimensions in the Particle Simulation engine.

## Fluid Simulation

Simulation of a fluid using a SPH(Smoothed-particle hydrodynamics). 

The code base is the same as the Animation project, so you will find similar classes and structure.

## Animation Project

This project uses the physics created in the projects done before and it adds some AI behavoir. It includes features as:
#### A* Pathfinding
#### Steering Behaviors (Dynamic Obstacle Avoidance)
#### Pyhisics Simulation (Simulation Solvers, Collisions, Forces)

The project include three maps that can be dynamically changed.
The previous work on Paricles Simulation and String Systems can be also found in here with a better structure.
