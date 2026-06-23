#include "renderer.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <vector>

// =============================================================================
// Shaders
// =============================================================================

// Vertex shader:
//   - Recebe a malha da esfera em model-space
//   - Recebe por instância: posição no mundo, raio, cor
//   - Constrói a model matrix por instância (translate + scale uniforme)
//   - Passa Normal e FragPos para o fragment shader (Phong)
static const char* VERT_SRC = R"glsl(
#version 330 core

// Atributos da malha da esfera (iguais para todas as instâncias)
layout(location = 0) in vec3 aPos;      // posição em model-space
layout(location = 1) in vec3 aNormal;   // normal em model-space

// Atributos por instância (um por corpo)
layout(location = 2) in vec3  iPos;     // posição no mundo
layout(location = 3) in float iRadius;  // raio
layout(location = 4) in vec3  iColor;   // cor RGB

uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Normal;
out vec3 Color;

void main() {
    // Model matrix = translate(iPos) * scale(iRadius)
    mat4 model = mat4(1.0);
    model[0][0] = iRadius;
    model[1][1] = iRadius;
    model[2][2] = iRadius;
    model[3]    = vec4(iPos, 1.0);

    vec4 worldPos = model * vec4(aPos, 1.0);
    FragPos  = vec3(worldPos);
    Normal   = mat3(transpose(inverse(model))) * aNormal;
    Color    = iColor;

    gl_Position = projection * view * worldPos;
}
)glsl";

// Fragment shader: Phong com luz direcional
static const char* FRAG_SRC = R"glsl(
#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec3 Color;

out vec4 FragColor;

uniform vec3 lightDir;   // direção da luz (normalizada, aponta PARA a luz)
uniform vec3 viewPos;    // posição da câmera

void main() {
    vec3 norm     = normalize(Normal);
    vec3 light    = normalize(lightDir);

    // Ambient
    float ambientStrength = 0.15;
    vec3 ambient = ambientStrength * Color;

    // Diffuse
    float diff    = max(dot(norm, light), 0.0);
    vec3 diffuse  = diff * Color;

    // Specular (Blinn-Phong)
    float specularStrength = 0.5;
    vec3 viewDir  = normalize(viewPos - FragPos);
    vec3 halfDir  = normalize(light + viewDir);
    float spec    = pow(max(dot(norm, halfDir), 0.0), 32.0);
    vec3 specular = specularStrength * spec * vec3(1.0);

    vec3 result = ambient + diffuse + specular;
    FragColor   = vec4(result, 1.0);
}
)glsl";

// =============================================================================
// Renderer3D
// =============================================================================

Renderer3D::Renderer3D(unsigned int scrW, unsigned int scrH)
    : scrWidth(scrW), scrHeight(scrH)
    , shaderProgram(0)
    , sphereVAO(0), sphereVBO(0), sphereEBO(0), indexCount(0)
    , instanceVBO(0)
{}

// -----------------------------------------------------------------------------
void Renderer3D::init() {
    // Compila shaders
    unsigned int vert = compileShader(GL_VERTEX_SHADER,   VERT_SRC);
    unsigned int frag = compileShader(GL_FRAGMENT_SHADER, FRAG_SRC);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vert);
    glAttachShader(shaderProgram, frag);
    glLinkProgram(shaderProgram);

    int ok; char log[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &ok);
    if (!ok) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, log);
        throw std::runtime_error(std::string("Shader link error: ") + log);
    }
    glDeleteShader(vert);
    glDeleteShader(frag);

    glEnable(GL_DEPTH_TEST);

    buildSphereMesh();
    updateCamera();
}

// -----------------------------------------------------------------------------
// Gera a malha UV de uma esfera unitária (raio = 1).
// Layout: [x, y, z, nx, ny, nz] intercalado → stride = 6 floats.
// Índices: triângulos via EBO.
void Renderer3D::buildSphereMesh() {
    std::vector<float>        vertices;
    std::vector<unsigned int> indices;

    const float PI = 3.14159265358979f;

    // Vértices
    for (int i = 0; i <= sphereStacks; ++i) {
        float phi   = PI / 2.0f - i * (PI / sphereStacks);   // [-PI/2, PI/2]
        float y     = std::sin(phi);
        float cosP  = std::cos(phi);

        for (int j = 0; j <= sphereSectors; ++j) {
            float theta = j * (2.0f * PI / sphereSectors);
            float x = cosP * std::cos(theta);
            float z = cosP * std::sin(theta);

            // posição (= normal para esfera unitária)
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            // normal
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        }
    }

    // Índices (dois triângulos por quad)
    int cols = sphereSectors + 1;
    for (int i = 0; i < sphereStacks; ++i) {
        for (int j = 0; j < sphereSectors; ++j) {
            unsigned int a = i * cols + j;
            unsigned int b = a + cols;

            indices.push_back(a);
            indices.push_back(b);
            indices.push_back(a + 1);

            indices.push_back(a + 1);
            indices.push_back(b);
            indices.push_back(b + 1);
        }
    }
    indexCount = static_cast<unsigned int>(indices.size());

    // VAO / VBO / EBO da malha
    glGenVertexArrays(1, &sphereVAO);
    glGenBuffers(1, &sphereVBO);
    glGenBuffers(1, &sphereEBO);

    glBindVertexArray(sphereVAO);

    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER,
                 vertices.size() * sizeof(float),
                 vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 indices.size() * sizeof(unsigned int),
                 indices.data(), GL_STATIC_DRAW);

    constexpr int stride = 6 * sizeof(float);
    // location 0: aPos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    // location 1: aNormal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Instance VBO (alocado vazio, expandido em updateInstanceBuffer)
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);

    // Layout do buffer de instâncias: [x, y, z, radius, r, g, b]
    //   → 7 floats por instância
    constexpr int iStride = 7 * sizeof(float);

    // location 2: iPos (vec3)
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, iStride, (void*)0);
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);   // uma por instância

    // location 3: iRadius (float)
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, iStride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1);

    // location 4: iColor (vec3)
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, iStride, (void*)(4 * sizeof(float)));
    glEnableVertexAttribArray(4);
    glVertexAttribDivisor(4, 1);

    glBindVertexArray(0);
}

// -----------------------------------------------------------------------------
// Preenche o instanceVBO com posição, raio e cor de cada corpo.
void Renderer3D::updateInstanceBuffer(const std::vector<Body::Body>& bodies) {
    // Calcula min/max de velocidade para normalizar a cor
    float minSpeed = 1e30f, maxSpeed = 0.0f;
    for (const auto& b : bodies) {
        float s = glm::length(glm::vec3(
            b.getVelocity().getX(),
            b.getVelocity().getY(),
            b.getVelocity().getZ()
        ));                          
        minSpeed = std::min(minSpeed, s);
        maxSpeed = std::max(maxSpeed, s);
    }
    if (maxSpeed <= minSpeed) maxSpeed = minSpeed + 1.0f;

    // Monta buffer flat: [x, y, z, r_sphere, cr, cg, cb]
    std::vector<float> data;
    data.reserve(bodies.size() * 7);

    for (const auto& b : bodies) {
        float speed = glm::length(glm::vec3(
            b.getVelocity().getX(),
            b.getVelocity().getY(),
            b.getVelocity().getZ()
        ));                                   
        glm::vec3 col = velocityToColor(speed, minSpeed, maxSpeed);

        data.push_back(b.getPosition().getX());
        data.push_back(b.getPosition().getY());
        data.push_back(b.getPosition().getZ());
        data.push_back(b.getRadius());          // ajuste se seu Body usar outro campo
        data.push_back(col.r);
        data.push_back(col.g);
        data.push_back(col.b);
    }

    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER,
                 data.size() * sizeof(float),
                 data.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// -----------------------------------------------------------------------------
void Renderer3D::updateCamera() {
    const float PI = 3.14159265358979f;
    float yawR   = glm::radians(camYaw);
    float pitchR = glm::radians(camPitch);

    glm::vec3 eye;
    eye.x = camRadius * std::cos(pitchR) * std::cos(yawR);
    eye.y = camRadius * std::sin(pitchR);
    eye.z = camRadius * std::cos(pitchR) * std::sin(yawR);

    view       = glm::lookAt(eye, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    projection = glm::perspective(glm::radians(camFov),
                                   (float)scrWidth / (float)scrHeight,
                                   0.1f, camRadius * 10.0f);

    // Armazena posição da câmera para o specular
    glm::vec4 vp = glm::inverse(view) * glm::vec4(0, 0, 0, 1);
    // viewPos enviado em render()
}

// -----------------------------------------------------------------------------
void Renderer3D::render(const std::vector<Body::Body>& bodies, float /*dt*/) {
    glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (bodies.empty()) return;

    updateCamera();
    updateInstanceBuffer(bodies);

    glUseProgram(shaderProgram);

    // Uniforms de câmera
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"),
                       1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"),
                       1, GL_FALSE, glm::value_ptr(projection));

    // Posição da câmera (para specular)
    float yawR   = glm::radians(camYaw);
    float pitchR = glm::radians(camPitch);
    glm::vec3 eye;
    eye.x = camRadius * std::cos(pitchR) * std::cos(yawR);
    eye.y = camRadius * std::sin(pitchR);
    eye.z = camRadius * std::cos(pitchR) * std::sin(yawR);
    glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"),
                 1, glm::value_ptr(eye));

    // Luz direcional (vem de cima-direita-frente)
    glm::vec3 lightDir = glm::normalize(glm::vec3(1.0f, 2.0f, 1.5f));
    glUniform3fv(glGetUniformLocation(shaderProgram, "lightDir"),
                 1, glm::value_ptr(lightDir));

    // Desenha todas as instâncias de uma vez
    glBindVertexArray(sphereVAO);
    glDrawElementsInstanced(GL_TRIANGLES,
                            static_cast<GLsizei>(indexCount),
                            GL_UNSIGNED_INT,
                            nullptr,
                            static_cast<GLsizei>(bodies.size()));
    glBindVertexArray(0);
}

// -----------------------------------------------------------------------------
void Renderer3D::processInput(GLFWwindow* window, float dt) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Zoom por teclado (complementar ao scroll)
    float speed = camRadius * 0.5f * dt;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camRadius -= speed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camRadius += speed;
    if (camRadius < 1.0f) camRadius = 1.0f;
}

// -----------------------------------------------------------------------------
void Renderer3D::onMouseMove(double xpos, double ypos) {
    if (!mousePressed) return;

    if (firstMouse) {
        lastMouseX = static_cast<float>(xpos);
        lastMouseY = static_cast<float>(ypos);
        firstMouse = false;
    }

    float dx =  (static_cast<float>(xpos) - lastMouseX) * 0.3f;
    float dy = -(static_cast<float>(ypos) - lastMouseY) * 0.3f;  // invertido Y
    lastMouseX = static_cast<float>(xpos);
    lastMouseY = static_cast<float>(ypos);

    camYaw   += dx;
    camPitch += dy;
    camPitch  = glm::clamp(camPitch, -89.0f, 89.0f);
}

// -----------------------------------------------------------------------------
void Renderer3D::onScroll(double /*xoffset*/, double yoffset) {
    camRadius -= static_cast<float>(yoffset) * camRadius * 0.1f;
    if (camRadius < 1.0f) camRadius = 1.0f;
}

// -----------------------------------------------------------------------------
void Renderer3D::cleanup() {
    glDeleteVertexArrays(1, &sphereVAO);
    glDeleteBuffers(1, &sphereVBO);
    glDeleteBuffers(1, &sphereEBO);
    glDeleteBuffers(1, &instanceVBO);
    glDeleteProgram(shaderProgram);
}

// -----------------------------------------------------------------------------
unsigned int Renderer3D::compileShader(unsigned int type, const char* src) {
    unsigned int id = glCreateShader(type);
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int ok; char log[512];
    glGetShaderiv(id, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        glGetShaderInfoLog(id, 512, nullptr, log);
        throw std::runtime_error(std::string("Shader compile error: ") + log);
    }
    return id;
}

// -----------------------------------------------------------------------------
glm::vec3 Renderer3D::velocityToColor(float speed, float minSpeed, float maxSpeed) {
    float t = (speed - minSpeed) / (maxSpeed - minSpeed);
    t = glm::clamp(t, 0.0f, 1.0f);

    // Azul (lento) → ciano → verde → amarelo → vermelho (rápido)
    glm::vec3 colors[5] = {
        {0.2f, 0.4f, 1.0f},   // azul
        {0.0f, 0.9f, 0.9f},   // ciano
        {0.0f, 0.9f, 0.2f},   // verde
        {1.0f, 0.8f, 0.0f},   // amarelo
        {1.0f, 0.1f, 0.0f},   // vermelho
    };
    float scaled = t * 4.0f;
    int   lo     = (int)scaled;
    float frac   = scaled - lo;
    if (lo >= 4) return colors[4];
    return glm::mix(colors[lo], colors[lo + 1], frac);
}

// =============================================================================
// Callbacks GLFW
// Registre com:
//   glfwSetWindowUserPointer(window, &renderer);
//   glfwSetFramebufferSizeCallback(window, framebuffer_size_callback_3d);
//   glfwSetCursorPosCallback(window,    mouse_callback_3d);
//   glfwSetScrollCallback(window,       scroll_callback_3d);
//   glfwSetMouseButtonCallback(window,  mouse_button_callback_3d);
// =============================================================================

void framebuffer_size_callback_3d(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    auto* r = static_cast<Renderer3D*>(glfwGetWindowUserPointer(window));
    if (r) { r->scrWidth = width; r->scrHeight = height; }
}

void mouse_callback_3d(GLFWwindow* window, double xpos, double ypos) {
    auto* r = static_cast<Renderer3D*>(glfwGetWindowUserPointer(window));
    if (r) r->onMouseMove(xpos, ypos);
}

void scroll_callback_3d(GLFWwindow* window, double xoffset, double yoffset) {
    auto* r = static_cast<Renderer3D*>(glfwGetWindowUserPointer(window));
    if (r) r->onScroll(xoffset, yoffset);
}

void mouse_button_callback_3d(GLFWwindow* window, int button, int action, int /*mods*/) {
    auto* r = static_cast<Renderer3D*>(glfwGetWindowUserPointer(window));
    if (!r) return;
    
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            r->setMousePressed(true);
            r->setFirstMouse(true);   // reseta referência ao começar o arrasto do mouse
        } else {
            r->setMousePressed(false);
        }
    }
}
