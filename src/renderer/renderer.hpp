#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>

#include "../physics/body.hpp"

// =============================================================================
// Renderer
//
// Responsável por toda a comunicação com a GPU:
//   - compilação e linkagem dos shaders GLSL
//   - alocação e atualização dos buffers OpenGL (VAO/VBO)
//   - mapeamento velocidade -> cor (azul lento / vermelho rápido)
//   - desenho de cada partícula como GL_POINT circular com borda suavizada
//
// Uso típico:
//   Renderer r(SCR_WIDTH, SCR_HEIGHT, WORLD_WIDTH, WORLD_HEIGHT);
//   r.init();                         // compila shaders, aloca buffers
//   r.draw(particles, dt);            // chama a cada frame
//   r.cleanup();                      // libera recursos OpenGL
// =============================================================================

class Renderer {
public:
        unsigned int scrWidth;
        unsigned int scrHeight;

        float worldWidth;
        float worldHeight;

        float sizeScale = 40.0f;

        Renderer(unsigned int scrW, unsigned int scrH,
                 float worldW, float worldH);

        void init();

        void render(const std::vector<Body::Body>& bodies, float dt);

        void cleanup();

private:
        unsigned int shaderProgram;
        unsigned int VAO, VBO;
        glm::mat4 projection;

        unsigned int compileShader(unsigned int type, const char* source);
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

#endif