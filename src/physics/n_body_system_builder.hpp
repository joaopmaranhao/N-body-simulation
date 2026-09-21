#ifndef N_BODY_SYSTEM_BUILDER_HPP
#define N_BODY_SYSTEM_BUILDER_HPP

#include "n_body_system.hpp"

#include <initializer_list>

class NBodySystemBuilder {
public:
    NBodySystemBuilder& addBody(
        const Vec3& position,
        const Vec3& velocity,
        float mass,
        float radius
    );

    NBodySystemBuilder& addBody(Body body);

    template <typename Iterable>
    NBodySystemBuilder& addBodies(const Iterable& bodies) {
        for (const auto& body : bodies) {
            system.addBody(body);
        }
        return *this;
    }

    NBodySystemBuilder& addBodies(std::initializer_list<Body> bodies) {
        for (const auto& body : bodies) {
            system.addBody(body);
        }   
        return *this;
    }

    NBodySystem build();

private:
    NBodySystem system;
};

#endif // N_BODY_SYSTEM_BUILDER_HPP
