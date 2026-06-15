#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "renderer/renderer.hpp"
#include "physics/body.hpp"  
#include "physics/n_body_system.hpp"
#include <vector>
#include <iostream>

int main() {
        constexpr unsigned int W = 1280, H = 720;

        if (!glfwInit()) {
                std::cerr << "Falha ao inicializar GLFW\n";
                return -1;
        }
        
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        GLFWwindow* window = glfwCreateWindow(W, H, "N-Body 3D", nullptr, nullptr);
        if (!window) { std::cerr << "GLFW window failed\n"; glfwTerminate(); return -1; }
        glfwMakeContextCurrent(window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
                std::cerr << "GLAD failed\n"; return -1;
        }

        // -------------------------------------------------------------------------
        Renderer3D renderer(W, H);
        renderer.init();

        // Registra callbacks — window user pointer aponta para o renderer
        glfwSetWindowUserPointer(window, &renderer);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback_3d);
        glfwSetCursorPosCallback(window,   mouse_callback_3d);
        glfwSetScrollCallback(window,      scroll_callback_3d);
        glfwSetMouseButtonCallback(window, mouse_button_callback_3d);

        // -------------------------------------------------------------------------
        // INICIALIZAÇÃO DO SISTEMA FÍSICO
        // -------------------------------------------------------------------------
        NBodySystem physicsSystem;

        // Criando alguns corpos para teste (Valores arbitrários escalados para visualização)
        // Certifique-se de que o construtor da sua classe Body siga essa ordem ou ajuste os setters.
        // Exemplo hipotético: Body(posicao, velocidade, massa, raio)
        
        // 1. Uma estrela massiva no centro
        Body::Body sun;
        sun.setPosition(Vec3(0.0f, 0.0f, 0.0f));
        sun.setVelocity(Vec3(0.0f, 0.0f, 0.0f));
        sun.setMass(1.0e13f); // Massa grande para atrair os outros
        sun.setRadius(5.0f);  // Raio visual/físico
        physicsSystem.addBody(sun);

        // 2. Planeta interno em órbita
        Body::Body planet1;
        planet1.setPosition(Vec3(20.0f, 0.0f, 0.0f));
        planet1.setVelocity(Vec3(0.0f, 0.0f, 5.0f)); // Velocidade tangencial para orbitar
        planet1.setMass(1.0e8f);
        planet1.setRadius(1.2f);
        physicsSystem.addBody(planet1);

        // 3. Planeta externo em órbita oposta ou diferente
        Body::Body planet2;
        planet2.setPosition(Vec3(-35.0f, 0.0f, 0.0f));
        planet2.setVelocity(Vec3(0.0f, 0.0f, -4.0f));
        planet2.setMass(2.0e8f);
        planet2.setRadius(1.8f);
        physicsSystem.addBody(planet2);

        float lastTime = 0.0f;

        // -------------------------------------------------------------------------
        while (!glfwWindowShouldClose(window)) {
                float now = static_cast<float>(glfwGetTime());
                float dt  = now - lastTime;
                lastTime  = now;

                // Evita saltos gigantescos na física se a janela for arrastada ou travar
                if (dt > 0.1f) dt = 0.1f; 

                renderer.processInput(window, dt);

                // Passo de física (Usando o Verlet que está implementado no seu sistema)
                // Nota: O seu `updateVerlet` usa o `dt` interno fixo (0.016s). 
                // Se quiser física em tempo real variável, você precisará adaptar sua classe para aceitar o dt do frame.
                physicsSystem.updateVerlet();

                // Obtém a lista atualizada de corpos para renderizar
                // Como getBodies retorna um shared_ptr, pegamos a referência do vetor guardado nele
                const auto& currentBodies = physicsSystem.getBodies();

                // Passa os corpos atualizados para o renderer
                renderer.render(currentBodies, dt);

                glfwSwapBuffers(window);
                glfwPollEvents();
        }

        renderer.cleanup();
        glfwDestroyWindow(window);
        glfwTerminate();
        return 0;
}