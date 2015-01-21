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

#include "MuTimer.h"
#include "GLFW/glfw3.h"

#include <utility>

namespace mural {

    double performanceNow() {
        return glfwGetTime() * 1000.0;
    }

    double dateWithTimeIntervalSinceNow(double interval) {
        return performanceNow() + interval;
    }

    double timeIntervalSinceNow(double target) {
        return target - performanceNow();
    }

    MuTimer::MuTimer(std::function<void()> callback, double interval, bool repeat):
        active(true),
        callback(callback),
        interval(interval),
        target(dateWithTimeIntervalSinceNow(interval)),
        repeat(repeat)
    {}

    MuTimer::~MuTimer() {}

    void MuTimer::check() {
        if (active && timeIntervalSinceNow(target) < 0.0) {
            if (callback) {
                callback();
            }

            if (repeat) {
                target = dateWithTimeIntervalSinceNow(interval);
            }
            else {
                active = false;
            }
        }
    }

    MuTimerCollection::MuTimerCollection():
        currListIdx(0),
        lastId(0)
    {}

    MuTimerCollection::~MuTimerCollection() {
        timerListA.clear();
        timerListB.clear();
    }

    uint64_t MuTimerCollection::scheduleMessage(std::function<void()> callback, double interval, bool repeat) {
        lastId++;

        MuTimer timer(callback, interval, repeat);

        if (currListIdx == 0) {
            timerListA.emplace(lastId, timer);
        }
        else if (currListIdx == 1) {
            timerListB.emplace(lastId, timer);
        }

        return lastId;
    }

    void MuTimerCollection::cancelMessage(uint64_t id) {
        auto it = timerListA.find(id);
        if (it != timerListA.end()) {
            timersToBeRemoved.push_back(std::move(it->second));
            timerListA.erase(it);
        }
        else {
            it = timerListB.find(id);
            if (it != timerListB.end()) {
                timersToBeRemoved.push_back(std::move(it->second));
                timerListB.erase(it);
            }
        }
    }

    void MuTimerCollection::update() {
        timersToBeRemoved.clear();

        if (currListIdx == 0) {
            currListIdx = 1;
            for (auto it = timerListA.begin(); it != timerListA.end(); ++it) {
                it->second.check();
            }
        }
        else if (currListIdx == 1) {
            currListIdx = 0;
            for (auto it = timerListB.begin(); it != timerListB.end(); ++it) {
                it->second.check();
            }
        }
    }

}
