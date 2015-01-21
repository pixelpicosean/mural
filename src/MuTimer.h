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

#include <map>
#include <vector>
#include <functional>

namespace mural {

    class MuTimer {
        public:
            bool active;

            MuTimer(std::function<void()> callback, double interval, bool repeat);
            ~MuTimer();

            void check();

        private:
            std::function<void()> callback;
            double interval, target;
            bool repeat;
    };

    class MuTimerCollection {
        public:
            MuTimerCollection();
            ~MuTimerCollection();

            uint64_t scheduleMessage(std::function<void()> callback, double interval, bool repeat);
            void cancelMessage(uint64_t id);
            void update();

            static MuTimerCollection &defaultCollection() {
                static MuTimerCollection instance;
                return instance;
            }

        private:
            // TODO: make this thread safe
            std::map<uint64_t, MuTimer> timerListA;
            std::map<uint64_t, MuTimer> timerListB;
            std::vector<MuTimer> timersToBeRemoved;
            int currListIdx;
            uint64_t lastId;
    };

}

#define theScheduler mural::MuTimerCollection::defaultCollection()
