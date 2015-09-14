/*
Copyright (c) 2015 Sean Bohan

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#pragma once

#include "MuNanoVG.h"

#include "perf.h"

#include "MuGlobals.h"
#include "MuEventDispatcher.h"

#include <functional>
#include <vector>

namespace mural {

    class MuAppController;
    class MuCore : public EventDispatcher {
        public:
            static MuCore& getInstance() {
                static MuCore instance;
                return instance;
            }

            bool init();
            void start();
            void setController(MuAppController *ac);

            uint64_t scheduleMessage(std::function<void()> callback, double interval, bool repeat);
            void cancelMessage(uint64_t id);

            void tickAndRender();

            void update();
            void render();

            void terminate();

            int getWindowWidth() const {
                return winWidth;
            }
            int getWindowHeight() const {
                return winHeight;
            }
            Number getRatio() const {
                return ratio;
            }
            NVGcontext *getRenderContext() {
                return renderer;
            }
        private:
            MuCore();
            MuCore(MuCore const&) {}
            void operator=(MuCore const&) {}

            ~MuCore();
        private:
            GLFWwindow *window;
            NVGcontext *renderer;
            MuAppController *appController;

            int winWidth, winHeight;
            int fbWidth, fbHeight;
            Number ratio;

            int defaultFont;

            Number cpuTime, prevt, dt;
            GPUtimer gpuTimer;
            PerfGraph fpsGraph, cpuGraph, gpuGraph;
    };

}

#define app mural::MuCore::getInstance()
