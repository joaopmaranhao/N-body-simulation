# N body simulation

A 3d graphic simulation of n-bodies floating in space and interacting gravitationally using OpenGl, C++ and CUDA. The simulation uses the newtonian mechanics and numerical methods(like RK4) to intergrate the movement of the bodies

---

## Directories
```
N-body-simulation/
|
|__build/
|__libs/
|__src/
    |__math/
    |__physics/
    |__renderer/
    |__benchmark/
|_main.cpp
|_CmakeLists.txt
```

## Model:

$$
F = G \frac{m_1 m_2}{|r|²}r
$$
