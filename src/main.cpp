#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tiny_gltf.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/gtx/transform.hpp>

#include <iostream>
#include <filesystem>
#include <cassert>

#include "graphics_shader.hpp"
#include "camera.hpp"

GLFWwindow* window;
constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 1024;

std::vector<float> positions, normals, texCoords;
std::vector<unsigned short> indices;

bool LoadGLTFModel() {
    const auto cwd = std::filesystem::current_path();
    const auto path_model = cwd / "assets" / "models" / "monkey.glb";
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    //bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, path_model.c_str());
    bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, path_model.c_str());

    if (!warn.empty()) {
        printf("Warn: %s\n", warn.c_str());
    }
    
    if (!err.empty()) {
        throw std::runtime_error("Err: " + err + "\n");
    }

    if (!ret) {
        printf("Failed to parse glTF\n");
        return EXIT_FAILURE;
    }

    for (const auto& node : model.nodes) {
        if (node.mesh < 0) continue;

        const tinygltf::Mesh& mesh = model.meshes[node.mesh];

        for (const auto& primitive : mesh.primitives) {
            if (primitive.mode != TINYGLTF_MODE_TRIANGLES) continue;

            if (primitive.attributes.find("POSITION") != primitive.attributes.end()) {
                const tinygltf::Accessor& accessor = model.accessors[primitive.attributes.at("POSITION")];
                const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
                const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

                if (accessor.type == TINYGLTF_TYPE_VEC3 && accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
                    const float* data = reinterpret_cast<const float*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);
                    positions.insert(positions.end(), data, data + accessor.count * 3);
                }
            }

            if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
                const tinygltf::Accessor& accessor = model.accessors[primitive.attributes.at("NORMAL")];
                const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
                const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

                if (accessor.type == TINYGLTF_TYPE_VEC3 && accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
                    const float* data = reinterpret_cast<const float*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);
                    normals.insert(normals.end(), data, data + accessor.count * 3);
                }
            }

            if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
                const tinygltf::Accessor& accessor = model.accessors[primitive.attributes.at("TEXCOORD_0")];
                const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
                const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

                if (accessor.type == TINYGLTF_TYPE_VEC2 && accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
                    const float* data = reinterpret_cast<const float*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);
                    texCoords.insert(texCoords.end(), data, data + accessor.count * 2);
                }
            }

            if (primitive.indices >= 0) {
                const tinygltf::Accessor& accessor = model.accessors[primitive.indices];
                const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
                const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

                size_t indexSize = tinygltf::GetComponentSizeInBytes(accessor.componentType);
                const unsigned char* dataPtr = buffer.data.data() + bufferView.byteOffset + accessor.byteOffset;

                for (size_t i = 0; i < accessor.count; i++) {
                    if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
                        indices.push_back(*reinterpret_cast<const unsigned short*>(dataPtr + i * indexSize));
                    } else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
                        indices.push_back(*reinterpret_cast<const unsigned int*>(dataPtr + i * indexSize));
                    }
                }
            }
        }
    }

    return true;
}

GLuint VAO, VBO, NBO, TBO, EBO;

void SetupGLBuffers() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &NBO);
    glGenBuffers(1, &TBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), positions.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, NBO);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, TBO);
    glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(float), texCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char *message, const void *userParam);

int main()
{
    if (!glfwInit())
        throw std::runtime_error("Failed to initialize GLFW");

    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "glTF-viewer", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        throw std::runtime_error("Failed to create window");
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwDestroyWindow(window);
        glfwTerminate();
        throw std::runtime_error("Failed to initialize GLAD.\n");
    }

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(glDebugOutput, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    Camera camera{};
    camera.origin = { 0.0f, 0.0f, 5.0f };
	camera.vertical_fov = 45.0f;
	camera.aspect_ratio = static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT);
	camera.near_plane = 0.001f;
	camera.far_plane = 1000.0f;
    camera.movement_speed = 5.0f;
	camera.update_projection_matrix();
    glfwSetWindowUserPointer(window, &camera);

    GraphicsShader shader("default.vert", "default.frag");
    shader.use();

    glClearColor(0.25f, 0.25f, 0.25f, 1.0f);

    if (!LoadGLTFModel()) {
        std::cerr << "Failed to load model!" << std::endl;
        return EXIT_FAILURE;
    }

    SetupGLBuffers();

    auto model_matrix = glm::mat4(1.0f);

    float deltatime{ 0.0f };
    float last_frame{ 0.0f };
    while (!glfwWindowShouldClose(window))
    {
        const float current_frame = static_cast<float>(glfwGetTime());
		deltatime = current_frame - last_frame;
		last_frame = current_frame;

        const auto P = camera.projection_matrix;
        const auto V = camera.get_view_matrix();
        const auto PV = P * V;

        shader.use();
        shader.set_mat4("u_PV", PV);
        shader.set_mat4("u_ModelMatrix", model_matrix);
        shader.set_vec3("u_CameraPosition", camera.origin);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, 0);
        
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.move(FORWARD, deltatime);
    
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.move(BACKWARD, deltatime);
    
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.move(LEFT, deltatime);
    
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.move(RIGHT, deltatime);
    
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            camera.move(UP, deltatime);
    
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            camera.move(DOWN, deltatime);
    
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return EXIT_SUCCESS;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
    camera->mouse_callback(xpos, ypos);
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    double cursor_x, cursor_y;
	glfwGetCursorPos(window, &cursor_x, &cursor_y);

    Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
	camera->mouse_button_callback(cursor_x, cursor_y, button, action, mods);
}

void APIENTRY glDebugOutput(GLenum source,
                            GLenum type,
                            unsigned int id,
                            GLenum severity,
                            GLsizei length,
                            const char *message,
                            const void *userParam)
{
    // ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
        return;

    std::cout << "---------------\n";
    std::cout << "Debug message (" << id << "): " << message << "\n";

    switch (source)
    {
    case GL_DEBUG_SOURCE_API:
        std::cout << "Source: API";
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        std::cout << "Source: Window System";
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        std::cout << "Source: Shader Compiler";
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        std::cout << "Source: Third Party";
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        std::cout << "Source: Application";
        break;
    case GL_DEBUG_SOURCE_OTHER:
        std::cout << "Source: Other";
        break;
    }
    std::cout << "\n";

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:
        std::cout << "Type: Error";
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        std::cout << "Type: Deprecated Behaviour";
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        std::cout << "Type: Undefined Behaviour";
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        std::cout << "Type: Portability";
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        std::cout << "Type: Performance";
        break;
    case GL_DEBUG_TYPE_MARKER:
        std::cout << "Type: Marker";
        break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        std::cout << "Type: Push Group";
        break;
    case GL_DEBUG_TYPE_POP_GROUP:
        std::cout << "Type: Pop Group";
        break;
    case GL_DEBUG_TYPE_OTHER:
        std::cout << "Type: Other";
        break;
    }
    std::cout << "\n";

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:
        std::cout << "Severity: high";
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        std::cout << "Severity: medium";
        break;
    case GL_DEBUG_SEVERITY_LOW:
        std::cout << "Severity: low";
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        std::cout << "Severity: notification";
        break;
    }
    std::cout << "\n";
    std::cout << std::endl;
}