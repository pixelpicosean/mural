#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "MuCanvas.hpp"
#include "MuCanvasManager.hpp"
#include "MuCanvasContext2D.hpp"
#include "MuImage.hpp"
#include "MuTimer.hpp"

using namespace ci;
using namespace ci::app;
using namespace std;

using namespace mural;

class MuralApp : public App {
  public:
    void setup() override;
    void mouseDown(MouseEvent event) override;
    void update() override;
    void draw() override;
  protected:
    std::shared_ptr<mural::MuCanvas> canvas;
    bool finished = false;
};

void MuralApp::setup() {
  canvas = std::make_shared<mural::MuCanvas>();
}

void MuralApp::mouseDown(MouseEvent event) {}

void MuralApp::update() {
  theScheduler.update();
}

void MuralApp::draw() {
  // - TEST BEGIN -----------------------------------------------------
  if (!finished) {
    auto ctx = canvas->getContext(mural::kMuCanvasContextMode2D);

    // Tests
    // - LineCap
    auto testLineCap = [&]() {
      ctx->setStrokeColor(ColorAf(0.0f, 1.0f, 1.0f, 1.0f));
      ctx->state->lineWidth = 40;

      // - Butt
      ctx->state->lineCap = mural::kMuLineCapButt;

      ctx->moveTo(200, 100);
      ctx->lineTo(640 - 200, 100);
      ctx->stroke();

      // - Squire
      ctx->state->lineCap = mural::kMuLineCapSquare;

      ctx->moveTo(200, 200);
      ctx->lineTo(640 - 200, 200);
      ctx->stroke();

      // - Round
      ctx->state->lineCap = mural::kMuLineCapRound;

      ctx->moveTo(200, 300);
      ctx->lineTo(640 - 200, 300);
      ctx->stroke();
    };
    // - LineJoin
    auto testLineJoin = [&]() {
      ctx->setStrokeColor(ColorAf(0.0f, 1.0f, 1.0f, 1.0f));
      ctx->state->lineWidth = 40;

      MuLineJoin lineJoin[] = {
        kMuLineJoinRound,
        kMuLineJoinBevel,
        kMuLineJoinMiter,
      };

      for (int i = 0; i < 3; i++) {
        ctx->state->lineJoin = lineJoin[i];
        ctx->beginPath();
        ctx->moveTo(80, 30 + i * 120);
        ctx->lineTo(200, 130 + i * 120);
        ctx->lineTo(320, 30 + i * 120);
        ctx->lineTo(440, 130 + i * 120);
        ctx->lineTo(560, 30 + i * 120);
        ctx->stroke();
      }
    };

    // - Image
    auto testImage = [&] {
      auto img = new MuImage();

      img->onLoad([&](MuImage *img) {
        auto tex = img->getTexture();
        console() << "Image loaded, size: (" << tex->getWidth() << ", " << tex->getHeight() << ")" << std::endl;
      });

      img->setSrc("green-block.png");
    };

    // - drawImage
    auto testImageDrawing = [&] {
      auto img = new MuImage();

      img->onLoad([&](MuImage *img) {
        auto tex = img->getTexture();
        auto sw = tex->getWidth();
        auto sh = tex->getHeight();
        console() << "texture size: " << sw << ", " << sh << std::endl;
        ctx->drawImage(tex, 100, 100);
        ctx->drawImage(tex, 100, 200, 128, 64);
        ctx->drawImage(tex, 32, 16, 32, 16, 100, 300, 64, 32);
      });

      img->setSrc("green-block.png");
    };

    // - Non-block timer
    auto testTimer = [&] {
      auto canceledTimer = theScheduler.setTimeout([&] {
        console() << "You won't see this line!\n";
      }, 2000);

      theScheduler.setTimeout([&, canceledTimer] {
        console() << "Timer [" << canceledTimer << "] is canceled now.\n";
        theScheduler.clearTimeout(canceledTimer);
      }, 1000);

      auto looped = theScheduler.setInterval([&] {
        console() << "<Looped> 600ms\n";
      }, 600);

      theScheduler.setTimeout([&, looped] {
        theScheduler.clearInterval(looped);
        console() << "Looped timer [" << looped << "] is cancled now, it should have logged 4 times.\n";
      }, 2500);

      console() << "Canceled timer id: " << canceledTimer << '\n'
                << "Looped timer:  " << looped << '\n'
                << std::endl;
    };

    // testLineCap();
    // testLineJoin();
    // testImage();
    // testImageDrawing();
    testTimer();

    finished = true;
  }
  // - TEST END --------------------------------------------------------

  mural::theCanvasManager.drawScreenCanvas();
}

CINDER_APP(MuralApp, RendererGl, [&](App::Settings *settings) {
  settings->setWindowSize(640, 400);
  settings->setResizable(false);
  settings->setTitle("Mural");
})
