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
    std::shared_ptr<MuGameManager> game;
};

void MuralApp::setup() {
  game = std::make_shared<TestApp>();
}

void MuralApp::mouseDown(MouseEvent event) {}

void MuralApp::mouseMove(MouseEvent event) {}

void MuralApp::mouseUp(MouseEvent event) {}

void MuralApp::keyDown(KeyEvent event) {}

void MuralApp::keyUp(KeyEvent event) {}

void MuralApp::update() {}

void MuralApp::draw() {
  // Initialize GameManager once
  if (!started) {
    game->init();
    started = true;
  }

  theScheduler.update();
  theCanvasManager.drawScreenCanvas();
}

CINDER_APP(MuralApp, RendererGl, [&](App::Settings *settings) {
  settings->setWindowSize(640, 400);
  settings->setResizable(false);
  settings->setTitle("Mural");
})
