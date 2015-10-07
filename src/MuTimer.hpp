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

    public:
      unsigned int setTimeout(const std::function<void()> &cb, unsigned int ms);
      void clearTimeout(unsigned int id);

      unsigned int setInterval(const std::function<void()> &cb, unsigned int ms);
      void clearInterval(unsigned int id);

      unsigned int addTimer(const std::function<void()> &cb, unsigned int ms, bool loop);
      void removeTimer(unsigned int id);

      void update();

      static MuTimer &instance() {
        static MuTimer inst;
        return inst;
      }

    protected:
      bool usingQueueA = true;

      std::priority_queue<TimeEventRef, std::vector<TimeEventRef>, event_less> timerQueueA;
      std::priority_queue<TimeEventRef, std::vector<TimeEventRef>, event_less> timerQueueB;

      std::map<unsigned int, TimeEventRef> timerHash;

    private:
      MuTimer() {}
      ~MuTimer() {}
      MuTimer(MuTimer const &) {}
      void operator=(MuTimer const &) {}
  };

  #define theScheduler MuTimer::instance()

}
