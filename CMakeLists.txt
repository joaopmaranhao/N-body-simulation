cmake_minimum_required(VERSION 3.16)
project(NBody3D LANGUAGES C CXX) # Adicionado 'C' aqui explicitamente

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# =============================================================================
# Dependências do sistema
# =============================================================================
find_package(OpenGL REQUIRED)
find_package(glfw3  REQUIRED)

# =============================================================================
# Criar uma biblioteca separada apenas para o GLAD (Garante a compilação em C)
# =============================================================================
add_library(glad_lib STATIC 
    ${CMAKE_SOURCE_DIR}/libs/glad/src/glad.c
)
target_include_directories(glad_lib PUBLIC 
    ${CMAKE_SOURCE_DIR}/libs/glad/include
)

# =============================================================================
# Alvo Principal (Executável)
# =============================================================================
add_executable(NBody3D
    main.cpp
    src/physics/n_body_system.cpp
    src/renderer/renderer.cpp
)

# =============================================================================
# Include dirs para o executável
# =============================================================================
target_include_directories(NBody3D PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/libs/glad/include   # glad.h + KHR/khrplatform.h
    ${CMAKE_SOURCE_DIR}/libs/glm            # glm/glm.hpp
)

# =============================================================================
# Vinculando as bibliotecas (Vincular glad_lib aqui força o linker a achar tudo)
# =============================================================================
target_link_libraries(NBody3D PRIVATE
    glad_lib
    OpenGL::GL
    glfw
)

# Linux precisa de dl para GLAD
if(UNIX AND NOT APPLE)
    target_link_libraries(NBody3D PRIVATE dl)
endif()

# =============================================================================
# Flags de build
# =============================================================================
target_compile_options(NBody3D PRIVATE
    $<$<CONFIG:Debug>:   -g -O0 -Wall -Wextra>
    $<$<CONFIG:Release>: -O3 -DNDEBUG -march=native>
)