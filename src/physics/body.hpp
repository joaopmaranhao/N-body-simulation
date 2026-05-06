#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include <vector>
#include <cmath>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../math/vec3.hpp"

struct Particle{
    private:
        Vec3 position;
        Vec3 velocity;
        Vec3 acceleration;
        Vec3 forceAccumulator;
        float mass;
        float radius;
        glm::vec3 color;

    public:

        // Ajustado: construtor aceita (pos, vel, mass, radius, color[, acc])
        Particle(const Vec3& pos, const Vec3& vel, float m, float rad,
                 const glm::vec3& col = glm::vec3(1.0f), const Vec3& acc = Vec3::zero()) :
            position(pos),
            velocity(vel),
            acceleration(acc),
            forceAccumulator(Vec3::zero()),
            mass(std::max(1e-6f, m)),
            radius(rad),
            color(col) {}

        void applyForce(const Vec3& f) { forceAccumulator += f; }
        void clearForces() { forceAccumulator.clear(); }

        const Vec3& getPosition() const noexcept { return position; }
        void setPosition(const Vec3& p) noexcept { position = p; }
        const glm::vec3& getColor() const { return color; }

        float getMass() const { return mass; }
        void setMass(float m) { mass = std::max(1e-6f, m); }

        float getRadius() const { return radius; }
        void setRadius(float r) { radius = r; }

        void rk4_step(Vec3 Force, float timestep) {

                std::array<Vec3, 4> kp = {0, 0, 0, 0};
                std::array<Vec3, 4> kv = {acceleration, acceleration, acceleration, acceleration};

                Vec3 acceleration = Force / mass;
                Vec3 pos0 = position;
                Vec3 vel0 = velocity;

                //k1
                kp[0] = vel0;
                //k2
                kp[1] = vel0 * (timestep / 2) + kv[0] * (timestep * timestep / 2);
                
        }
};

#endif