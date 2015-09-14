#include "../src/MuAppController.h"
#include "../src/MuFbo.h"

#include "../src/MuCore.h"

#include <cstdio>

using namespace mural;

// Create a controller class for testing
class GameController : public mural::MuAppController {
    public:
        GameController() {}
        ~GameController() {
            nvgluDeleteFramebuffer(g, fb);
        }
        void init() {
            int img = nvgCreateImage(g, "assets/shield.png", 0);
            int w, h;
            nvgImageSize(g, img, &w, &h);

            // fbo = MuFbo::create(100, 100, 0);
            // fbo->bindFrameBuffer();
            //     // NVGpaint imgPaint = nvgImagePattern(g, 0, 0, w, h, 0, img, 1.0f);
            //     nvgBeginPath(g);
            //     nvgRect(g, 0, 0, w, h);
            //     // nvgFillPaint(g, imgPaint);
            //     nvgFillColor(g, nvgRGB(255, 0, 0));
            //     nvgFill(g);
            // fbo->unbindFrameBuffer();

            // printf("Fbo: w=%d, h=%d, image=%d, tex=%u\n", fbo->getWidth(), fbo->getHeight(), fbo->getImage(), fbo->getTexture());

            Number pxRatio = app.getRatio();
            fb = nvgluCreateFramebuffer(g, (int)(100 * pxRatio), (int)(100 * pxRatio), 0);

            nvgImageSize(g, fb->image, &w, &h);
            printf("Fbo: w=%d, h=%d, image=%d, tex=%u\n", w, h, fb->image, fb->texture);

            /*int winWidth, winHeight;
            int fboWidth, fboHeight;
            nvgImageSize(g, fb->image, &fboWidth, &fboHeight);
            winWidth = (int)(fboWidth / pxRatio);
            winHeight = (int)(fboHeight / pxRatio);

            // Draw some stull to an FBO as a test
            nvgluBindFramebuffer(fb);
            glViewport(0, 0, fboWidth, fboHeight);
            glClearColor(0, 0, 0, 0);
            glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
            nvgBeginFrame(g, winWidth, winHeight, pxRatio);

                nvgBeginPath(g);
                nvgRect(g, 0, 0, 200, 200);
                nvgFillColor(g, nvgRGBA(220, 160, 0, 200));
                nvgFill(g);

            nvgEndFrame(g);
            nvgluBindFramebuffer(NULL);*/
        }
        void render() {
            NVGpaint imgPaint = nvgImagePattern(g, 0, 0, 200, 200, 0, fb->image, 1.0f);
            nvgBeginPath(g);
            nvgRect(g, 0, 0, 200, 200);
            nvgFillPaint(g, imgPaint);
            // nvgFillColor(g, nvgRGB(0, 255, 255));
            nvgFill(g);
        }
    private:
        // MuFboRef fbo;
        NVGLUframebuffer *fb;
};

int main(int argc, char const *argv[]) {
    // Initialize everything
    app.init();
    // Set a game controller which provides REAL logic
    app.setController(new GameController);
    // Start the game
    app.start();
    // Cleanup anything and quit
    app.terminate();

    return 0;
}
