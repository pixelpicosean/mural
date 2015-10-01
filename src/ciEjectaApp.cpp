#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "MuCanvasContext2D.hpp"

using namespace ci;
using namespace ci::app;
using namespace std;

class ciEjectaApp : public App {
  public:
    void setup() override;
    void mouseDown( MouseEvent event ) override;
    void update() override;
    void draw() override;
  protected:
    std::shared_ptr<mural::MuCanvasContext2D> ctx;
    bool finished = false;
};

void ciEjectaApp::setup() {
  ctx = std::make_shared<mural::MuCanvasContext2D>(getWindowWidth(), getWindowHeight());
}

void ciEjectaApp::mouseDown(MouseEvent event) {
}

void ciEjectaApp::update() {
  if (!finished) {
    // Create context first
    ctx->create();

    // Draw something here
    ctx->prepare();
    ctx->rect(10, 10, 40, 40);
    ctx->setFillColor(ColorAf{ 0.0f, 1.0f, 1.0f, 1.0f });
    ctx->fill();

    finished = true;
  }
}

void ciEjectaApp::draw() {
	gl::clear(Color(0, 0, 0));

  gl::draw(ctx->getTexture(), getWindowBounds());
}

CINDER_APP(ciEjectaApp, RendererGl)
