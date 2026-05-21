#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include <vector>
#include <cmath>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../math/vec3.hpp"

namespace Body{

struct Body{
    private:
        Vec3 position;
        Vec3 prev_position;
        Vec3 velocity;
        Vec3 acceleration;
        Vec3 forceAccumulator;
        float mass;
        float radius;
        glm::vec3 color;

    public:

        // Ajustado: construtor aceita (pos, vel, mass, radius, color[, acc])
        Body(const Vec3& pos, const Vec3& vel, float m, float rad,
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

        void verlet_integration(float dt){
            acceleration = forceAccumulator / mass;

            Vec3 temp     = position;
            Vec3 velocity = position - prev_position;

            position      = position + velocity + acceleration * (dt * dt);
            prev_position = temp;

            clearForces();
            acceleration.clear();
        }

        void rk4_step(Vec3 Force, float timestep) {
                //implementation later
        }
};

}

#endif