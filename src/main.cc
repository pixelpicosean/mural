#include "MuCore.h"

#include "MuAppController.h"
#include "MuInputEvent.h"

#include <cstdio>

// Create a controller class for testing
class GameController : public mural::MuAppController {
    public:
        GameController() {
            app.addEventListener(this, mural::MouseEvent::MOUSE_DOWN);
        }
        void render() {
            Number radius = 64.0f;
            nvgFillColor(g, nvgRGBA(220, 160, 0, 200));

            nvgBeginPath(g);
            nvgCircle(g, radius, radius, radius);
            nvgFill(g);

            nvgBeginPath(g);
            nvgCircle(g, 960.0f - radius, radius, radius);
            nvgFill(g);

            nvgBeginPath(g);
            nvgCircle(g, radius, 640.0f - radius, radius);
            nvgFill(g);

            nvgBeginPath(g);
            nvgCircle(g, 960.0f - radius, 640.0f - radius, radius);
            nvgFill(g);
        }
        void handleEvent(mural::Event *evt) {
            mural::MouseEvent *mouse = dynamic_cast<mural::MouseEvent *>(evt);
            printf("Mouse pressed at (%d, %d)\n", mouse->x, mouse->y);
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
