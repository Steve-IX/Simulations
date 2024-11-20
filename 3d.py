from vpython import *
import random
import math

# Constants
g = 9.80665  # Gravity (m/s^2)
elasticity = 0.9  # Coefficient of restitution (bounciness factor)
num_balls = 5  # Number of balls

# Simulation parameters
dt = 0.01  # Time step (s)
rate_val = 100  # Simulation update rate (Hz)

# Set up the scene
scene = canvas(title='Bouncing Balls in a Box', width=800, height=800, center=vector(0, 0, 0))

floor = box(pos=vector(0, -1, 0), size=vector(10, 0.2, 10), color=color.white)
walls = [
    box(pos=vector(-5, 0, 0), size=vector(0.2, 10, 10), color=color.white),  # Left
    box(pos=vector(5, 0, 0), size=vector(0.2, 10, 10), color=color.white),   # Right
    box(pos=vector(0, 0, -5), size=vector(10, 10, 0.2), color=color.white),  # Back
    box(pos=vector(0, 0, 5), size=vector(10, 10, 0.2), color=color.white)    # Front
]

# Global variables to store balls and a running state
balls = []
running = False


# Function to create a random vector within a range
def random_vector(min_val, max_val):
    return vector(
        random.uniform(min_val, max_val),
        random.uniform(min_val, max_val),
        random.uniform(min_val, max_val)
    )


# Function to initialize the simulation
def initialize_simulation():
    global balls
    # Clear existing balls and trails (if any)
    for ball in balls:
        ball.clear_trail()  # Clear the ball's trail
        ball.visible = False  # Hide the ball
        del ball  # Remove the ball object entirely
    balls = []  # Reset the list

    # Create new balls with random initial positions, velocities, and colors
    for _ in range(num_balls):
        ball_color = vector(random.random(), random.random(), random.random())  # Generate random color
        ball = sphere(
            pos=vector(random.uniform(-4, 4), 4.5, random.uniform(-4, 4)),  # Start near the top
            radius=0.5,
            color=ball_color,
            make_trail=True,  # Enable the trail
            retain=50  # Fading trail with limited points
        )
        ball.velocity = random_vector(-5, 5)  # Random initial velocity
        balls.append(ball)


# Function to toggle the simulation
def run_simulation():
    global running
    if running:
        running = False  # Stop the simulation
    else:
        initialize_simulation()
        running = True


# Function to handle collisions between balls
def handle_ball_collisions():
    for i in range(len(balls)):
        for j in range(i + 1, len(balls)):
            ball1 = balls[i]
            ball2 = balls[j]

            # Compute the distance between the centers of the two balls
            distance = mag(ball1.pos - ball2.pos)
            if distance < (ball1.radius + ball2.radius):  # Collision detected
                # Compute the unit normal and tangential vectors
                normal = norm(ball1.pos - ball2.pos)
                tangent = vector(-normal.y, normal.x, 0)  # Perpendicular vector

                # Project velocities onto the normal and tangential vectors
                v1_normal = dot(ball1.velocity, normal)
                v2_normal = dot(ball2.velocity, normal)

                # Swap the normal components of the velocities
                ball1.velocity += (v2_normal - v1_normal) * normal
                ball2.velocity += (v1_normal - v2_normal) * normal

                # Ensure the balls separate to prevent overlap
                overlap = ball1.radius + ball2.radius - distance
                ball1.pos += normal * (overlap / 2)
                ball2.pos -= normal * (overlap / 2)


# Add a "RUN" button to start or stop the simulation
button(text="RUN", bind=lambda _: run_simulation())


# Simulation loop
while True:
    rate(rate_val)
    if running:  # Only run the simulation if the state is active
        for ball in balls:
            # Update position
            ball.pos += ball.velocity * dt

            # Check for collisions with walls and floor
            if ball.pos.x > walls[1].pos.x - ball.radius or ball.pos.x < walls[0].pos.x + ball.radius:
                ball.velocity.x = -ball.velocity.x * elasticity
            if ball.pos.y < floor.pos.y + ball.radius:
                ball.pos.y = floor.pos.y + ball.radius  # Reset position to prevent sinking
                ball.velocity.y = -ball.velocity.y * elasticity
            if ball.pos.z > walls[3].pos.z - ball.radius or ball.pos.z < walls[2].pos.z + ball.radius:
                ball.velocity.z = -ball.velocity.z * elasticity

            # Update velocity due to gravity
            ball.velocity.y += -g * dt

        # Handle collisions between balls
        handle_ball_collisions()
