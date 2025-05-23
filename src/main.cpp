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
constexpr const char* GAME_NAME = "GLGame";
float HORIZONTAL_SENSITIVITY = 0.0005f;
float VERTICAL_SENSITIVITY = 0.0005f;
float camera_exponent = 1.f;

GLFWwindow* window;

static auto rng = std::minstd_rand();

void onResize(GLFWwindow* window, int width, int height);
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
    Engine::Mesh platform = Engine::cuboidMesh(100.f, 3.f, 100.f);
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

    Engine::Texture checkerboard("assets/textures/checkerboard.png");
    checkerboard.setWrap(Engine::TextureWrap::MirroredRepeat);

    Engine::Shader shader;
    shader.build();

    auto camera = glm::identity<glm::mat4>();

    static float rot_y = 0.f;
    static float rot_x = 0.f;
    static bool move = true;
    static bool first_motion = true;

    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double x, double y) {
        static double last_x = 0.f;
        static double last_y = 0.f;

        float dx = (float)(x - last_x);
        float dy = (float)(y - last_y);

        last_x = x;
        last_y = y;

        if (!move) {
            ImGui::GetIO().MousePos = ImVec2(x, y);
            return;
        }

        if (first_motion) {
            first_motion = false;
            return;
        }

        float new_rot_x = rot_x + dy * VERTICAL_SENSITIVITY;
        float new_rot_y = rot_y + dx * HORIZONTAL_SENSITIVITY;

        if (std::pow(std::abs(new_rot_x), camera_exponent) + std::pow(std::abs(new_rot_y), camera_exponent) >
            std::pow(PI / 2, camera_exponent)) {
            return;
        }

        rot_x = new_rot_x;
        rot_y = new_rot_y;
    });

    auto projection = [&]() {
        static glm::vec3 camera_position = {0.f, 50.f, 70.f};

        camera = glm::rotate(glm::identity<glm::mat4>(), rot_y, glm::vec3(0.f, 1.f, 0.f));
        camera = glm::rotate(camera, rot_x, glm::vec3(1.f, 0.f, 0.f));

        static auto perspective = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 1000.0f);
        return perspective * glm::inverse(camera) * glm::translate(glm::identity<glm::mat4>(), -camera_position);
    };

    bool last_frame = false;

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
        ImGui::SliderFloat("HORIZONTAL_SENSITIVITY", &HORIZONTAL_SENSITIVITY, 0.f, 0.001f, "%.5f");
        ImGui::SliderFloat("VERTICAL_SENSITIVITY", &VERTICAL_SENSITIVITY, 0.f, 0.001f, "%.5f");
        ImGui::SliderFloat("camera_exponent", &camera_exponent, 0.1f, 5.f, "%.2f");

        imguiEnd();

        shader.use();

        auto transform = glm::identity<glm::mat4>();
        transform = glm::translate(transform, glm::vec3(0.f, 20.f, 0.f));
        transform = glm::rotate(transform, float(glfwGetTime()), glm::vec3(1.0f, 1.0f, 1.0f));
        shader.setMat4Uniform("transform", projection() * transform);

        crate_texture.bind();
        // cube.draw();
        // transform = glm::scale(transform, glm::vec3(1.f, 1.f, 1.f) * 8.f);
        shader.setMat4Uniform("transform", projection() * transform);
        sphere.draw();

        transform = glm::identity<glm::mat4>();
        shader.setMat4Uniform("transform", projection() * transform);

        checkerboard.bind();
        platform.draw();

        glfwSwapBuffers(window);

        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && !last_frame) {
            move = !move;
            if (move == false)
                first_motion = true;
        }

        last_frame = glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS;
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

void onResize(GLFWwindow* window, int width = -1, int height = -1) {
    if (width == -1 || height == -1) {
        glfwGetWindowSize(window, &width, &height);
    }

    float scale = std::min(1.0 * width / WIDTH, 1.0 * height / HEIGHT);
    auto width_ = static_cast<size_t>(std::round(WIDTH * scale));
    auto height_ = static_cast<size_t>(std::round(HEIGHT * scale));

    auto x_ = (width - width_) / 2;
    auto y_ = (height - height_) / 2;

    // DBG("scale: " << scale);

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

    auto monitor = glfwGetPrimaryMonitor();

    const auto mode = glfwGetVideoMode(monitor);
    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(mode->width, mode->height, GAME_NAME, monitor, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    DBG("window created: " << mode->width << "x" << mode->height);

    glfwSetFramebufferSizeCallback(window, onResize);

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        DBG("failed to initialize glad");
        return -1;
    }

    onResize(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    assert(glfwRawMouseMotionSupported());
    glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.MouseDrawCursor = true;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    DBG("vendor: " << glGetString(GL_VENDOR) << "\nversion: " << glGetString(GL_VERSION) << "\nshader: "
                   << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\nrenderer: " << glGetString(GL_RENDERER)
                   << "\nimgui: " << IMGUI_VERSION << "\nglfw: " << glfwGetVersionString());

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
