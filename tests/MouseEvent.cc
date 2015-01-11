#include "../src/MuCore.h"

#include "../src/MuAppController.h"
#include "../src/MuInputEvent.h"

// Create a controller class for testing
class GameController : public mural::MuAppController {
    public:
        GameController() {
            app.addEventListener(this, mural::MouseEvent::MOUSE_DOWN);
        }
        ~GameController() {}
        void update(Number dt) {}
        void render(NVGcontext *ctx) {}
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
