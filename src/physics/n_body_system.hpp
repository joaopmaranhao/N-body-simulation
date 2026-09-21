#ifndef N_BODY_SYSTEM_HPP
#define N_BODY_SYSTEM_HPP

#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <cmath>

#include "body.hpp"

class NBodySystem {
private:
    std::vector<Body> bodies;
    float dt;

    void handleCollision(const Body& b1, const Body& b2);

public:
    NBodySystem();

    const std::vector<Body>& getBodies() const;
    const float getTimeStep() const;

    float distance(const Vec3& a, const Vec3& b);

    void addBody(const Body& b);
    void setBodyPosition(int index, const Vec3& pos);

    void handleCollisions();

    void updateVerlet();
    void updateRK4();
};

#endif // N_BODY_SYSTEM_HPP
