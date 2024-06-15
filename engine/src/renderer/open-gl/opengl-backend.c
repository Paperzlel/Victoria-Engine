#include "opengl-backend.h"
#include "opengl-shaders.h"
#include "opengl-buffers.h"
#include "opengl-arrays.h"


b8 BackendInitializeGLFW(OpenGLContext* context, i32 width, i32 height) {
    //Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    //Set window context
    context->window = glfwCreateWindow(800, 600, "Victoria Engine", NULL, NULL);
    if (context->window == NULL)
    {
        VFATAL("Failed to create GLFW window.");
        glfwTerminate();
        return FALSE;
    }
    glfwMakeContextCurrent(context->window);
    glfwSetFramebufferSizeCallback(context->window, BackendFramebufferSizeCallback);

    //Load GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        VFATAL("Failed to load GLAD.");
        return FALSE;
    }

    /* Shader program initialization */
    if (!GLInitializeShaderProgram(&context->shaderProgram)) {
       VFATAL("Failed to initialise shader program.");
       return FALSE;
    }

    // Shader setup
    Shader myShader;
    myShader.name = "test_shader";

    //TODO: Uniform lists

    if (!GLRegisterNewShader(&myShader, &context->shaderProgram)) {
        VFATAL("Shader failed to be initialized.");
        return FALSE;
    }
    glLinkProgram(context->shaderProgram);
    GLDeleteShaders(&myShader);

    // Buffer object setup
    float vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
    };  

    //VAO creation and binding (functions overshadow the default for now)
    GLGenerateArrayObject(&context->vertexArrayObj);
    GLBindVertexArray(context->vertexArrayObj);

    /* VBO initialization and binding */
    if (!RegisterNewBufferObject(&context->vertexBufferObj, ARRAY_BUF)) {
        VFATAL("VBO could not be registered.");
        return FALSE;
    }
    glBufferData(ARRAY_BUF, sizeof(vertices), vertices, STATIC_DRAW);

    /* Function info:
    * First parameter is the layout location for the vertex shader (i.e. the layout (location = 0)) part
    * Second pararm is the size of the vertex attribute (it's a vec3 so 3)
    * Third param is the type of data (vec3 is float data)
    * Fourth param is if the data needs to be normalized (if it's a int or byte)
    * Fifth param is the stride or the size of each vertex attribute
    * Sixth param is a void* to the offset of the data (if we have colour data after each vertex it'd be the offset from that)
    */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), (void*)0);
    // Enable the layout in the location above
    glEnableVertexAttribArray(0);


    return TRUE;
}

b8 BackendRenderFrame(OpenGLContext* context, i32 width, i32 height) {
    f32 startFrame = glfwGetTime();

    // Input processing
    ProcessInput(context->window);

    // Create background colour
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    //Uniform detection
    glUseProgram(context->shaderProgram);
    f32 greenValue = (vsin(startFrame) / 2.0f) + 0.5f;
    i32 vertexColourLoc = glGetUniformLocation(context->shaderProgram, "ourColour");
    glUniform4f(vertexColourLoc, 0.0f, greenValue, 0.0f, 1.0f);
    
    // Draw objects
    //glUseProgram(context->shaderProgram);
    glBindVertexArray(context->vertexArrayObj);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(context->window);
    glfwPollEvents();
    f32 endFrame = glfwGetTime();

    //Check to see if the window should close
    context->windowShouldClose = glfwWindowShouldClose(context->window);
    context->deltaTime = endFrame - startFrame;
    return TRUE;
}

b8 BackendShutdownRendering(OpenGLContext* context) {
    // In the case that the application failed to load our window, this happens.
    if (context->window == 0) {
        VERROR("Context doesn't exist! Booting out...");
        return FALSE;
    }
    
    glfwSetWindowShouldClose(context->window, TRUE);
    glDeleteVertexArrays(1, &context->vertexArrayObj);
    glDeleteBuffers(1, &context->vertexBufferObj);
    glDeleteProgram(context->shaderProgram);
    glfwTerminate();
    VINFO("Backend successfully terminated.");
    return TRUE;
}

void BackendFramebufferSizeCallback(GLFWwindow* window, i32 width, i32 height) {
    // Typecasting just to be safe
    glViewport(0, 0, (int)width, (int)height);
}

void ProcessInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, TRUE);
    }
}