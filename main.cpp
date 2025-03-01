#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <memory>
#include <chrono>

// Include our new classes
#include "triangle.h"
#include "rectangle.h"
#include "circle.h"
#include "polygon.h"
#include "scene.h"
#include "transformEditor.h"
#include "camera.h"

// Window dimensions
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;

// Function declarations
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

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

    // Create some shapes
    auto triangle = scene.CreateShape<Triangle2D>(0.3f);
    triangle->SetPosition(glm::vec2(-0.5f, 0.0f));
    triangle->SetColor(glm::vec3(1.0f, 0.0f, 0.0f)); // Red

    auto square = scene.CreateShape<Rectangle2D>(0.3f, 0.3f);
    square->SetPosition(glm::vec2(0.0f, 0.0f));
    square->SetColor(glm::vec3(0.0f, 1.0f, 0.0f)); // Green

    auto circle = scene.CreateShape<Circle2D>(0.15f, 32);
    circle->SetPosition(glm::vec2(0.5f, 0.0f));
    circle->SetColor(glm::vec3(0.0f, 0.0f, 1.0f)); // Blue

    // Create a custom polygon (pentagon)
    std::vector<glm::vec2> pentagonPoints = {
        glm::vec2(0.0f, 0.2f),     // Top
        glm::vec2(0.19f, 0.06f),   // Top right
        glm::vec2(0.12f, -0.16f),  // Bottom right
        glm::vec2(-0.12f, -0.16f), // Bottom left
        glm::vec2(-0.19f, 0.06f)   // Top left
    };
    auto polygon = scene.CreateShape<Polygon2D>(pentagonPoints);
    polygon->SetPosition(glm::vec2(0.0f, -0.5f));
    polygon->SetColor(glm::vec3(1.0f, 1.0f, 0.0f)); // Yellow

    // Setup transform editor
    TransformEditor editor;
    editor.SelectShape(triangle, "Triangle");

    // Setup viewport
    int viewport_width, viewport_height;
    glfwGetFramebufferSize(window, &viewport_width, &viewport_height);
    glViewport(0, 0, viewport_width, viewport_height);

    // Setup projection matrix
    glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);

    // Variables for FPS and render time calculation
    int frameCount = 0;
    float fps = 0.0f;
    float frameTime = 0.0f;
    float lastTime = 0.0f;
    float currentTime = 0.0f;
    float lastFpsUpdateTime = 0.0f;
    std::chrono::high_resolution_clock::time_point frameStartTime;
    std::chrono::high_resolution_clock::time_point frameEndTime;
    float renderTime = 0.0f;

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

        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Clear the screen
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw ImGui shape selector
        ImGui::Begin("Shape Selector");
        if (ImGui::Button("Triangle"))
            editor.SelectShape(triangle, "Triangle");
        if (ImGui::Button("Square"))
            editor.SelectShape(square, "Square");
        if (ImGui::Button("Circle"))
            editor.SelectShape(circle, "Circle");
        if (ImGui::Button("Polygon"))
            editor.SelectShape(polygon, "Polygon");
        ImGui::End();

        // Draw transform editor
        editor.DrawImGuiControls();

        // Stats window
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiCond_FirstUseEver);
        ImGui::Begin("Performance Stats");
        ImGui::Text("FPS: %.1f", fps);
        ImGui::Text("Frame Time: %.2f ms", frameTime * 1000.0f);
        ImGui::Text("Render Time: %.2f ms", renderTime * 1000.0f);
        ImGui::End();

        // Update and draw the scene
        // Set the view and projection matrix in the default shader
        Camera2D camera;
        camera.UpdateAspectRatio(viewport_width, viewport_height);
        glfwSetWindowUserPointer(window, &camera);

        // In your render loop, replace the projection matrix setup
        auto shader = triangle->GetShader();
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
        renderTime = std::chrono::duration<float, std::chrono::seconds::period>(frameEndTime - frameStartTime).count();
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