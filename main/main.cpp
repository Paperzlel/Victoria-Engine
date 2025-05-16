#include "main.h"

#include "core/os/display_manager.h"
#include "core/os/os.h"

#include "thirdparty/glad/glad.h"

static bool should_quit = false;

struct GLContext {
    GLuint shader_program;
    GLuint vertex_array;
    GLuint vertex_buffer;
} context;

const char *vertex_shader_source = "#version 460 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\n\0";

const char *fragment_shader_source = "#version 460 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
        "FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";

void Main::set_should_quit(bool p_value) {
    should_quit = p_value;
}

Error Main::setup(int argc, char *argv[]) {
    OS::get_singleton()->initialize();

    return OK;
}

Error Main::start() {
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, nullptr);
    glCompileShader(vertex_shader);

    int success;
    char info_log[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, 512, nullptr, info_log);
        OS::get_singleton()->printerr(info_log);
    }

    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, nullptr);
    glCompileShader(fragment_shader);

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, 512, nullptr, info_log);
        OS::get_singleton()->printerr(info_log);
    }

    context.shader_program = glCreateProgram();
    glAttachShader(context.shader_program, vertex_shader);
    glAttachShader(context.shader_program, fragment_shader);
    glLinkProgram(context.shader_program);

    glGetProgramiv(context.shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(context.shader_program, 512, nullptr, info_log);
        OS::get_singleton()->printerr(info_log);
    }
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    float vertices[9] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f
    };

    glGenVertexArrays(1, &context.vertex_array);
    glGenBuffers(1, &context.vertex_buffer);

    glBindVertexArray(context.vertex_array);
    glBindBuffer(GL_ARRAY_BUFFER, context.vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return OK;
}

bool Main::iteration() {
    glClearColor(0.2, 0.2, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(context.shader_program);
    glBindVertexArray(context.vertex_array);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    DisplayManager::get_singleton()->swap_buffers();

    return should_quit;
}

void Main::finalize() {
    DisplayManager::get_singleton()->finalize();
}
