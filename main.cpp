#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <memory>
#include <chrono>
#include <random>

#include "triangle.h"
#include "rectangle.h"
#include "circle.h"
#include "polygon.h"
#include "scene.h"
#include "transformEditor.h"
#include "camera.h"
#include "particle.h"

// Window dimensions
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// Function declarations
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
glm::vec2 getRandomPosition(float minX, float maxX, float minY, float maxY);
glm::vec3 getColor(int numTypes, int typeIndex);

float getValueBetween(float min, float max)
{
    std::random_device rd;
    std::uniform_real_distribution<float> dist(min, max);
    return dist(rd);
}

int main()
{
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a GLFW window
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "tuf2D", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Center the window on the screen
    const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    int xpos = (mode->width - SCR_WIDTH) / 2;
    int ypos = (mode->height - SCR_HEIGHT) / 2;
    glfwSetWindowPos(window, xpos, ypos);

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Setup Dear ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Create our scene
    Scene2D scene;
    scene.Initialize("res/shaders/default.vert", "res/shaders/default.frag");

    // auto circle = scene.CreateShape<Circle2D>(0.15f, 32);
    // circle->SetPosition(glm::vec2(0.0f, 0.0f));
    // circle->SetColor(glm::vec3(0.0f, 0.0f, 1.0f)); // Blue

    // Setup transform editor
    TransformEditor editor;
    // editor.SelectShape(circle, "Circle");

    // Setup viewport
    int viewport_width, viewport_height;
    glfwGetFramebufferSize(window, &viewport_width, &viewport_height);
    glViewport(0, 0, viewport_width, viewport_height);

    Camera2D camera;
    camera.UpdateAspectRatio(viewport_width, viewport_height);
    glfwSetWindowUserPointer(window, &camera);

    // Variables for FPS and render time calculation
    int frameCount = 0;
    float fps = 0.0f;
    float frameTime = 0.0f;
    float lastTime = 0.0f;
    float currentTime = 0.0f;
    float lastFpsUpdateTime = 0.0f;
    std::chrono::high_resolution_clock::time_point frameStartTime;
    std::chrono::high_resolution_clock::time_point frameEndTime;

    std::vector<Particle> particles;

    int numParticles = 1500;
    int numTypes = 7;
    std::vector<std::shared_ptr<Shape2D>> particleShapes;

    // Initialize interaction matrices with meaningful default values
    std::vector<std::vector<float>> minDist(numTypes, std::vector<float>(numTypes, 40.0f));
    std::vector<std::vector<float>> forces(numTypes, std::vector<float>(numTypes, 0.0f));
    std::vector<std::vector<float>> radii(numTypes, std::vector<float>(numTypes, 150.0f));

    // Grid
    int gridSize = 40;
    int gridWidth = SCR_WIDTH / gridSize;
    int gridHeight = SCR_HEIGHT / gridSize;

    std::vector<std::vector<std::vector<Particle *>>> grid(
        gridHeight,
        std::vector<std::vector<Particle *>>(
            gridWidth,
            std::vector<Particle *>()));
    // Create particles
    for (int i = 0; i < numParticles; i++)
    {
        int type = glm::linearRand(0, numTypes - 1);
        Particle p = Particle(getRandomPosition(0, SCR_WIDTH, 0, SCR_HEIGHT), type);

        auto shape = std::make_shared<Circle2D>(2.0f, 6);
        shape->SetPosition(p.pos);
        shape->SetColor(getColor(numTypes, p.type));

        scene.AddShape(shape);
        particleShapes.push_back(shape);
        particles.push_back(p);

        grid[int(p.pos.y / gridSize)][int(p.pos.x / gridSize)].push_back(&p);
    }

    for (int i = 0; i < numTypes; i++)
    {
        for (int j = 0; j < numTypes; j++)
        {
            forces[i][j] = getValueBetween(0.3f, 1.0f);
            if (getValueBetween(0.0f, 100.0f) < 50.0f)
                forces[i][j] *= -1;

            minDist[i][j] = getValueBetween(30.0f, 50.0f);
            radii[i][j] = getValueBetween(70.0f, 250.0f);
        }
    }
    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        // Start frame timing
        frameStartTime = std::chrono::high_resolution_clock::now();

        // Calculate FPS
        currentTime = glfwGetTime();
        frameTime = currentTime - lastTime;
        lastTime = currentTime;

        frameCount++;

        // Update FPS every 0.25 seconds
        if (currentTime - lastFpsUpdateTime >= 0.25f)
        {
            fps = frameCount / (currentTime - lastFpsUpdateTime);
            frameCount = 0;
            lastFpsUpdateTime = currentTime;
        }

        // Process input
        processInput(window);

        for (int i = 0; i < particles.size(); i++)
        {
            particles[i].update(grid, gridSize, gridHeight, gridWidth, minDist, forces, radii, SCR_WIDTH, SCR_HEIGHT);
            particleShapes[i]->SetPosition(particles[i].pos);
        }

        // Then, clear and rebuild the grid with updated positions
        for (int i = 0; i < gridHeight; i++)
        {
            for (int j = 0; j < gridWidth; j++)
            {
                grid[i][j].clear();
            }
        }

        for (auto &particle : particles)
        {
            int x = particle.pos.x / gridSize;
            int y = particle.pos.y / gridSize;

            if (x >= 0 && x < gridWidth && y >= 0 && y < gridHeight)
            {
                grid[y][x].push_back(&particle);
            }
        }

        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Clear the screen
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw ImGui shape selector
        // ImGui::Begin("Shape Selector");
        // if (ImGui::Button("Circle"))
        //     editor.SelectShape(circle, "Circle");
        // ImGui::End();

        // Draw transform editor
        // editor.DrawImGuiControls();
        // editor.DrawInteractionMatrix(minDist, numTypes, getColor, "Min Dist", 60.f, 100.f);
        editor.DrawInteractionMatrix(forces, numTypes, getColor, "Forces", -1.f, 1.f);
        // editor.DrawInteractionMatrix(radii, numTypes, getColor, "Radii", 140.f, 500.f);

        // Stats window
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiCond_FirstUseEver);
        ImGui::Begin("Performance Stats");
        ImGui::Text("FPS: %.1f", fps);
        ImGui::Text("Frame Time: %.2f ms", frameTime * 1000.0f);
        if (ImGui::Button("Randomize"))
        {
            for (int i = 0; i < numTypes; i++)
            {
                for (int j = 0; j < numTypes; j++)
                {
                    forces[i][j] = getValueBetween(0.3f, 1.0f);
                    if (getValueBetween(0.0f, 100.0f) < 50.0f)
                        forces[i][j] *= -1;

                    minDist[i][j] = getValueBetween(30.0f, 50.0f);
                    radii[i][j] = getValueBetween(70.0f, 250.0f);
                }
            }
        }
        ImGui::End();

        // In your render loop, replace the projection matrix setup
        auto shader = scene.m_defaultShader;
        shader->Activate();

        GLuint viewLoc = glGetUniformLocation(shader->ID, "view");
        GLuint projLoc = glGetUniformLocation(shader->ID, "projection");
        glm::mat4 view = glm::mat4(1.0f);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(camera.GetProjectionMatrix()));

        // Draw the scene
        scene.Draw();

        // Render ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();

        // End frame timing
        frameEndTime = std::chrono::high_resolution_clock::now();
    }

    // Cleanup ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Terminate GLFW
    glfwTerminate();

    return 0;
}

// Process keyboard input
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// Handle window resize
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);

    // Get the camera from user pointer
    Camera2D *camera = static_cast<Camera2D *>(glfwGetWindowUserPointer(window));
    if (camera)
    {
        camera->UpdateAspectRatio((float)width, (float)height);
    }
}

glm::vec2 getRandomPosition(float minX, float maxX, float minY, float maxY)
{
    std::random_device rd;
    std::uniform_real_distribution<float> distX(minX, maxX);
    std::uniform_real_distribution<float> distY(minY, maxY);

    return glm::vec2(distX(rd), distY(rd));
}

int getRandomType(int numTypes)
{
    std::random_device rd;
    std::uniform_int_distribution<int> dist(0, numTypes - 1);

    return dist(rd);
}

glm::vec3 getColor(int numTypes, int typeIndex)
{
    if (numTypes <= 0)
        return glm::vec3(1.0f, 1.0f, 1.0f); // Default white if invalid numTypes

    float hue = fmod((360.0f / numTypes) * typeIndex, 360.0f); // Distribute hues evenly
    float r, g, b;

    int i = int(hue / 60.0f) % 6;
    float f = (hue / 60.0f) - i;
    float q = 1.0f - f;

    switch (i)
    {
    case 0:
        r = 1.0f, g = f, b = 0.0f;
        break;
    case 1:
        r = q, g = 1.0f, b = 0.0f;
        break;
    case 2:
        r = 0.0f, g = 1.0f, b = f;
        break;
    case 3:
        r = 0.0f, g = q, b = 1.0f;
        break;
    case 4:
        r = f, g = 0.0f, b = 1.0f;
        break;
    case 5:
        r = 1.0f, g = 0.0f, b = q;
        break;
    default:
        r = g = b = 1.0f;
        break;
    }

    return glm::vec3(r, g, b);
}