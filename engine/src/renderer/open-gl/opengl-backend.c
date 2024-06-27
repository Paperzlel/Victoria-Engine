#include "opengl-backend.h"
#include "opengl-shaders.h"
#include "opengl-buffers.h"
#include "opengl-arrays.h"
#include "opengl-textures.h"

#include "core/vmemory.h"

static OpenGLContext context;

b8 GLBackendInitialize(RendererBackend* backend, i32 width, i32 height, string applicationName) {
    //Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    //Set window context
    context.window = glfwCreateWindow(width, height, applicationName, NULL, NULL);
    if (context.window == NULL) {
        VFATAL("Failed to create GLFW window.");
        glfwTerminate();
        return FALSE;
    }
    glfwMakeContextCurrent(context.window);
    glfwSetFramebufferSizeCallback(context.window, GLBackendFramebufferSizeCallback);

    //Load GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        VFATAL("Failed to load GLAD.");
        return FALSE;
    }

    /* Shader program initialization */
    if (!GLInitializeShaderProgram(&context.shaderProgram)) {
       VFATAL("Failed to initialise shader program.");
       return FALSE;
    }

    // Shader setup
    Shader myShader;
    myShader.name = "test_shader";

    //TODO: Uniform lists

    if (!GLRegisterNewShader(&myShader, &context.shaderProgram)) {
        VERROR("Shader failed to be initialized."); // TODO: Find a way to ensure the program can continue without problems 
        return FALSE;
    }
    glLinkProgram(context.shaderProgram);
    GLDeleteShaders(&myShader);

    float vertices[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
    };

    unsigned int indices[] = {  
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    //VAO creation and binding (functions overshadow the default for now)
    GLGenerateArrayObject(&context.vertexArrayObj);
    GLBindVertexArray(context.vertexArrayObj);

    /* VBO initialization and binding */
    if (!RegisterNewBufferObject(&context.vertexBufferObj, ARRAY_BUF)) {
        VFATAL("VBO could not be registered.");
        return FALSE;
    }

    glBufferData(ARRAY_BUF, sizeof(vertices), vertices, STATIC_DRAW);

    if (!RegisterNewBufferObject(&context.elementBufferObj, ELEMENT_ARRAY_BUF)) {
        VFATAL("EBO could not be registered.");
        return FALSE;
    }

    glBufferData(ELEMENT_ARRAY_BUF, sizeof(indices), indices, STATIC_DRAW);


    /* Function info:
    * First parameter is the layout location for the vertex shader (i.e. the layout (location = 0)) part
    * Second pararm is the size of the vertex attribute (it's a vec3 so 3)
    * Third param is the type of data (vec3 is float data)
    * Fourth param is if the data needs to be normalized (if it's a int or byte)
    * Fifth param is the stride or the size of each vertex attribute
    * Sixth param is a void* to the offset of the data (if we have colour data after each vertex it'd be the offset from that)
    */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(f32), (void*)0);
    // Enable the layout in the location above
    glEnableVertexAttribArray(0);

    //Same thing 2 more times
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(f32), (void*)(3 * sizeof(f32)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(f32), (void*)(6 * sizeof(f32)));
    glEnableVertexAttribArray(2);

    // Texture system
    if (!GenerateTexture(&context.texture, TEXTURE_2D)) {
        VERROR("Texture was unable to be initialized.");
        return FALSE;
    }

    return TRUE;
}

i8 GLBackendRenderFrame(RendererBackend* backend) {
    f32 startFrame = glfwGetTime();

    // Input processing
    GLProcessInput(context.window);

    // Check if the application was requested to shut down
    if (context.windowShouldClose) {
        return 0;
    }

    // Create background colour
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Texture bindings
    glBindTexture(TEXTURE_2D, context.texture.texID);

    //TODO: Temp code. Remove later
    mat4 translation;
    vec3 scaleVec = {0.5f, 0.5f, 0.5f};
    translation = Mat4EulerZ(90.0f * DEG_TO_RAD_MULTIPLIER * glfwGetTime());
    translation = Mat4Mul(translation, Mat4ScaleMat(scaleVec));

    // Uniform detection
    glUseProgram(context.shaderProgram);
    u32 transformLoc = glGetUniformLocation(context.shaderProgram, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, translation.data);
    
    // Draw objects
    glBindVertexArray(context.vertexArrayObj);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(context.window);
    glfwPollEvents();
    f32 endFrame = glfwGetTime();

    //Check to see if the window should close
    context.windowShouldClose = glfwWindowShouldClose(context.window);
    context.deltaTime = endFrame - startFrame;
    return 1;
}

void GLBackendShutdownRendering(RendererBackend* backend) {
    glfwSetWindowShouldClose(context.window, TRUE);
    glDeleteVertexArrays(1, &context.vertexArrayObj);

    DeleteBufferObject(&context.vertexBufferObj);

    glDeleteProgram(context.shaderProgram);
    glfwTerminate();
    VINFO("Backend successfully terminated.");
}

void GLBackendFramebufferSizeCallback(GLFWwindow* window, i32 width, i32 height) {
    // Typecasting just to be safe
    glViewport(0, 0, (int)width, (int)height);
}

void GLProcessInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, TRUE);
    }
}