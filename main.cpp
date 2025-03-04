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
#include <thread>
#include <mutex>
#include <atomic>

#include "triangle.h"
#include "rectangle.h"
#include "circle.h"
#include "polygon.h"
#include "scene.h"
#include "transformEditor.h"
#include "camera.h"
#include "particle.h"

// Window dimensions
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;

// Function declarations
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window, int numTypes, std::vector<std::vector<float>> &minDist,
                  std::vector<std::vector<float>> &forces,
                  std::vector<std::vector<float>> &radii);
glm::vec2 getRandomPosition(float minX, float maxX, float minY, float maxY);
glm::vec3 getColor(int numTypes, int typeIndex);

// Thread worker function to update particles
void updateParticlesBatch(
    std::vector<Particle> &particles,
    std::vector<std::shared_ptr<Shape2D>> &particleShapes,
    std::vector<std::vector<std::vector<Particle *>>> &grid,
    int gridSize, int gridHeight, int gridWidth,
    std::vector<std::vector<float>> &minDist,
    std::vector<std::vector<float>> &forces,
    std::vector<std::vector<float>> &radii,
    int startIdx, int endIdx,
    float screenWidth, float screenHeight);

// Thread worker function to rebuild grid
void rebuildGridBatch(
    std::vector<Particle> &particles,
    std::vector<std::vector<std::vector<Particle *>>> &grid,
    int gridSize, int gridHeight, int gridWidth,
    int startIdx, int endIdx,
    std::mutex &gridMutex);

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

    // Setup transform editor
    TransformEditor editor;

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

    int numParticles = 1800;
    std::vector<Particle> particles;
    particles.reserve(numParticles);

    int numTypes = 7;
    std::vector<std::shared_ptr<Shape2D>> particleShapes;

    // Initialize interaction matrices with meaningful default values
    std::vector<std::vector<float>> minDist(numTypes, std::vector<float>(numTypes, 40.0f));
    std::vector<std::vector<float>> forces(numTypes, std::vector<float>(numTypes, 0.0f));
    std::vector<std::vector<float>> radii(numTypes, std::vector<float>(numTypes, 150.0f));

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

    // Grid
    int gridSize = 50;
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
        int type = int(getValueBetween(0, numTypes));
        Particle p = Particle(getRandomPosition(0, SCR_WIDTH, 0, SCR_HEIGHT), type);

        auto shape = std::make_shared<Circle2D>(2.5f, 4);
        shape->SetPosition(p.pos);
        shape->SetColor(getColor(numTypes, p.type));

        scene.AddShape(shape);
        particleShapes.push_back(shape);
        particles.push_back(p);

        grid[int(p.pos.y / gridSize)][int(p.pos.x / gridSize)].push_back(&p);
    }

    // Threading configuration
    const unsigned int numThreads = std::thread::hardware_concurrency();

    // Control for multithreading
    bool enableMultithreading = true;

    // Mutex for grid access
    std::mutex gridMutex;

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
        processInput(window, numTypes, minDist, forces, radii);

        if (enableMultithreading)
        {
            // Multithreaded particle update
            std::vector<std::thread> threads;
            const int particlesPerThread = numParticles / numThreads;

            // Update particles in parallel
            for (unsigned int t = 0; t < numThreads; t++)
            {
                int startIdx = t * particlesPerThread;
                int endIdx = (t == numThreads - 1) ? numParticles : (t + 1) * particlesPerThread;

                threads.emplace_back(updateParticlesBatch,
                                     std::ref(particles),
                                     std::ref(particleShapes),
                                     std::ref(grid),
                                     gridSize, gridHeight, gridWidth,
                                     std::ref(minDist),
                                     std::ref(forces),
                                     std::ref(radii),
                                     startIdx, endIdx,
                                     SCR_WIDTH, SCR_HEIGHT);
            }

            // Wait for all update threads to finish
            for (auto &thread : threads)
            {
                thread.join();
            }
            threads.clear();

            // Clear the grid for rebuilding
            for (int i = 0; i < gridHeight; i++)
            {
                for (int j = 0; j < gridWidth; j++)
                {
                    grid[i][j].clear();
                }
            }

            // Rebuild the grid in parallel
            for (unsigned int t = 0; t < numThreads; t++)
            {
                int startIdx = t * particlesPerThread;
                int endIdx = (t == numThreads - 1) ? numParticles : (t + 1) * particlesPerThread;

                threads.emplace_back(rebuildGridBatch,
                                     std::ref(particles),
                                     std::ref(grid),
                                     gridSize, gridHeight, gridWidth,
                                     startIdx, endIdx,
                                     std::ref(gridMutex));
            }

            // Wait for all rebuild threads to finish
            for (auto &thread : threads)
            {
                thread.join();
            }
        }
        else
        {
            // Original single-threaded code
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
        }

        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Clear the screen
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw transform editor
        editor.DrawInteractionMatrix(forces, numTypes, getColor, "Forces", -1.f, 1.f);

        // Stats window
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(250, 150), ImGuiCond_FirstUseEver);
        ImGui::Begin("Performance Stats");
        ImGui::Text("FPS: %.1f", fps);
        ImGui::Text("Frame Time: %.2f ms", frameTime * 1000.0f);
        ImGui::Text("Particles: %d", numParticles);
        ImGui::Checkbox("Enable Multithreading", &enableMultithreading);
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

// Thread worker function to update particles
void updateParticlesBatch(
    std::vector<Particle> &particles,
    std::vector<std::shared_ptr<Shape2D>> &particleShapes,
    std::vector<std::vector<std::vector<Particle *>>> &grid,
    int gridSize, int gridHeight, int gridWidth,
    std::vector<std::vector<float>> &minDist,
    std::vector<std::vector<float>> &forces,
    std::vector<std::vector<float>> &radii,
    int startIdx, int endIdx,
    float screenWidth, float screenHeight)
{
    for (int i = startIdx; i < endIdx; i++)
    {
        particles[i].update(grid, gridSize, gridHeight, gridWidth, minDist, forces, radii, screenWidth, screenHeight);
        particleShapes[i]->SetPosition(particles[i].pos);
    }
}

// Thread worker function to rebuild grid
void rebuildGridBatch(
    std::vector<Particle> &particles,
    std::vector<std::vector<std::vector<Particle *>>> &grid,
    int gridSize, int gridHeight, int gridWidth,
    int startIdx, int endIdx,
    std::mutex &gridMutex)
{
    for (int i = startIdx; i < endIdx; i++)
    {
        Particle &particle = particles[i];
        int x = particle.pos.x / gridSize;
        int y = particle.pos.y / gridSize;

        // Handle wrapping
        x = (x + gridWidth) % gridWidth;
        y = (y + gridHeight) % gridHeight;

        if (x >= 0 && x < gridWidth && y >= 0 && y < gridHeight)
        {
            // Lock the mutex when updating the grid
            std::lock_guard<std::mutex> lock(gridMutex);
            grid[y][x].push_back(&particle);
        }
    }
}

// Process keyboard input
void processInput(GLFWwindow *window, int numTypes, std::vector<std::vector<float>> &minDist,
                  std::vector<std::vector<float>> &forces,
                  std::vector<std::vector<float>> &radii)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
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