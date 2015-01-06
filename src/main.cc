#include <cstdio>

// ONLY include GLEW for non-Apple platforms
#ifndef __APPLE__
#define NANOVG_GLEW
#endif

#ifdef NANOVG_GLEW
#define GLEW_STATIC
#include <GL/glew.h>
#endif

#ifdef __APPLE__
#   define GLFW_INCLUDE_GLCOREARB
#endif
#include <GLFW/glfw3.h>

#include "nanovg/nanovg.h"
#define NANOVG_GL3_IMPLEMENTATION
#include "nanovg/nanovg_gl.h"
#include "nanovg/nanovg_gl_utils.h"

#include "perf.h"


void errorcb(int error, const char *desc) {
    printf("GLFW error %d: %s\n", error, desc);
}

static void key(GLFWwindow *window, int key, int scancode, int action, int mods) {
    NVG_NOTUSED(scancode);
    NVG_NOTUSED(mods);
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

void testApp(NVGcontext *vg) {
    nvgFillColor(vg, nvgRGBA(220, 160, 0, 200));

    nvgBeginPath(vg);
    nvgCircle(vg, 32.0f, 32.0f, 32.0f);
    nvgFill(vg);

    nvgBeginPath(vg);
    nvgCircle(vg, 960.0f - 32.0f, 640.0f - 32.0f, 32.0f);
    nvgFill(vg);
}

int main(int argc, const char *argv[]) {
    GLFWwindow* window;
    NVGcontext* vg = NULL;

    int winWidth, winHeight;
    int fbWidth, fbHeight;
    float pxRatio;

    int defaultFont = -1;
    GPUtimer gpuTimer;
    PerfGraph fps, cpuGraph, gpuGraph;

    double prevt = 0, cpuTime = 0;

    if (!glfwInit()) {
        printf("Failed to init GLFW.");
        return -1;
    }

    initGraph(&fps, GRAPH_RENDER_FPS, "Frame Time");
    initGraph(&cpuGraph, GRAPH_RENDER_MS, "CPU Time");
    initGraph(&gpuGraph, GRAPH_RENDER_MS, "GPU Time");

    glfwSetErrorCallback(errorcb);

    // Disable window resizable
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    // OpenGL version
#ifndef _WIN32 // don't require this on win32, and works with more cards
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
#ifdef DEMO_MSAA
    glfwWindowHint(GLFW_SAMPLES, 4);
#endif

    window = glfwCreateWindow(960, 640, "Mural", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwSetKeyCallback(window, key);

    glfwMakeContextCurrent(window);

#ifdef NANOVG_GLEW
    if (glewInit() != GLEW_OK) {
        printf("Could not init glew.\n");
        return -1;
    }
    // GLEW generates GL error because it calls glGetString(GL_EXTENSIONS), we'll consume it here.
    glGetError();
#endif

// TODO: Set MSAA from script or enable by default
#ifdef MURAL_MSAA
    vg = nvgCreateGL3(NVG_DEBUG);
#else
    vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_DEBUG);
#endif
    if (vg == NULL) {
        printf("Could not init nanovg.\n");
        return -1;
    }

    // Create hi-dpi FBO for hi-dpi screens.
    glfwGetWindowSize(window, &winWidth, &winHeight);
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    // Calculate pixel ration for hi-dpi devices.
    pxRatio = (float)fbWidth / (float)winWidth;

    // Load default font for performance rendering
    defaultFont = nvgCreateFont(vg, "sans", "assets/Roboto-Regular.ttf");

    initGPUTimer(&gpuTimer);

    // Start timer
    glfwSetTime(0);
    prevt = glfwGetTime();

    // Run loop
    while (!glfwWindowShouldClose(window)) {
        double mx, my, t, dt;

        float gpuTimes[3];
        int i, n;

        t = glfwGetTime();
        dt = t - prevt;
        prevt = t;

        startGPUTimer(&gpuTimer);

        glfwGetCursorPos(window, &mx, &my);
        glfwGetWindowSize(window, &winWidth, &winHeight);
        glfwGetFramebufferSize(window, &fbWidth, &fbHeight);

        // Calculate pixel ration for hi-dpi devices.
        pxRatio = (float)fbWidth / (float)winWidth;

        // Setup viewport
        glViewport(0, 0, fbWidth, fbHeight);

        // Clear screen
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // Draw frame
        nvgBeginFrame(vg, winWidth, winHeight, pxRatio);
            testApp(vg);
            // Performance graph
            renderGraph(vg, 5,5, &fps);
            renderGraph(vg, 5 + 200 + 5, 5, &cpuGraph);
            if (gpuTimer.supported) {
                renderGraph(vg, 5 + 200 + 5 + 200 + 5, 5, &gpuGraph);
            }
        nvgEndFrame(vg);

        // Measure the CPU time taken excluding swap buffers
        // (as the swap may wait for GPU)
        cpuTime = glfwGetTime() - t;

        updateGraph(&fps, dt);
        updateGraph(&cpuGraph, cpuTime);

        // We may get multiple results.
        n = stopGPUTimer(&gpuTimer, gpuTimes, 3);
        for (i = 0; i < n; i++) {
            updateGraph(&gpuGraph, gpuTimes[i]);
        }

        // Swap frame
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    nvgDeleteGL3(vg);
    glfwTerminate();

    return 0;
}
