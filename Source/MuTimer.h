#ifndef Mural_MuTimer_h
#define Mural_MuTimer_h

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

#endif
