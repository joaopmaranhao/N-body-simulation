#ifndef RENDERER3D_HPP
#define RENDERER3D_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "../physics/body.hpp"

using uint32 = unsigned int; 

class Renderer3D {
public:
    uint32 scrWidth;
    uint32 scrHeight;

    // Câmera orbital
    float camRadius   = 200.0f;   // distância ao centro da cena
    float camYaw      = 45.0f;    // graus
    float camPitch    = 30.0f;    // graus
    float camFov      = 45.0f;

    // Qualidade da esfera
    int sphereStacks  = 16;
    int sphereSectors = 24;

    Renderer3D(uint32 scrW, uint32 scrH);

    void init();
    void render(const std::vector<Body::Body>& bodies, float dt);
    void processInput(GLFWwindow* window, float dt);
    void cleanup();

    // Callbacks internos acionados pelos callbacks do GLFW
    void onMouseMove(double xpos, double ypos);
    void onScroll(double xoffset, double yoffset);

    // Getters/Setters públicos para permitir que a função global altere o estado do mouse
    void setMousePressed(bool pressed) { mousePressed = pressed; }
    void setFirstMouse(bool first)     { firstMouse = first; }

private:
    // Shader
    uint32 shaderProgram;
    uint32 compileShader(uint32 type, const char* src);

    // Sphere mesh
    uint32 sphereVAO, sphereVBO, sphereEBO;
    uint32 indexCount;
    void buildSphereMesh();

    // Instance buffer
    uint32 instanceVBO;
    void updateInstanceBuffer(const std::vector<Body::Body>& bodies);

    // Matrizes
    glm::mat4 view;
    glm::mat4 projection;
    void updateCamera();

    // Estado do mouse (Declarados apenas UMA vez aqui no private)
    bool  firstMouse = true;
    bool  mousePressed = false;
    float lastMouseX = 0.0f;
    float lastMouseY = 0.0f;

    // Utilitário de cor
    glm::vec3 velocityToColor(float speed, float minSpeed, float maxSpeed);
};

// Callbacks Globais do GLFW — Conectados via glfwSetWindowUserPointer
void framebuffer_size_callback_3d(GLFWwindow* window, int width, int height);
void mouse_callback_3d(GLFWwindow* window, double xpos, double ypos);
void scroll_callback_3d(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback_3d(GLFWwindow* window, int button, int action, int mods);

#endif // RENDERER3D_HPP