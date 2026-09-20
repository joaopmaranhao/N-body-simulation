#ifndef BODY_BUILDER_HPP
#define BODY_BUILDER_HPP

#include "body.hpp"

class BodyBuilder {
public:
    BodyBuilder& setPosition(const Vec3& position);
    BodyBuilder& setVelocity(const Vec3& velocity);
    BodyBuilder& setMass(float mass);
    BodyBuilder& setRadius(float radius);
    Body::Body build();

private:
    Body::Body body;
};

#endif // BODY_BUILDER_HPP
