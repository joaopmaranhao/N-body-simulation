#include "application/simulation_app.hpp"
#include "physics/body_builder.hpp"
#include "physics/n_body_system_builder.hpp"

int main() {

    Body::Body b1 = BodyBuilder()
                    .setPosition({0.0f, 0.0f, 0.0f})
                    .setVelocity({0.0f, 0.0f, 0.0f})
                    .setMass(1.0e13f)
                    .setRadius(5.0f)
                    .build();

    Body::Body b2 = BodyBuilder()
                    .setPosition({20.0f, 0.0f, 0.0f})
                    .setVelocity({0.0f, 0.0f, 5.0f})
                    .setMass(1.0e8f)
                    .setRadius(1.2f)
                    .build();


    Body::Body b3 = BodyBuilder()
                    .setPosition({-35.0f, 0.0f, 0.0f})
                    .setVelocity({0.0f, 0.0f, -4.0f})
                    .setMass(2.0e8f)
                    .setRadius(1.8f)
                    .build();

    auto system = NBodySystemBuilder()
                .addBodies({b1, b2, b3})
                .build();

    return SimulationApp("N-Body 3D").run(system);
}
