#include "AppController.h"
#include "MuTimer.h"

namespace mural {

  AppController::~AppController() {}

  void AppController::init(int width, int height) {
    theScheduler.scheduleMessage([] {
      printf("timer\n");
    }, 1000, false);
  }

  void AppController::update() {
    theScheduler.update();
  }

  void AppController::draw() {}

}
