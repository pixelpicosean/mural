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

void MuralApp::update() {}

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

      theScheduler.requestAnimationFrame([&] {
        console() << "requestAnimationFrame" << std::endl;
      });

      console() << "Canceled timer id: " << canceledTimer << '\n'
                << "Looped timer:  " << looped << '\n'
                << std::endl;
    };

    auto testCurves = [&] {
      ctx->setStrokeColor(ColorAf(1.0f, 1.0f, 0.0f, 1.0f));
      ctx->state->lineWidth = 20;
      ctx->state->lineCap = kMuLineCapRound;

      ctx->beginPath();
      ctx->moveTo(120.5, 130);
      ctx->quadraticCurveTo(150.8, 130, 160.6, 150.5);
      ctx->quadraticCurveTo(190, 250.0, 210.5, 160.5);
      ctx->quadraticCurveTo(240, 100.5, 290, 70.5);

      ctx->stroke();
    };

    auto testComplexCurves = [&] {
      std::random_device r;
      std::seed_seq seed{r(),r(),r(),r(),r(),r()};
      std::mt19937 eng{seed};
      std::uniform_int_distribution<> dist(0, 1);

      auto w = 640;
      auto h = 400;
      auto w2 = w / 2;
      auto h2 = h / 2;

      int maxCurves = 70;

      struct Curve {
        float current = 0.0f;
        float inc = 0.0f;
        Curve(float curr = 0.0f, float i = 0.0f): current(curr), inc(i) {}
      };

      Curve curves[70];
      for (auto i = 0; i < 70; i++) {
        curves[i] = {
          dist(eng) * 1000.0f,           // current
          dist(eng) * 0.005f + 0.002f    // inc
        };
      }

      auto anim = [=, &curves] {
        // ctx->clearRect(0.0f, 0.0f, getWindowWidth(), getWindowHeight());

        float p[8] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
        for (auto i = 0; i < maxCurves; i++ ) {
          auto curve = curves[i];
          curve.current += curve.inc;
          for (auto j = 0; j < 4; j += 2) {
            auto a = sinf(curve.current * (j + 3) * 373 * 0.0001f);
            auto b = sinf(curve.current * (j + 5) * 927 * 0.0002f);
            auto c = sinf(curve.current * (j + 5) * 573 * 0.0001f);
            p[j] = (a * a * b + c * a + b) * w * c + w2;
            p[j + 1] = (a * b * b + c - a * b *c) * h2 + h2;
          }

          ctx->prepare();
          ctx->beginPath();
          ctx->moveTo(p[0], p[1]);
          ctx->bezierCurveTo(p[2], p[3], p[4], p[5], p[6], p[7]);
          ctx->setStrokeColor(ColorAf::white());
          ctx->stroke();
        }
      };

      // anim();

      // theScheduler.setTimeout(anim, 400);
      // theScheduler.setTimeout(anim, 800);
      // theScheduler.setTimeout(anim, 1200);
      // theScheduler.setTimeout(anim, 1600);
      // theScheduler.setTimeout(anim, 2400);

      theScheduler.setInterval(anim, 800);
    };

    auto testPatternFill = [=] {
      auto img = new MuImage();

      img->onLoad([&](MuImage *img) {
        auto tex = img->getTexture();

        MuCanvasPattern repeatX(tex, kMuCanvasPatternRepeatX);
        MuCanvasPattern repeatY(tex, kMuCanvasPatternRepeatY);

        MuCanvasPattern noRepeat(tex, kMuCanvasPatternNoRepeat);
        MuCanvasPattern repeatXY(tex, kMuCanvasPatternRepeat);

        ctx->state->fillObject = &noRepeat;
        ctx->fillRect(0, 0, 320, 200);

        ctx->state->fillObject = &repeatX;
        ctx->fillRect(320, 0, 320, 200);

        ctx->state->fillObject = &repeatY;
        ctx->fillRect(0, 200, 320, 200);

        ctx->state->fillObject = &repeatXY;
        ctx->fillRect(320, 200, 320, 200);
      });

      img->setSrc("ball.png");
    };

    // testLineCap();
    // testLineJoin();
    // testImage();
    // testImageDrawing();
    // testTimer();
    // testCurves();
    // testComplexCurves();
    testPatternFill();

    finished = true;
  }
  // - TEST END --------------------------------------------------------

  theScheduler.update();
  theScheduler.animate();
  theCanvasManager.drawScreenCanvas();
}

CINDER_APP(MuralApp, RendererGl, [&](App::Settings *settings) {
  settings->setWindowSize(640, 400);
  settings->setResizable(false);
  settings->setTitle("Mural");
})
