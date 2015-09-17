#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cstdio>

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

  app.init(WIDTH, HEIGHT);

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    app.update();
    app.draw();

    glfwSwapBuffers(window);
  }

  glfwTerminate();

  return 0;
}
