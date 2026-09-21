#include "body_builder.hpp"

#include <utility>

BodyBuilder& BodyBuilder::setVelocity(const Vec3& velocity) {
    body.setVelocity(velocity);
    return *this;
}
BodyBuilder& BodyBuilder::setPosition(const Vec3& position) {
    body.setPosition(position);
    return *this;
}
BodyBuilder& BodyBuilder::setMass(float mass) {
    body.setMass(mass);
    return *this;
}
BodyBuilder& BodyBuilder::setRadius(float radius) {
    body.setRadius(radius);
    return *this;
}

Body BodyBuilder::build() {
    return std::move(body);
}
