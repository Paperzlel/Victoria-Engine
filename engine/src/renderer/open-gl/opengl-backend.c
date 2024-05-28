#include "opengl-backend.h"

b8 BackendInitializeGLFW(openGLContext* context, i32 width, i32 height) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    context->window = glfwCreateWindow(800, 600, "OpenGL Testing", NULL, NULL);
    if (context->window == NULL)
    {
        VFATAL("Failed to create window!");
        glfwTerminate();
        return FALSE;
    }
    glfwMakeContextCurrent(context->window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        VFATAL("Failed to load GLAD!");
        return FALSE;
    }
    glViewport(0, 0, 800, 600);
    return TRUE;
}

b8 BackendRenderFrame(GLFWwindow* window, i32 width, i32 height) {
    //TODO: Add method to find if the renderer is active
    glfwSwapBuffers(window);
    glfwPollEvents();
    return TRUE;
}

b8 BackendShutdownRendering(GLFWwindow* window) {
    if (glfwWindowShouldClose(window)) {
        VERROR("Window already closing!");
        return FALSE;
    }
    glfwSetWindowShouldClose(window, TRUE);
    glfwTerminate();
    return TRUE;
}