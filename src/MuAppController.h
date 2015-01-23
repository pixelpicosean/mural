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

#include "MuGlobals.h"
#include "MuEventHandler.h"
#include "nanovg/nanovg.h"

namespace mural {

    class MuAppController : public EventHandler {
        public:
            MuAppController() {}
            virtual ~MuAppController() {}

            /**
             * Setup controller.
             * ONLY system setup things should go here.
             */
            virtual void setup(NVGcontext *renderer) {
                this->g = renderer;
            }
            /**
             * Initialize before start.
             * Everything is ready to use now.
             */
            virtual void init() {}
            virtual void update(Number dt) {}
            virtual void render() {}
        protected:
            /**
             * NanoVG context for drawing
             */
            NVGcontext *g;
    };

}
