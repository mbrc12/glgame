#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <random>
#include "common.hpp"
#include "engine/mesh.hpp"
#include "engine/shader.hpp"
#include "engine/shapes.hpp"
#include "engine/texture.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

constexpr int WIDTH = 640;
constexpr int HEIGHT = 480;
constexpr const char *GAME_NAME = "GLGame";

GLFWwindow *window;

static auto rng = std::minstd_rand();

void onResize(GLFWwindow *window, int width, int height);
int init();
void cleanup();
void imguiBegin();
void imguiEnd();

int main() {
    std::random_device rd;
    rng.seed(rd());

    if (init() != 0) {
        return -1;
    }

    float bg_color[4] = {0.02, 0.04, 0.06, 1.0};
    glClearColor(bg_color[0], bg_color[1], bg_color[2], bg_color[3]);

    double fps = 0.f;
    float last_time = glfwGetTime();

    Engine::Mesh cube = Engine::cuboidMesh(5.f);
    Engine::Mesh platform = Engine::cuboidMesh(40.f, 2.f, 20.f);
    Engine::Mesh sphere = Engine::sphereMesh(10.f, 50);

    std::uniform_real_distribution<float> uniform(0.f, 1.f);
    auto tex_coords = std::vector<glm::vec2>();
    std::generate_n(std::back_inserter(tex_coords), 8, [&]() {
        float x = uniform(rng);
        float y = uniform(rng);
        return glm::vec2(x, y);
    });

    // cube.setAssociatedData<glm::vec2>(1, tex_coords.data(), tex_coords.size());
    platform.setAssociatedData<glm::vec2>(1, tex_coords.data(), tex_coords.size());

    tex_coords.clear();
    std::generate_n(std::back_inserter(tex_coords), sphere.getVertexCount(), [&]() {
        float x = uniform(rng);
        float y = uniform(rng);
        return glm::vec2(x, y);
    });
    sphere.setAssociatedData<glm::vec2>(1, tex_coords.data(), tex_coords.size());

    Engine::Texture crate_texture("assets/textures/crate-texture.jpg");
    crate_texture.setWrap(Engine::TextureWrap::MirroredRepeat);

    Engine::Texture checkerboard_texture("assets/textures/checkerboard.png");
    checkerboard_texture.setWrap(Engine::TextureWrap::MirroredRepeat);

    Engine::Shader shader;
    shader.build();

    auto perspective = glm::perspective(glm::radians(90.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {

        double time = glfwGetTime();
        fps = fps * 0.9 + 0.1 / (time - last_time);
        last_time = time;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        imguiBegin();

        ImGui::ColorPicker4("Choose bgcolor:", bg_color);
        glClearColor(bg_color[0], bg_color[1], bg_color[2], bg_color[3]);

        ImGui::Text("fps: %.2f", fps);

        imguiEnd();

        shader.use();
    
        auto transform = glm::identity<glm::mat4>();
        transform = glm::translate(transform, glm::vec3(0.0f, 0.0f, -20.f));
        transform = glm::rotate(transform, float(glfwGetTime()), glm::vec3(1.0f, 1.0f, 1.0f));
        shader.setMat4Uniform("transform", perspective * transform);

        crate_texture.bind();
        // cube.draw();
        // transform = glm::scale(transform, glm::vec3(1.f, 1.f, 1.f) * 8.f);
        shader.setMat4Uniform("transform", perspective * transform);
        sphere.draw();

        transform = glm::translate(glm::identity<glm::mat4>(), glm::vec3(0.f, -15.f, -20.f));
        shader.setMat4Uniform("transform", perspective * transform);

        checkerboard_texture.bind();
        platform.draw();

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
    float scale = std::min(1.0 * width/WIDTH, 1.0 * height/HEIGHT);
    auto width_ = static_cast<size_t>(std::round(WIDTH * scale));
    auto height_ = static_cast<size_t>(std::round(HEIGHT * scale));

    auto x_ = (width - width_) / 2;
    auto y_ = (height - height_) / 2;

    glViewport(x_, y_, width_, height_);
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

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

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


