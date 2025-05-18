#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "common.hpp"
#include "engine/shader.hpp"
#include "engine/texture.hpp"

constexpr int WIDTH = 640;
constexpr int HEIGHT = 480;
constexpr const char *GAME_NAME = "GLGame";

GLFWwindow *window;

void onResize(GLFWwindow *window, int width, int height);
int init();
void cleanup();
void imguiBegin();
void imguiEnd();

int main() {
    if (init() != 0) {
        return -1;
    }

    float bg_color[4] = {0.1, 0.4, 0.2, 1.0};
    glClearColor(bg_color[0], bg_color[1], bg_color[2], bg_color[3]);

    double fps = 0.f;
    float last_time = glfwGetTime();

    float vertices[] = {
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.0f, 2.0f, 0.0f,
        -0.5f,  0.5f, 0.0f, 0.0f, 2.0f,
        0.5f, 0.5f, 0.0f, 2.0f, 2.0f,
    };

    uint VBO;
    glGenBuffers(1, &VBO);

    uint EBO;
    glGenBuffers(1, &EBO);

    uint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    int indices[] = {
        0, 1, 2,
        1, 3, 2,
    };

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    Engine::Texture texture("assets/textures/crate-texture.jpg");
    texture.setWrap(Engine::TextureWrap::MirroredRepeat);

    Engine::Shader shader;
    shader.build();

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {

        double time = glfwGetTime();
        fps = fps * 0.9 + 0.1 / (time - last_time);
        last_time = time;

        glClear(GL_COLOR_BUFFER_BIT);

        imguiBegin();

        ImGui::ColorPicker4("Choose bgcolor:", bg_color);
        glClearColor(bg_color[0], bg_color[1], bg_color[2], bg_color[3]);

        ImGui::Text("fps: %.2f", fps);

        imguiEnd();

        glBindVertexArray(VAO);
        shader.use();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);

        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
    }

    cleanup();
    return 0;
}

void imguiBegin() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void imguiEnd() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void onResize(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    DBG("window resized to " << width << "x" << height);
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    DBG("framebuffer resized to " << w << "x" << h);
}

int init() {
    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(WIDTH, HEIGHT, GAME_NAME, NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, onResize);

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        DBG("failed to initialize glad");
        return -1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    DBG("vendor: " << glGetString(GL_VENDOR) << "\nversion: " << glGetString(GL_VERSION) << 
        "\nshader: "
                   << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\nrenderer: " << glGetString(GL_RENDERER)
                   << "\nimgui: " << IMGUI_VERSION << "\nglfw: " << glfwGetVersionString());

    glViewport(0, 0, WIDTH, HEIGHT);

    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    return 0;
}


void cleanup() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}


