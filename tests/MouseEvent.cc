#include "../src/MuCore.h"

#include "../src/MuAppController.h"
#include "../src/MuInputEvent.h"

#include <cstdio>

// Create a controller class for testing
class GameController : public mural::MuAppController {
    private:
        bool leftBtn, rightBtn;
        bool alt, ctrl, meta, shift;
        int mouseX, mouseY;
    public:
        GameController():
            leftBtn(false), rightBtn(false),
            alt(false), ctrl(false), meta(false), shift(false),
            mouseX(0), mouseY(0)
        {
            app.addEventListener(this, mural::MouseEvent::MOUSE_DOWN);
            app.addEventListener(this, mural::MouseEvent::MOUSE_UP);
            app.addEventListener(this, mural::MouseEvent::MOUSE_MOVE);
        }
        ~GameController() {}
        void update(Number dt) {}
        void render(NVGcontext *ctx) {
            float strokeWidth = 6.0f;
            nvgStrokeWidth(ctx, strokeWidth);
            nvgStrokeColor(ctx, nvgRGB(255, 255, 255));

            float marginLeft = 100.0f, marginTop = 100.0f;
            float w = 240.0f, h = 340.0f;
            float btnHeight = 120.0f;

            // Left button
            if (leftBtn) {
                nvgBeginPath(ctx);
                nvgRect(ctx, marginLeft + strokeWidth / 2, marginTop, w / 2 - strokeWidth, btnHeight - strokeWidth);
                nvgFillColor(ctx, nvgRGB(0, 255, 255));
                nvgFill(ctx);
            }
            // Right button
            if (rightBtn) {
                nvgBeginPath(ctx);
                nvgRect(ctx, marginLeft + w / 2 + strokeWidth / 2, marginTop, w / 2 - strokeWidth, btnHeight - strokeWidth);
                nvgFillColor(ctx, nvgRGB(0, 255, 255));
                nvgFill(ctx);
            }

            // Mouse body
            nvgBeginPath(ctx);
            nvgRoundedRect(ctx, marginLeft, marginTop, w, h, 12.0f);
            nvgStroke(ctx);
            nvgBeginPath(ctx);
            nvgMoveTo(ctx, marginLeft + w / 2, marginTop + strokeWidth / 2);
            nvgLineTo(ctx, marginLeft + w / 2, marginTop + btnHeight);
            nvgMoveTo(ctx, marginLeft, marginTop + btnHeight - strokeWidth / 2);
            nvgLineTo(ctx, marginLeft + w, marginTop + btnHeight - strokeWidth / 2);
            nvgStroke(ctx);

            nvgFontSize(ctx, 32);
            nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
            String text("Position: (");
            text += std::to_string(mouseX); text += ", ";
            text += std::to_string(mouseY); text += ")";
            nvgText(ctx, 110.0f, 70.0f, text.c_str(), NULL);

            nvgBeginPath(ctx);
            // TODO: draw buttons
        }
        void handleEvent(mural::Event *evt) {
            mural::MouseEvent *mouse = dynamic_cast<mural::MouseEvent *>(evt);

            leftBtn = mouse->buttons & 1;
            rightBtn = mouse->buttons & 2;

            mouseX = mouse->x;
            mouseY = mouse->y;

            alt = mouse->altDown;
            ctrl = mouse->ctrlDown;
            meta = mouse->metaDown;
            shift = mouse->shiftDown;
        }
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
