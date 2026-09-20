# N-Body Simulation

3D simulation of bodies under Newtonian gravity, written in C++17 and rendered with OpenGL, GLFW, GLAD, and GLM.

## Compilation

From the repository root:

```bash
cmake -S . -B build
cmake --build build --parallel 2
```

The executable is generated at `build/NBody3D`.

The project requires CMake, a C++17 compiler, OpenGL, and GLFW installed on the system. GLAD and GLM are included in `libs/`.

## Structure and API

- `Body::Body`: represents a body with position, velocity, mass, and radius.
- `BodyBuilder`: creates bodies through chained method calls.
- `NBodySystemBuilder`: collects bodies and constructs an `NBodySystem`.
- `NBodySystem`: calculates forces and advances the simulation.
- `SimulationApp`: encapsulates the GLFW window, renderer, callbacks, and main loop.

Example of creating a body:

```cpp
auto planet = BodyBuilder()
    .setPosition({20.0f, 0.0f, 0.0f})
    .setVelocity({0.0f, 0.0f, 5.0f})
    .setMass(1.0e8f)
    .setRadius(1.2f)
    .build();
```

Bodies can be added individually or from any iterable container, such as std::vector, std::list, std::array, or native arrays:

```cpp
auto system = NBodySystemBuilder()
    .addBody(planet)
    .addBodies(bodies)
    .build();

return SimulationApp("N-Body 3D").run(system);
```

## Physical Model

For each pair of bodies, the gravitational force is calculated as:

$$
\vec{F} = G \frac{m_1 m_2}{(\lVert\vec{r}\rVert + \varepsilon)^3}\vec{r}
$$

where G = 6.674e-11, r is the vector between the bodies, and ε = 0.1 is a softening term used to prevent singularities when bodies become very close to each other. The opposite force is applied to the other body in the pair.

The force calculation has a time complexity of $O(n^2)$ per simulation step, since all pairs of bodies are evaluated.

### Verlet Integration

NBodySystem::updateVerlet() uses a fixed timestep of 0.016f seconds. On the first step, the previous position is initialized using the initial velocity. Each body then updates its acceleration using a = F / m and its position according to:

$$
\vec{x}_{new} = \vec{x} + (\vec{x} - \vec{x}_{previous}) + \vec{a}\Delta t^2
$$

The simulation includes overlap detection based on the bodies' radii, but collision response is currently a placeholder; bodies detected as colliding do not undergo physical collision resolution.
