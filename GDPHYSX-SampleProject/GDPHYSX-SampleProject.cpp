#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <sstream>
#include <fstream>

// TinyOBJLoader
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "p6/MyVector.h"
#include "p6/P6Particle.h"

// Global Variables
float x_mod = 0;
float z_mod = 0;
float y_mod = 0;
float scale = 0.1f;

void Key_Callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_D) x_mod += 0.1f;
    if (key == GLFW_KEY_A) x_mod -= 0.1f;
    if (key == GLFW_KEY_W) z_mod += 0.1f;
    if (key == GLFW_KEY_S) z_mod -= 0.1f;
}

bool checkCompileErrors(unsigned int shader, std::string type) {
    int success;
    char infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "| ERROR::SHADER: Compile-time error: Type: " << type << "\n"
                << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            return false;
        }
    }
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "| ERROR::Program: Link-time error: Type: " << type << "\n"
                << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            return false;
        }
    }
    return true;
}

int main(void) {
    std::fstream vertSrc("Shaders/Sample.vert");
    std::stringstream vertBuff;
    vertBuff << vertSrc.rdbuf();
    std::string vertS = vertBuff.str();
    const char* v = vertS.c_str();

    std::fstream fragSrc("Shaders/Sample.frag");
    std::stringstream fragBuff;
    fragBuff << fragSrc.rdbuf();
    std::string fragS = fragBuff.str();
    const char* f = fragS.c_str();

    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(700, 700, "Matthew Jayd Baldonado", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGL()) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &v, NULL);
    glCompileShader(vertexShader);
    if (!checkCompileErrors(vertexShader, "VERTEX")) return -1;

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &f, NULL);
    glCompileShader(fragmentShader);
    if (!checkCompileErrors(fragmentShader, "FRAGMENT")) return -1;

    GLuint shaderProg = glCreateProgram();
    glAttachShader(shaderProg, vertexShader);
    glAttachShader(shaderProg, fragmentShader);
    glLinkProgram(shaderProg);
    if (!checkCompileErrors(shaderProg, "PROGRAM")) return -1;

    glfwSetKeyCallback(window, Key_Callback);

    std::string path = "3D/sphere.obj";
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warning, error;
    tinyobj::attrib_t attributes;
    bool success = tinyobj::LoadObj(&attributes, &shapes, &materials, &warning, &error, path.c_str());
    if (!success) {
        std::cerr << "Failed to load OBJ file: " << error << std::endl;
        return -1;
    }

    std::vector<GLuint> mesh_indices;
    for (const auto& index : shapes[0].mesh.indices) {
        mesh_indices.push_back(index.vertex_index);
    }

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, attributes.vertices.size() * sizeof(float), attributes.vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh_indices.size() * sizeof(GLuint), mesh_indices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glm::mat4 projectionMatrix = glm::ortho(-350.f, 350.f, -350.f, 350.f, -350.f, 350.f);


    P6::MyVector scale(10, 10, 10);

    P6::P6Particle particles[4]{
        P6::P6Particle(),  // red
        P6::P6Particle(),  // green
        P6::P6Particle(),  // blue
        P6::P6Particle()   // yellow
    };

    particles[0].Position = P6::MyVector(-350, 350, 201);
    particles[1].Position = P6::MyVector(350, 350, 173);
    particles[2].Position = P6::MyVector(350, -350, -300);
    particles[3].Position = P6::MyVector(-350, -350, -150);

    particles[0].Acceleration = P6::MyVector(14.5, -14.5, -14.5);
    particles[0].Velocity = P6::MyVector(80, -80, -80);
    particles[1].Acceleration = P6::MyVector(-8, -8, -8);
    particles[1].Velocity = P6::MyVector(-90, -90, -90);
    particles[2].Acceleration = P6::MyVector(-1, 1, 1);
    particles[2].Velocity = P6::MyVector(-130, 130, 130);
    particles[3].Acceleration = P6::MyVector(3, 3, 3);
    particles[3].Velocity = P6::MyVector(110, 110, 110);

    glm::vec3 colors[4] = {
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, 0.0f)
    };

    constexpr std::chrono::nanoseconds timestep(12);
    using clock = std::chrono::high_resolution_clock;
    auto curr_time = clock::now();
    auto prev_time = curr_time;
    std::chrono::nanoseconds curr_ns(0);

    bool timeStop = false;
    bool isMoving = true;

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        curr_time = clock::now();
        auto dur = std::chrono::duration_cast<std::chrono::nanoseconds>(curr_time - prev_time);
        prev_time = curr_time;
        curr_ns += dur;

        if (curr_ns >= timestep && isMoving) {
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(curr_ns);
            curr_ns -= curr_ns;

            for (auto& particle : particles) {
                particle.update((float)ms.count() / 1000);
            }

            if (particles[0].Position.x == 0 && particles[0].Position.y == 0 &&
                particles[1].Position.x == 0 && particles[1].Position.y == 0 &&
                particles[2].Position.x == 0 && particles[2].Position.y == 0 &&
                particles[3].Position.x == 0 && particles[3].Position.y == 0 && !timeStop) {
                timeStop = true;
                isMoving = false;
                for (auto& particle : particles) {
                    particle.Acceleration = P6::MyVector(0, 0, 0);
                    particle.Velocity = P6::MyVector(0, 0, 0);
                }
                auto start_time = clock::now();
                std::chrono::duration<double> time_taken = start_time - prev_time;
                std::cout << "It took " << time_taken.count() << " seconds for it to land" << std::endl;
            }
        }

        glUseProgram(shaderProg);
        unsigned int projectionLoc = glGetUniformLocation(shaderProg, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

        glBindVertexArray(VAO);

        for (int i = 0; i < 4; ++i) {
            glm::mat4 transformation_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(particles[i].Position));
            transformation_matrix = glm::scale(transformation_matrix, glm::vec3(scale));
            transformation_matrix = glm::rotate(transformation_matrix, glm::radians(0.0f), glm::normalize(glm::vec3(0.0, 1.0, 0.0)));

            unsigned int transformLoc = glGetUniformLocation(shaderProg, "transform");
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformation_matrix));

            glDrawElements(GL_TRIANGLES, mesh_indices.size(), GL_UNSIGNED_INT, 0);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}
