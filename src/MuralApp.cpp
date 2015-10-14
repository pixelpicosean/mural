#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "MuCanvas.hpp"
#include "MuCanvasManager.hpp"
#include "MuCanvasContext2D.hpp"
#include "MuImage.hpp"
#include "MuTimer.hpp"
#include "MuCanvasPattern.hpp"

#include <random>

using namespace ci;
using namespace ci::app;
using namespace std;

using namespace mural;

int timeCount = 5;
void animate() {
  console() << "raf(" << timeCount << ")\n";
  timeCount -= 1;
  if (timeCount > 0) {
    theScheduler.requestAnimationFrame(animate);
  }
}

class MuralApp : public App {
  public:
    void setup() override;
    void mouseDown(MouseEvent event) override;
    void update() override;
    void draw() override;
  protected:
    std::shared_ptr<mural::MuCanvas> canvas;
    std::shared_ptr<mural::MuCanvas> canvas2;

    bool finished = false;
};

void MuralApp::setup() {
  // Screen canvas
  canvas = std::make_shared<mural::MuCanvas>();

  // Texture canvas
  canvas2 = std::make_shared<mural::MuCanvas>(getWindowWidth(), getWindowHeight());
}

void MuralApp::mouseDown(MouseEvent event) {}

void MuralApp::update() {}

void MuralApp::draw() {
  // - TEST BEGIN -----------------------------------------------------
  if (!finished) {
    auto ctx = canvas->getContext(mural::kMuCanvasContextMode2D);
    auto ctx2 = canvas2->getContext(mural::kMuCanvasContextMode2D);

    // Tests
    // - LineCap
    auto testLineCap = [&]() {
      ctx->setStrokeColor(ColorAf(0.0f, 1.0f, 1.0f, 1.0f));
      ctx->setLineWidth(40);

      // - Butt
      ctx->setLineCap("butt");

      ctx->moveTo(200, 100);
      ctx->lineTo(640 - 200, 100);
      ctx->stroke();

      // - Squire
      ctx->setLineCap("square");

      ctx->moveTo(200, 200);
      ctx->lineTo(640 - 200, 200);
      ctx->stroke();

      // - Round
      ctx->setLineCap("round");

      ctx->moveTo(200, 300);
      ctx->lineTo(640 - 200, 300);
      ctx->stroke();
    };
    // - LineJoin
    auto testLineJoin = [&]() {
      ctx->setStrokeColor(ColorAf(0.0f, 1.0f, 1.0f, 1.0f));
      ctx->setLineWidth(40);

      std::string lineJoin[] = {
        "round",
        "bevel",
        "miter",
      };

      for (int i = 0; i < 3; i++) {
        ctx->setLineJoin(lineJoin[i]);
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

      img->on("load", [&](MuImage *img) {
        auto tex = img->getTexture();
        console() << "Image loaded, size: (" << tex->getWidth() << ", " << tex->getHeight() << ")" << std::endl;
      });

      img->setSrc("green-block.png");

      img = new MuImage();
      img->on("error", [&](MuImage *img) {
        console() << "Failed to load image \"" << img->getSrc() << "\"" << std::endl;
      });
      img->setSrc("not-exist.png");
    };

    // - drawImage
    auto testImageDrawing = [&] {
      auto img = new MuImage();

      img->on("load", [&](MuImage *img) {
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

      theScheduler.requestAnimationFrame(animate);

      console() << "Canceled timer id: " << canceledTimer << '\n'
                << "Looped timer:  " << looped << '\n'
                << std::endl;
    };

    auto testCurves = [&] {
      ctx->setStrokeColor(ColorAf(1.0f, 1.0f, 0.0f, 1.0f));
      ctx->setLineWidth(20);
      ctx->setLineCap("round");

      ctx->beginPath();
      ctx->moveTo(120.5, 130);
      ctx->quadraticCurveTo(150.8, 130, 160.6, 150.5);
      ctx->quadraticCurveTo(190, 250.0, 210.5, 160.5);
      ctx->quadraticCurveTo(240, 100.5, 290, 70.5);

      ctx->stroke();
    };


    auto testPatternFill = [=] {
      auto img = new MuImage();

      img->on("load", [&](MuImage *img) {
        auto tex = img->getTexture();

        MuCanvasPattern repeatX(tex, kMuCanvasPatternRepeatX);
        MuCanvasPattern repeatY(tex, kMuCanvasPatternRepeatY);

        MuCanvasPattern noRepeat(tex, kMuCanvasPatternNoRepeat);
        MuCanvasPattern repeatXY(tex, kMuCanvasPatternRepeat);

        ctx->setFillObject(&noRepeat);
        ctx->fillRect(0, 0, 320, 200);

        ctx->setFillObject(&repeatX);
        ctx->fillRect(320, 0, 320, 200);

        ctx->setFillObject(&repeatY);
        ctx->fillRect(0, 200, 320, 200);

        ctx->setFillObject(&repeatXY);
        ctx->fillRect(320, 200, 320, 200);
      });

      img->setSrc("ball.png");
    };

    auto testTextureContext = [&] {
      // Fill the screen canvas first, so we can see whether something has been drawn to it
      ctx->setFillColor("plum");
      ctx->fillRect(0, 0, canvas->getWidth(), canvas->getHeight());

      // Draw to an offline texture canvas
      ctx2->setFillColor("lightsteelblue");
      ctx2->fillRect(0, 0, 320, 200);

      // Draw the offline texture canvas to screen canvas
      ctx->drawImage(ctx2->getTexture(), 160, 100);

      // Draw something again to the texture canvas
      ctx2->setStrokeColor("cyan");
      ctx2->setLineWidth(10);
      ctx2->setLineJoin("round");
      ctx2->strokeRect(20, 20, 100, 100);

      // And draw texture canvas to screen canvas again
      ctx->drawImage(ctx2->getTexture(), 0, 0, 200, 200);

      // And draw some primitives to screen canvas to see whether it is broken
      ctx->setStrokeColor("black");
      ctx->setFillColor("teal");
      ctx->setLineWidth(20);
      ctx->setLineCap("round");
      ctx->setLineJoin("miter");
      ctx->beginPath();
      ctx->moveTo(200, 20);
      ctx->lineTo(200, 200);
      ctx->lineTo(150, 300);
      ctx->lineTo(100, 100);
      ctx->closePath();
      ctx->fill();
    };

    auto testGlobalCompositeOperation = [&] {
      float x = 32.0f, y = 10.0f, offset = 40.0f, size = 80.0f;

      std::vector<std::vector<std::string> > op = {
        { "source-over",      "lighter",        "darken",     "destination-out",  },
        { "destination-over", "source-atop",    "xor",        "copy",             },
        { "source-in",        "destination-in", "source-out", "destination-atop", },
      };

      int r = 0, q = 0;
      for (auto i = op.begin(); i != op.end(); ++i, ++r) {
        q = 0;
        for (auto j = (*i).begin(); j != (*i).end(); ++j, ++q) {
          float ltX = x + q * 152;
          float ltY = y + 130 * r;

          ctx->setGlobalCompositeOperation(*j);

          ctx->setFillColor("blue");
          ctx->fillRect(ltX, ltY, size, size);

          ctx->setFillColor("red");
          ctx->fillRect(ltX + offset, ltY + offset, size, size);
        }
      }
    };

    // testLineCap();
    // testLineJoin();
    // testImage();
    // testImageDrawing();
    // testTimer();
    // testCurves();
    // testPatternFill();
    // testTextureContext();
    testGlobalCompositeOperation();

    finished = true;
  }
  // - TEST END --------------------------------------------------------

  theScheduler.update();
  theCanvasManager.drawScreenCanvas();
}

CINDER_APP(MuralApp, RendererGl, [&](App::Settings *settings) {
  settings->setWindowSize(640, 400);
  settings->setResizable(false);
  settings->setTitle("Mural");
})
