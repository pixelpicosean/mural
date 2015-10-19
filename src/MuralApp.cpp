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

    auto testComplexCurves = [&] {
      auto w = 640;
      auto h = 400;
      auto w2 = w / 2;
      auto h2 = h / 2;

      int maxCurves = 70;

      struct Curve {
        float current = 0.0f;
        float inc = 0.0f;
        Color color = { rand() * 1.0f / RAND_MAX, rand() * 1.0f / RAND_MAX, rand() * 1.0f / RAND_MAX };
        Curve(float curr = 0.0f, float i = 0.0f): current(curr), inc(i) {}
      };

      std::vector<Curve*> curves;
      for (auto i = 0; i < 70; i++) {
        curves.push_back(new Curve {
          (rand() * 1.0f / RAND_MAX) * 1000.0f,           // current
          (rand() * 1.0f / RAND_MAX) * 0.005f + 0.002f    // inc
        });
      }

      //ctx->setGlobalCompositeOperation("lighter");
      ctx->setLineWidth(2.0f);
      ctx->setGlobalAlpha(0.5f);

      ctx->setFillColor(ColorAf::hex(0x000000));

      auto anim = [=] {
        ctx->clearRect(0.0f, 0.0f, getWindowWidth(), getWindowHeight());

        ctx->setGlobalCompositeOperation("source-over");
        ctx->fillRect(0, 0, canvas->getWidth(), canvas->getHeight());
        // ctx->setGlobalAlpha(0.5f);

        float p[8] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
        for (auto i = 0; i < maxCurves; i++ ) {
          auto curve = curves[i];
          curve->current += curve->inc;
          for (auto j = 0; j < 4; j += 2) {
            auto a = sinf(curve->current * (j + 3) * 373 * 0.0001f);
            auto b = sinf(curve->current * (j + 5) * 927 * 0.0002f);
            auto c = sinf(curve->current * (j + 5) * 573 * 0.0001f);
            p[j] = (a * a * b + c * a + b) * w * c + w2;
            p[j + 1] = (a * b * b + c - a * b *c) * h2 + h2;
          }

          ctx->beginPath();
          ctx->moveTo(p[0], p[1]);
          ctx->bezierCurveTo(p[2], p[3], p[4], p[5], p[6], p[7]);
          ctx->setStrokeColor(curve->color);
          ctx->stroke();
        }
      };

      theScheduler.setInterval(anim, 1000 / 60.0f);
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

    auto testMSAA = [&] {
      auto c0 = std::make_shared<MuCanvas>(320, 400);
      c0->msaaEnabled = false;
      auto ctx0 = c0->getContext(kMuCanvasContextMode2D);

      ctx0->setStrokeColor(ColorAf(1.0f, 1.0f, 0.0f, 1.0f));
      ctx0->setLineWidth(20);
      ctx0->setLineCap("round");

      ctx0->beginPath();
      ctx0->moveTo(120.5, 130);
      ctx0->quadraticCurveTo(150.8, 130, 160.6, 150.5);
      ctx0->quadraticCurveTo(190, 250.0, 210.5, 160.5);
      ctx0->quadraticCurveTo(240, 100.5, 290, 70.5);

      ctx0->stroke();

      auto c16 = std::make_shared<MuCanvas>(320, 400);
      c16->msaaEnabled = true;
      c16->msaaSamples = 16;
      auto ctx16 = c16->getContext(kMuCanvasContextMode2D);

      ctx16->setStrokeColor(ColorAf(1.0f, 1.0f, 0.0f, 1.0f));
      ctx16->setLineWidth(20);
      ctx16->setLineCap("round");

      ctx16->beginPath();
      ctx16->moveTo(120.5, 130);
      ctx16->quadraticCurveTo(150.8, 130, 160.6, 150.5);
      ctx16->quadraticCurveTo(190, 250.0, 210.5, 160.5);
      ctx16->quadraticCurveTo(240, 100.5, 290, 70.5);

      ctx16->stroke();

      ctx->drawImage(ctx0->getTexture(), 0, 0);
      ctx->drawImage(ctx16->getTexture(), 320, 0);
    };

    auto pong = [=] {
      auto imgBall = new MuImage();
      auto imgPaddle = new MuImage();

      auto start = [=] {
        auto texBall = imgBall->getTexture();
        auto texPaddle = imgPaddle->getTexture();

        float paddleOffset = 40;

        ctx->save();
        ctx->translate(canvas->getWidth() * 0.5f, canvas->getHeight() * 0.5f);
        ctx->drawImage(texBall, -texBall->getWidth() * 0.5f, -texBall->getHeight() * 0.5f);
        ctx->restore();

        ctx->save();
        ctx->translate(paddleOffset, canvas->getHeight() * 0.5f);
        ctx->rotate(M_PI_2);
        ctx->drawImage(texPaddle, -texPaddle->getWidth() * 0.5f, -texPaddle->getHeight() * 0.5f);
        ctx->restore();

        ctx->save();
        ctx->translate(canvas->getWidth() - paddleOffset, canvas->getHeight() * 0.5f);
        ctx->rotate(-M_PI_2);
        ctx->drawImage(texPaddle, -texPaddle->getWidth() * 0.5f, -texPaddle->getHeight() * 0.5f);
        ctx->restore();
      };

      auto onload = [=](MuImage *img) {
        static int count = 0;
        count++;

        console() << "load " << count << std::endl;

        if (count == 2) {
          start();
        }
      };

      imgBall->on("load", onload);
      imgBall->setSrc("ball.png");

      imgPaddle->on("load", onload);
      imgPaddle->setSrc("paddle.png");
    };

    auto testTransform = [&] {
      auto imgPaddle = new MuImage();
      imgPaddle->on("load", [&](MuImage *img) {
        auto tex = img->getTexture();

        float offset = 160.0f;

        // Translate
        ctx->save();
        ctx->translate(canvas->getWidth() * 0.5f - offset, canvas->getHeight() * 0.5f);
        ctx->drawImage(tex, -tex->getWidth() * 0.5f, -tex->getHeight() * 0.5f);
        ctx->restore();

        // Rotate
        ctx->save();
        ctx->translate(canvas->getWidth() * 0.5f, canvas->getHeight() * 0.5f);
        ctx->rotate(M_PI * 0.2f);
        ctx->drawImage(tex, -tex->getWidth() * 0.5f, -tex->getHeight() * 0.5f);
        ctx->restore();

        // Scale
        ctx->save();
        ctx->translate(canvas->getWidth() * 0.5f + offset, canvas->getHeight() * 0.5f);
        ctx->scale(1.5f, 1.5f);
        ctx->drawImage(tex, -tex->getWidth() * 0.5f, -tex->getHeight() * 0.5f);
        ctx->restore();
      });
      imgPaddle->setSrc("paddle.png");
    };

    auto testImageData = [&] {
      ctx->setFillColor("teal");
      ctx->fillRect(20, 20, 20, 20);

      auto imgData = ctx->getImageData(20, 20, 40, 40);
      ctx->putImageData(imgData, canvas->getWidth() * 0.5 + 20, 20);
    };

    auto testDrawText = [&] {
      ctx->setFont(Font("Verdana", 48.0f));
      ctx->setFillColor("pink");
      ctx->setTextAlign("center");
      ctx->fillText("It Works!", canvas->getWidth() * 0.5f, canvas->getHeight() * 0.5f);

      vec2 m = ctx->measureText("IT Works!");
      console() << "text measure result: " << m << std::endl;
    };

    // testLineCap();
    // testLineJoin();
    // testImage();
    // testImageDrawing();
    // testTimer();
    // testCurves();
    // testComplexCurves();
    // testPatternFill();
    // testTextureContext();
    // testGlobalCompositeOperation();
    // testMSAA();
    // pong();
    // testTransform();
    // testImageData();
    testDrawText();

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
