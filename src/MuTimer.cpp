//
//  MuTimer.cpp
//  Mural
//
//  Created by Sean on 10/6/15.
//
//

#include "MuTimer.hpp"

namespace mural {

  unsigned int MuTimer::TimeEvent::baseID = 0;

  unsigned int MuTimer::setTimeout(const std::function<void ()> &cb, unsigned int ms) {
    return addTimer(cb, ms, false);
  }

  void MuTimer::clearTimeout(unsigned int id) {
    removeTimer(id);
  }

  unsigned int MuTimer::setInterval(const std::function<void ()> &cb, unsigned int ms) {
    return addTimer(cb, ms, true);
  }

  void MuTimer::clearInterval(unsigned int id) {
    removeTimer(id);
  }

  unsigned int MuTimer::addTimer(const std::function<void ()> &cb, unsigned int ms, bool loop) {
    auto e = std::make_shared<TimeEvent>(cb, ms, loop);

    if (usingQueueA) {
      timerQueueA.push(e);
    }
    else {
      timerQueueB.push(e);
    }

    timerHash[e->id] = e;

    return e->id;
  }

  void MuTimer::removeTimer(unsigned int id) {
    auto e = timerHash.find(id);
    if (e != timerHash.end()) {
      e->second->finished = true;
      timerHash.erase(e);
    }
  }

  void MuTimer::update() {
    TimeEvent::Time now = std::chrono::system_clock::now();

    auto &timerQueue = usingQueueA ? timerQueueA : timerQueueB;
    auto &timerQueue2 = usingQueueA ? timerQueueB : timerQueueA;

    usingQueueA = !usingQueueA;

    while (!timerQueue.empty() && (timerQueue.top()->when < now)) {
      auto t = timerQueue.top();
      timerQueue.pop();

      if (!t->finished) {
        t->callback();

        if (t->loop) {
          t->setDuration(t->duration);
          timerQueue2.push(t);
          continue;
        }
      }

      removeTimer(t->id);
    }
  }

}
