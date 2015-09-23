#include <cstdio>

#include "MuOpenGL.h"
#include "MuNanoVG.h"
#include "AppController.h"

// TODO: set width and height from script instead
const int WIDTH = 640;
const int HEIGHT = 400;

int main(int argc, char const *argv[]) {
  if (!glfwInit()) {
    printf("Failed to initialize GLFW\n");
    return -1;
  }

  // Require OpenGL 3.3
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Mural", nullptr, nullptr);
  if (!window) {
    printf("Failed to create window\n");
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);

  glewExperimental = GL_TRUE;
  if (glewInit() != GLEW_OK) {
    printf("Failed to initialize GLEW\n");
    glfwTerminate();
    return -1;
  }

  // Create hi-dpi FBO for hi-dpi screens.
  int winWidth, winHeight;
  int fbWidth, fbHeight;
  glfwGetWindowSize(window, &winWidth, &winHeight);
  glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
  // Calculate pixel ration for hi-dpi devices.
  int ratio = (int)(fbWidth / (float)winWidth);

  NVGcontext *ctx = nvg::createGLContext();
  app.init(winWidth, winHeight, ratio, ctx);

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    app.update();

    glfwSwapBuffers(window);
  }

  nvg::deleteGLContext(ctx);

  glfwTerminate();

  return 0;
}
