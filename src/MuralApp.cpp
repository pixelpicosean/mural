#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "Game/TestApp.hpp"

using namespace ci;
using namespace ci::app;
using namespace std;

using namespace mural;

class MuralApp : public App {
  public:
    void setup() override;

    void mouseDown(MouseEvent event) override;
    void mouseMove(MouseEvent event) override;
    void mouseUp(MouseEvent event) override;

    void keyDown(KeyEvent event) override;
    void keyUp(KeyEvent event) override;

    void update() override;
    void draw() override;

  protected:
    bool started = false;
    std::shared_ptr<MuGameController> game;
};

void MuralApp::setup() {
  game = std::make_shared<TestApp>();
}

void MuralApp::mouseDown(MouseEvent event) {
  game->mouseDown(event);
}

void MuralApp::mouseMove(MouseEvent event) {
  game->mouseMove(event);
}

void MuralApp::mouseUp(MouseEvent event) {
  game->mouseUp(event);
}

void MuralApp::keyDown(KeyEvent event) {
  game->keyDown(event);
}

void MuralApp::keyUp(KeyEvent event) {
  game->keyUp(event);
}

void MuralApp::update() {}

void MuralApp::draw() {
  // Initialize GameManager once
  if (!started) {
    game->init();
    started = true;
  }

  theScheduler.update();
  game->update();
  game->draw();
  theCanvasManager.drawScreenCanvas();
}

CINDER_APP(MuralApp, RendererGl, [&](App::Settings *settings) {
  settings->setWindowSize(640, 400);
  settings->setResizable(false);
  settings->setTitle("Mural");
})
