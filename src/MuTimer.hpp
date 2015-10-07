//
//  MuTimer.hpp
//  Mural
//
//  Created by Sean on 10/6/15.
//
//

#pragma once

#include <functional>
#include <queue>
#include <map>
#include <chrono>
#include <sys/time.h>

namespace mural {

  class MuTimer {
    protected:
      struct TimeEvent {
        typedef std::chrono::time_point<std::chrono::system_clock> Time;

        TimeEvent(const std::function<void()> &cb, unsigned int duration, bool loop):
          callback(cb),
          loop(loop)
        {
          id = TimeEvent::baseID++;
          setDuration(std::chrono::milliseconds(duration));
        }

        void setDuration(std::chrono::milliseconds newDuration) {
          finished = false;
          duration = newDuration;
          when = std::chrono::system_clock::now() + newDuration;
        }

        std::function<void()> callback;
        std::chrono::milliseconds duration;
        Time                  when;
        bool                  loop = false;
        bool                  finished = false;

        unsigned int          id = -1;

        static unsigned int baseID;
      };

      typedef std::shared_ptr<TimeEvent> TimeEventRef;

      struct event_less : public std::less<TimeEventRef> {
        bool operator()(const TimeEventRef &e1, const TimeEventRef &e2) const {
          return (e2->when < e1->when);
        }
      };

      // callback, canceled
      typedef std::pair<std::function<void()>, bool> RAFEvent;
      typedef std::shared_ptr<RAFEvent> RAFEventRef;

    public:
      // Browser-like timer functions
      unsigned int setTimeout(const std::function<void()> &cb, unsigned int ms);
      void clearTimeout(unsigned int id);

      unsigned int setInterval(const std::function<void()> &cb, unsigned int ms);
      void clearInterval(unsigned int id);

      unsigned int requestAnimationFrame(const std::function<void()> &cb);
      void cancelAnimationFrame(unsigned int id);

      // Direct timer functions
      unsigned int addTimer(const std::function<void()> &cb, unsigned int ms, bool loop);
      void removeTimer(unsigned int id);

      // Internal functions which will be called by app
      void update();
      void animate();

      static MuTimer &instance() {
        static MuTimer inst;
        return inst;
      }

    protected:
      bool usingQueueA = true;

      std::priority_queue<TimeEventRef, std::vector<TimeEventRef>, event_less> timerQueueA;
      std::priority_queue<TimeEventRef, std::vector<TimeEventRef>, event_less> timerQueueB;

      std::map<unsigned int, TimeEventRef> timerHash;

      // requestAnimationFrame timers
      std::deque<RAFEventRef> rafCallbackQueue;
      std::map<unsigned int, RAFEventRef> rafCallbackHash;

    private:
      MuTimer() {}
      ~MuTimer() {}
      MuTimer(MuTimer const &) {}
      void operator=(MuTimer const &) {}
  };

  #define theScheduler MuTimer::instance()

}
