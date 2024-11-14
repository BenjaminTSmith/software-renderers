#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>
#include <fstream>

constexpr int width = 800;
constexpr int height = 450;

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // TODO(Ben): make viewport an integer scaling of the width and height
}

static std::string read_file(const std::string& filepath) {
    std::ifstream filestream(filepath);
    if (!filestream.is_open()) {
        std::cout << filepath << " failed to open" << std::endl;
    }
    std::string file = "";
    std::string line;
    while (std::getline(filestream, line)) {
        file += line + '\n';   
    }
    filestream.close();
    return file;
}

struct Color {
    unsigned char r;
    unsigned char g;
    unsigned char b;

    Color() : r(0), g(0), b(0) {}
    Color(unsigned char r, unsigned char g, unsigned char b) : r(r), g(g), b(b) {}
};

int main() {
    GLFWwindow* window;

    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return 0;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(width, height, "Software Renderer", NULL, NULL);
    if (!window) {
        std::cout << "Failed to create window" << std::endl;
        glfwTerminate();
        return 0;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return 0;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glViewport(0, 0, width, height);

    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Set up screen quad
    // NOTE(Ben): texture coordinates are flipped so that the first index into the framebuffer is
    // the top left corner.
    float vertices[] = {
        -1.f, 1.f, 0.f, 0.f, // top left
        1.f, 1.f, 1.f, 0.f, // top right
        -1.f, -1.f, 0.f, 1.f, // bottom left

        1.f, 1.f, 1.f, 0.f, // top right
        1.f, -1.f, 1.f, 1.f, // bottom right
        -1.f, -1.f, 0.f, 1.f // bottom left
    };

    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Initialize and compile shaders
    std::string vertex_source = read_file("shaders/vertex.glsl");
    const char* c_vertex_source = vertex_source.c_str();
    unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &c_vertex_source, nullptr);
    glCompileShader(vertex_shader);

    int  success;
    char info_log[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
        std::cout << "error: vertex shader failed: " << info_log << std::endl;
    }

    std::string fragment_source = read_file("shaders/fragment.glsl");
    const char* c_fragment_source = fragment_source.c_str();
    unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &c_fragment_source, nullptr);
    glCompileShader(fragment_shader);

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
        std::cout << "error: fragment shader failed: " << info_log << std::endl;
    }

    unsigned int shader = glCreateProgram();
    glAttachShader(shader, vertex_shader);
    glAttachShader(shader, fragment_shader);
    glLinkProgram(shader);
    glUseProgram(shader);
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader, 512, NULL, info_log);
    }

    Color* framebuffer = new Color[width * height];
    for (int i = 0; i < width * 3; i++) {
        framebuffer[i].r = 255;
    }

    for (int i = 0; i < height; i++) {
        framebuffer[i * width + 32].g = 255;
    }
    unsigned int texture;
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height,
                 0, GL_RGB, GL_UNSIGNED_BYTE, framebuffer);
    // Don't know if needed yet
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glClearColor(0, 0, 0, 0);
    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }
        glClear(GL_COLOR_BUFFER_BIT);

        // TODO(Ben): Possibly change to an index buffer if need be
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    glfwTerminate();

    delete[] framebuffer;
    return 0;
}
