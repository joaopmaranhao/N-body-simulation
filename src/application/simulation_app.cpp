#include "simulation_app.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "physics/n_body_system.hpp"
#include "renderer/renderer.hpp"

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <utility>

SimulationApp::SimulationApp(std::string title, unsigned int width, unsigned int height)
    : title(std::move(title)), width(width), height(height) {}

SimulationApp::~SimulationApp() {
    shutdown();
}

int SimulationApp::run(NBodySystem& system) {
    try {
        initialize();

        float lastTime = static_cast<float>(glfwGetTime());
        while (!glfwWindowShouldClose(window)) {
            float now = static_cast<float>(glfwGetTime());
            float dt = std::min(now - lastTime, 0.1f);
            lastTime = now;

            renderer->processInput(window, dt);
            system.updateVerlet();
            renderer->render(system.getBodies(), dt);

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        shutdown();
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Falha ao executar a simulacao: " << error.what() << '\n';
        shutdown();
        return 1;
    }
}

void SimulationApp::initialize() {
    if (!glfwInit()) {
        throw std::runtime_error("falha ao inicializar GLFW");
    }
    glfwInitialized = true;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window) {
        throw std::runtime_error("falha ao criar a janela GLFW");
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        throw std::runtime_error("falha ao inicializar GLAD");
    }

    renderer = std::make_unique<Renderer3D>(width, height);
    renderer->init();

    glfwSetWindowUserPointer(window, renderer.get());
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback_3d);
    glfwSetCursorPosCallback(window, mouse_callback_3d);
    glfwSetScrollCallback(window, scroll_callback_3d);
    glfwSetMouseButtonCallback(window, mouse_button_callback_3d);
}

void SimulationApp::shutdown() noexcept {
    if (renderer) {
        renderer->cleanup();
        renderer.reset();
    }

    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }

    if (glfwInitialized) {
        glfwTerminate();
        glfwInitialized = false;
    }
}
