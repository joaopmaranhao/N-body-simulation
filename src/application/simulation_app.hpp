#ifndef SIMULATION_APP_HPP
#define SIMULATION_APP_HPP

#include <memory>
#include <string>

class GLFWwindow;
class NBodySystem;
class Renderer3D;

class SimulationApp {
public:
    explicit SimulationApp(
        std::string title = "N-Body 3D",
        unsigned int width = 1280,
        unsigned int height = 720
    );
    ~SimulationApp();

    SimulationApp(const SimulationApp&) = delete;
    SimulationApp& operator=(const SimulationApp&) = delete;

    int run(NBodySystem& system);

private:
    void initialize();
    void shutdown() noexcept;

    std::string title;
    unsigned int width;
    unsigned int height;
    GLFWwindow* window = nullptr;
    std::unique_ptr<Renderer3D> renderer;
    bool glfwInitialized = false;
};

#endif // SIMULATION_APP_HPP
