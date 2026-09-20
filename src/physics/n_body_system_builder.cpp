#include "n_body_system_builder.hpp"

#include <utility>

NBodySystemBuilder& NBodySystemBuilder::addBody(
    const Vec3& position,
    const Vec3& velocity,
    float mass,
    float radius
) {
    system.addBody(Body::Body(position, velocity, mass, radius));
    return *this;
}

NBodySystem NBodySystemBuilder::build() {
    return std::move(system);
}
