# Open Drone Platform
*A simulation-to-real (sim2real) learning platform for autonomous aerial systems*

### :hammer: This project is still under heavy development! :hammer:

This project is an open-source quadrotor platform designed to teach and explore the full robotics development pipeline:

Design &rarr; Simulation &rarr; Algorithm Development &rarr; Validation &rarr; Real-world Deployment

The goal is to make it possible for anyone, students, hobbyists, robotics teams, or educators, to **build and understand an autonomous drone system from first principles**.

The project combines:

- A **6-DOF rigid-body flight dynamics simulator**
- **Embedded flight firmware** running on a Raspberry Pi Pico
- **State estimation algorithms** (MEKF)
- **Hardware documentation and CAD files**

all in a fully **containerized development environment** to make the setup as painless as possible.

This enables a **sim2real workflow**, allowing algorithms to be developed and validated in simulation before running on real hardware.

---

# Motivation

The project originally began as a physical drone build to learn about:

- Flight Dynamics
- Sensor Fusion
- Embedded Systems
- Autonomous Robotics

To better reflect real-world robotics development, a **6-DOF simulation environment** was added. This enables algorithm development and testing in simulation before deploying to hardware.

The simulator is validated using the **NASA Engineering and Safety Center (NESC) 6-DOF simulation check cases**, ensuring the dynamics implementation is correct.
