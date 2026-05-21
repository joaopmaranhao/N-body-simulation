#include "n_body_system.hpp"
 
static constexpr float G          = 6.674e-11f;
static constexpr float SOFTENING  = 0.1f;  // evita singularidade quando r -> 0
static constexpr float DEFAULT_DT = 0.016f;
 
NBodySystem::NBodySystem() : dt(DEFAULT_DT) {}
 
float NBodySystem::distance(const Vec3& a, const Vec3& b) {
        Vec3 delta = a - b;
        return delta.length();
}
 
void NBodySystem::addBody(const Body::Body& b) {
        bodies.push_back(b);
}
 
void NBodySystem::setBodyPosition(int index, const Vec3& pos) {
        bodies[index].setPosition(pos);
}
 
const std::shared_ptr<std::vector<Body::Body>> NBodySystem::getBodies() const {
        return std::make_shared<std::vector<Body::Body>>(bodies);
}
 
const float NBodySystem::getTimeStep() const {
        return dt;
}
 
// Acumula forças gravitacionais entre todos os pares O(n²)
static void accumulateGravity(std::vector<Body::Body>& bodies) {
        const std::size_t n = bodies.size();
 
        for (std::size_t i = 0; i < n; ++i) {
                for (std::size_t j = i + 1; j < n; ++j) {
                        Vec3  r    = bodies[j].getPosition() - bodies[i].getPosition();
                        float dist = r.length() + SOFTENING;
                        float mag  = G * bodies[i].getMass() * bodies[j].getMass()
                                   / std::pow(dist, 3);
 
                        Vec3 force = r * mag;
                        bodies[i].applyForce(force);
                        bodies[j].applyForce(force * -1.0f);
                }
        }
}
 
void NBodySystem::handleCollision(const Body::Body& b1, const Body::Body& b2) {
        // placeholder — colisões elásticas podem ser adicionadas aqui
        (void)b1;
        (void)b2;
}
 
void NBodySystem::handleCollisions() {
        const std::size_t n = bodies.size();
 
        for (std::size_t i = 0; i < n; ++i) {
                for (std::size_t j = i + 1; j < n; ++j) {
                        float d    = distance(bodies[i].getPosition(), bodies[j].getPosition());
                        float rSum = bodies[i].getRadius() + bodies[j].getRadius();
 
                        if (d < rSum) {
                                handleCollision(bodies[i], bodies[j]);
                        }
                }
        }
}
 
void NBodySystem::updateVerlet() {
        accumulateGravity(bodies);
 
        for (auto& body : bodies) {
                body.verlet_integration(dt);
        }
 
        handleCollisions();
}
 
void NBodySystem::updateRK4() {
        // stub — preencher quando rk4_step estiver implementado no Body
        accumulateGravity(bodies);
        handleCollisions();
}
