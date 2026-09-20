#ifndef N_BODY_SYSTEM_BUILDER_HPP
#define N_BODY_SYSTEM_BUILDER_HPP

#include "n_body_system.hpp"

class NBodySystemBuilder {
public:
    NBodySystemBuilder& addBody(
        const Vec3& position,
        const Vec3& velocity,
        float mass,
        float radius
    );

    NBodySystem build();

private:
    NBodySystem system;
};

#endif // N_BODY_SYSTEM_BUILDER_HPP
