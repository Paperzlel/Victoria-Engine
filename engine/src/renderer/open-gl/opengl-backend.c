#include "opengl-backend.h"
#include "opengl-shaders.h"



b8 BackendInitializeGLFW(openGLContext* context, i32 width, i32 height) {
    //Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //Set window context
    context->window = glfwCreateWindow(800, 600, "OpenGL Testing", NULL, NULL);
    if (context->window == NULL)
    {
        VFATAL("Failed to create window!");
        glfwTerminate();
        return FALSE;
    }
    glfwMakeContextCurrent(context->window);
    glfwSetFramebufferSizeCallback(context->window, BackendFramebufferSizeCallback);

    //Load GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        VFATAL("Failed to load GLAD!");
        return FALSE;
    }
    
    //TODO: Separate functions

    //Initialize shader program
    GLShaderInitializeShaderProgram(&context->shaderProgram);

    //TODO: Temporary code. Find a way to use tables to have more than one.
    ShaderObject fragShader;
    //DO THIS!!!!!
    fragShader.type = FRAGMENT_SHADER;
    fragShader.filepath = "C:/Programs/C-and-C++/Victoria-Engine/assets/test_shader.frag.glsl";
    //ShaderObject vertShader;

    //TODO: Look for materials and set up their respective fragment and vertex shaders
    for (u32 i = 0; i < 10; i++) {
        if (i == 1) {
            RegisterNewShaderObject(&fragShader, &context->shaderProgram);
        }
        //if (i == 2) {
        //    RegisterNewShaderObject(&vertShader, &context->shaderProgram);
        //}
    }


    GLLinkShaderProgram(&context->shaderProgram);

    //TODO: Delete shaders
    //glDeleteShader(vertexShader);
    //glDeleteShader(fragmentShader);

    f32 vertices[] = {
     0.5f, 0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     -0.5f,  -0.5f, 0.0f,
     -0.5f, 0.5f, 0.0f
    };
    u32 indicies[] = {
        0, 1, 3, // Triangle 1
        1, 2, 3  // Triangle 2
    };


    glGenVertexArrays(1, &context->VAO);
    glGenBuffers(1, &context->VBO);  
    glGenBuffers(1, &context->EBO);
    glBindVertexArray(context->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, context->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    //EBO buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, context->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    //TODO: temp wireframe view
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    return TRUE;
}

b8 BackendRenderFrame(openGLContext* context, i32 width, i32 height) {
    //TODO: Temporary code. Fix later
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);


    glUseProgram(context->shaderProgram);

    glBindVertexArray(context->VAO);
    //EBO buffer stuff
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(context->window);
    glfwPollEvents();
    return TRUE;
}

b8 BackendShutdownRendering(openGLContext* context) {
    if (glfwWindowShouldClose(context->window)) {
        VERROR("Window already closing!");
        return FALSE;
    }
    glfwSetWindowShouldClose(context->window, TRUE);
    glDeleteVertexArrays(1, &context->VAO);
    glDeleteBuffers(1, &context->VBO);
    glDeleteBuffers(1, &context->EBO);
    glDeleteProgram(context->shaderProgram);
    glfwTerminate();
    VINFO("Backend successfully terminated.");
    return TRUE;
}

void BackendFramebufferSizeCallback(GLFWwindow* window, i32 width, i32 height) {
    // Typecasting just to be safe
    glViewport(0, 0, (int)width, (int)height);
}