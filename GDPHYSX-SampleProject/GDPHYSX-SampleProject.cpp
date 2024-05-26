#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>

#include <vector>
#include <iomanip>
#include <cmath>
#include <limits>

//include for time
#include <chrono>
using namespace std::chrono_literals;

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "p6/MyVector.h"
#include "p6/P6Particle.h"


float x_mod = 0;
float z_mod = 0;
float y_mod = 0;
float scale = 0.1f;
float orig = 1.0f;

void Key_Callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_D) x_mod += 0.1f;
    if (key == GLFW_KEY_A) x_mod -= 0.1f;
    if (key == GLFW_KEY_W) z_mod += 0.1f;
    if (key == GLFW_KEY_S) z_mod -= 0.1f;
}

static std::string numbering(int num) {
    switch (num % 10) {
    case 1:
        return "st";

    case 2:
        return "nd";

    case 3:
        return "rd";

    default:
        return "th";
    }
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

    //Create projection matrix
    glm::mat4 projectionMatrix = glm::ortho(-350.f, //L
        350.f,//R
        -350.f,//B
        350.f,//T
        -350.f,//Znear
        350.f);//Zfar


    /*P6::MyVector position(0, -350, 0);*/
    P6::MyVector scale(10, 10, 10);


    //time in between frames
    constexpr std::chrono::nanoseconds timestep(16ms);

    //particle array
    P6::P6Particle particles[4]{
        P6::P6Particle(),  // red
        P6::P6Particle(),  // green
        P6::P6Particle(),  // blue
        P6::P6Particle()   // yellow
    };

    //colors
    glm::vec3 particlecolor[4] = {
        glm::vec3(1.0f, 0.0f, 0.0f), //red
        glm::vec3(0.0f, 1.0f, 0.0f), //green
        glm::vec3(0.0f, 0.0f, 1.0f), //blue
        glm::vec3(1.0f, 1.0f, 0.0f) //yellow
    };

    //particle

    //top left (red)
    particles[0].Position = P6::MyVector(-350, 350, 201);
    //top right (green)
    particles[1].Position = P6::MyVector(350, 350, 173);
    //bottom right (blue)
    particles[2].Position = P6::MyVector(350, -350, -300);
    //bottom left (yellow)
    particles[3].Position = P6::MyVector(-350, -350, -150);


    ////this is 100m/s to the right
    //particle.Velocity = P6::MyVector(0, 0, 0);

    //this is 100m/s to the left

    //particle red
    particles[0].Acceleration = particles[0].Position.Direction().scalarMultiplication(14.5f).scalarMultiplication(-1.f);
    particles[0].Velocity = particles[0].Position.Direction().scalarMultiplication(80.f).scalarMultiplication(-1.f);
    //particle green
    particles[1].Acceleration = particles[1].Position.Direction().scalarMultiplication(8.f).scalarMultiplication(-1.f);
    particles[1].Velocity = particles[1].Position.Direction().scalarMultiplication(90.f).scalarMultiplication(-1.f);
    //particle blue
    particles[2].Acceleration = particles[2].Position.Direction().scalarMultiplication(1.f).scalarMultiplication(-1.f);
    particles[2].Velocity = particles[2].Position.Direction().scalarMultiplication(130.f).scalarMultiplication(-1.f);
    //particle yellow
    particles[3].Acceleration = particles[3].Position.Direction().scalarMultiplication(3.f).scalarMultiplication(-1.f);
    particles[3].Velocity = particles[3].Position.Direction().scalarMultiplication(110.f).scalarMultiplication(-1.f);

    ////initial velocity for computation
    //glm::vec3 iVelocity[4] = {
    //    glm::vec3(80, -80, -80), //red
    //    glm::vec3(-90, -90, -90), //green
    //    glm::vec3(-130, 130, 130), //blue
    //    glm::vec3(110, 110, 110) //yellow
    //};

    //initial velocity for computation
    P6::MyVector iVelocity[4] = {
        particles[0].Velocity, //red
        particles[1].Velocity, //blue
        particles[2].Velocity, //greeen
        particles[3].Velocity //yellow
    };



    //initializing clock variables
    using clock = std::chrono::high_resolution_clock;
    auto curr_time = clock::now();
    auto prev_time = curr_time;
    std::chrono::nanoseconds curr_ns(0);

    bool timeStop = false;
    bool isMoving = true;

    bool end_race = false;
    bool resultPrinted = false;

    std::vector<bool> finished(4, false);
    std::vector<float> MagVelocity(4, 0.0f);

    std::vector<std::chrono::time_point<std::chrono::high_resolution_clock>> startTime(4, clock::now());
    std::vector<std::chrono::time_point<std::chrono::high_resolution_clock>> endTime(4);




    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        curr_time = clock::now();
        auto dur = std::chrono::duration_cast<std::chrono::nanoseconds>(curr_time - prev_time);
        prev_time = curr_time;
        curr_ns += dur;


        if (curr_ns >= timestep) {
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(curr_ns);
            curr_ns -= curr_ns;
            end_race = true;

            for (int i = 0; i < 4; i++) { //double check again
                if (!finished[i]) {

                    particles[i].update((float)ms.count() / 1000);
                    int pos_x = particles[i].Position.x;
                    int pos_y = particles[i].Position.y;
                    int pos_z = particles[i].Position.z;

                    if (std::abs(pos_x) < orig && std::abs(pos_y) < orig && std::abs(pos_z) < orig) {
                        finished[i] = true;
                        endTime[i] = clock::now();
                        MagVelocity[i] = particles[i].Velocity.Magnitude();
                    }
                    else {
                        end_race = false;
                    }
                }
            }

            if (end_race && !resultPrinted) {
                std::vector<int> index = { 0, 1, 2, 3 };

                for (int i = 0; i < index.size() - 1; i++) {
                    for (int j = 0; j < index.size() - i - 1; j++) {
                        auto time_diff_a = std::chrono::duration_cast<std::chrono::milliseconds>(endTime[index[j]] - startTime[index[j]]);
                        auto time_diff_b = std::chrono::duration_cast<std::chrono::milliseconds>(endTime[index[j + 1]] - startTime[index[j + 1]]);

                        if (time_diff_a.count() > time_diff_b.count()) {
                            std::swap(index[j], index[j + 1]);
                        }
                    }
                }

                int rank = 1;
                for (int i : index) {
                    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(endTime[i] - startTime[i]).count();

                    float avgVelocityX = (iVelocity[i].x + particles[i].Velocity.x) / 2.0f;
                    float avgVelocityY = (iVelocity[i].y + particles[i].Velocity.y) / 2.0f;
                    float avgVelocityZ = (iVelocity[i].z + particles[i].Velocity.z) / 2.0f;

                    std::cout << rank << numbering(rank) << " : ";

                    //Particle display pain int he ass                   

                    if (particlecolor[i] == glm::vec3(0.0f, 1.0f, 0.0f)) {
                        std::cout << "Green" << std::endl;
                    }
                    else if (particlecolor[i] == glm::vec3(1.0f, 0.0f, 0.0f)) {
                        std::cout << "Red" << std::endl;
                    }
                    else if (particlecolor[i] == glm::vec3(0.0f, 0.0f, 1.0f)) {
                        std::cout << "Blue" << std::endl;
                    }
                    else if (particlecolor[i] == glm::vec3(1.0f, 1.0f, 0.0f)) {
                        std::cout << "Yellow" << std::endl;
                    }

                    std::cout << "Mag. of Velocity: " << std::fixed << std::setprecision(2) << MagVelocity[i] << " m/s" << std::endl;
                    std::cout << "Average Velocity: (" << std::fixed << std::setprecision(2) << avgVelocityX << ", " << avgVelocityY << ", " << avgVelocityZ << ") m/s" << std::endl;
                    std::cout << static_cast<float>(elapsed / 1000.f) << " secs" << std::endl;
                    std::cout << "\n";
                    rank++;
                }
                resultPrinted = true;
            }

        }
        //
        glUseProgram(shaderProg);
        unsigned int projectionLoc = glGetUniformLocation(shaderProg, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

        glBindVertexArray(VAO);

        //draw array of particles
        for (int i = 0; i < 4; ++i) {
            glm::mat4 transformation_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(particles[i].Position));
            transformation_matrix = glm::scale(transformation_matrix, glm::vec3(scale));
            transformation_matrix = glm::rotate(transformation_matrix, glm::radians(0.0f), glm::normalize(glm::vec3(0.0, 1.0, 0.0)));

            unsigned int transformLoc = glGetUniformLocation(shaderProg, "transform");
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformation_matrix));

            //set color
            unsigned int colorLoc = glGetUniformLocation(shaderProg, "objectColor");
            glUniform3fv(colorLoc, 1, glm::value_ptr(particlecolor[i]));

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
