#include "application/simulation_app.hpp"
#include "physics/body_builder.hpp"
#include "physics/n_body_system_builder.hpp"

#include <cmath>
#include <cstddef>
#include <vector>

int main() {
    constexpr std::size_t planetCount = 16;
    constexpr float gravitationalConstant = 6.674e-11f;
    constexpr float starMass = 1.0e13f;
    constexpr float pi = 3.14159265358979323846f;

    std::vector<Body::Body> bodies;
    bodies.reserve(planetCount + 1);

    bodies.push_back(
        BodyBuilder()
            .setPosition({0.0f, 0.0f, 0.0f})
            .setVelocity({0.0f, 0.0f, 0.0f})
            .setMass(starMass)
            .setRadius(5.0f)
            .build()
    );

    for (std::size_t i = 0; i < planetCount; ++i) {
        float fraction = static_cast<float>(i) / static_cast<float>(planetCount);
        float angle = 2.0f * pi * fraction;
        float orbitRadius = 25.0f + 4.0f * static_cast<float>(i);
        float orbitalSpeed = std::sqrt(gravitationalConstant * starMass / orbitRadius);

        auto body = BodyBuilder()
            .setPosition({
                orbitRadius * std::cos(angle),
                0.0f,
                orbitRadius * std::sin(angle)
            })
            .setVelocity({
                -orbitalSpeed * std::sin(angle),
                0.0f,
                orbitalSpeed * std::cos(angle)
            })
            .setMass(1.0e8f)
            .setRadius(1.2f)
            .build();

        bodies.push_back(body);
    }

    auto system = NBodySystemBuilder()
                .addBodies(bodies)
                .build();

    return SimulationApp("N-Body 3D").run(system);
}
