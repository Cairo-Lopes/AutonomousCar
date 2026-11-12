# Autonomous Car Simulation

An educational C++ project that combines a simple autonomous-car simulation with experiments in basic machine learning models. The main goal is to learn C++ by building a small interactive application and to explore how a model such as a perceptron can use sensor readings to make steering decisions.

> **Work in progress:** This project is at an early stage. It is not complete, and features, code structure, and behavior may change as I learn. The car is currently controlled manually; the machine-learning code is experimental and is not yet training or driving the car.

## What’s in the project

- **Raylib driving simulation:** steer a car around a track and view its sensor readings.
- **Five distance sensors:** the car measures nearby track walls and displays the readings as telemetry.
- **Machine-learning experiments:** a perceptron and a basic neural-network layer, with configurable activation functions.

The command-line model test bench is organized into two groups:

- **Model examples:** demonstrates a single perceptron and a chained network (`5 → 5 → 4 → 2`), printing each layer's activations.
- **Five-sensor vehicle scenarios:** compares a simple rule-based action with the perceptron's steering output and a `Layer(5, 2)` steering/speed output. It includes left/right obstacles, a clear road, a front obstacle, and a left-wall approach sweep with a terminal steering plot.

The simulation rules are illustrative: turn away from the closer side; for the forward sensor, clearance below `0.4` means reduce speed, `0.4` through `0.8` means keep speed, and above `0.8` means increase speed. The model weights in the test bench are manually assigned examples, not trained weights, and these experiments do not control the graphical simulation.

## Controls

- **A / Left Arrow:** steer left
- **D / Right Arrow:** steer right
- **W / Up Arrow:** accelerate
- **S / Down Arrow:** slow down
- **R:** reset the car after a crash

## Requirements

- CMake 3.14 or newer
- A C++20 compiler
- An internet connection for the initial CMake configure, which downloads Raylib 4.5

Raylib may also require platform-specific system libraries. See the [Raylib build documentation](https://github.com/raysan5/raylib#build-and-installation) if configuration reports missing system dependencies.

## Build

Run these commands from the repository root:

```sh
cmake -S AutonomousCar -B build
cmake --build build
```

## Run

Start the graphical simulation:

```sh
./build/AutonomousCar
```

Run the perceptron and layer test bench:

```sh
./build/TestModels
```

## Project layout

```text
AutonomousCar/
├── CMakeLists.txt
├── ml/
│   ├── Layer.hpp
│   └── Perceptron.hpp
└── src/
    ├── Car.cpp
    ├── Car.hpp
    ├── Track.cpp
    ├── Track.hpp
    ├── main.cpp
    └── test_perceptron.cpp
```

## Files and folders

- **`AutonomousCar/CMakeLists.txt`** — CMake project configuration. It downloads Raylib and defines the graphical simulation and command-line test targets.
- **`AutonomousCar/src/main.cpp`** — Starts the Raylib window, creates the track and car, handles keyboard input, and draws the simulation.
- **`AutonomousCar/src/Car.hpp` / `Car.cpp`** — Declares and implements the car, movement, collision checks, five distance sensors, and telemetry.
- **`AutonomousCar/src/Track.hpp` / `Track.cpp`** — Declares and implements the track walls, default oval track, drawing, and line-intersection checks used by the car and sensors.
- **`AutonomousCar/src/test_perceptron.cpp`** — Command-line perceptron and layer examples, rule-based five-sensor vehicle scenarios, and a terminal steering-response plot.
- **`AutonomousCar/ml/Perceptron.hpp`** — A single perceptron that computes a weighted sum and applies a selected activation function.
- **`AutonomousCar/ml/Layer.hpp`** — A basic fully connected neural-network layer that calculates outputs from inputs, weights, and biases.
- **`build/`** — Generated CMake files and executables created by the build commands. This folder is local and is not committed.

This is a starting point for learning C++ and basic machine-learning concepts. Many parts are still exploratory, and the project is expected to grow over time.
