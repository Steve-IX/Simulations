# Simulations

A collection of small physics demonstrations created with the [VPython](https://vpython.org) module. These scripts were written during a first year university course and showcase basic techniques for simulating motion in three dimensions.

## Requirements

- Python 3.8 or newer
- `vpython` for the 3D graphics
- `numpy` for numerical calculations (only needed for `pendulum.py`)

Install the dependencies with:

```bash
pip install vpython numpy
```

## Included Simulations

### `3d.py` – Bouncing Balls in a Box

This script places several spheres inside a box and models their motion under gravity. Balls collide elastically with the walls and with each other. A "RUN" button toggles the simulation, and each sphere leaves a short trail so the motion is easier to follow.

Camera controls: hold the right mouse button and drag to rotate the view.

Run the simulation with:

```bash
python 3d.py
```

### `pendulum.py` – Simple Pendulum

Simulates a single pendulum swinging in a plane. The angle is updated using a simple Euler method and plotted against time in real time.

Run with:

```bash
python pendulum.py
```

## Purpose

These examples illustrate basic physics concepts and serve as a starting point for more involved simulations with VPython. Feel free to experiment with different parameters or extend the code for further study.
