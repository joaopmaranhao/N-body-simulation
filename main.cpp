#include "application/simulation_app.hpp"
#include "physics/n_body_system_builder.hpp"

int main() {
    auto system = NBodySystemBuilder()
        .addBody({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 1.0e13f, 5.0f)
        .addBody({20.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 5.0f}, 1.0e8f, 1.2f)
        .addBody({-35.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -4.0f}, 2.0e8f, 1.8f)
        .build();

    return SimulationApp("N-Body 3D").run(system);
}
