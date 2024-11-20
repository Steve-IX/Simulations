from vpython import *
import numpy as np

# Constants
L = 1 # Length of pendulum
g = 9.8 # Acceleration due to gravity
dt = 0.01 # Time step

# Initial conditions
theta = 0.1 # Initial angle in radians
omega = 0 # Initial angular velocity
alpha = -g/L * np.sin(theta) # Initial angular acceleration

# Create the pendulum
bob = sphere(pos=vec(L*np.sin(theta), -L*np.cos(theta), 0), radius=0.1, color=color.red)
rod = cylinder(pos=vec(0,0,0), axis=bob.pos, radius=0.05)
pivot = cylinder(pos=vec(0,0,0), axis=vec(0,0,0.2), radius=0.1)

# Create the graph
graph = graph(width=400, height=300, title='Theta vs. Time', xtitle='Time (s)', ytitle='Theta (rad)')
theta_vs_time = gcurve(color=color.blue)

# Simulation
t = 0 # Initial time
while True: # Run the simulation for 10 seconds
    rate(100) # Set the animation rate to 100 updates per second
    theta = theta + omega*dt + 0.5*alpha*dt**2 # Update the angle using the Euler algorithm
    omega = omega + alpha*dt # Update the angular velocity
    alpha = -g/L * np.sin(theta) # Update the angular acceleration
    bob.pos = vec(L*np.sin(theta), -L*np.cos(theta), 0) # Update the position of the bob
    rod.axis = bob.pos # Update the position of the rod
    t = t + dt # Increment time
    if t % 0.1 < dt: # Plot every 0.1 seconds
        theta_vs_time.plot(pos=(t, theta)) # Plot theta vs. time
