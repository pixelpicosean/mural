#include "../src/MuCore.h"

#include "../src/MuAppController.h"
#include "../src/MuInputEvent.h"

#include <cstdio>

// Create a controller class for testing
class GameController : public mural::MuAppController {
    public:
        GameController() {
            app.addEventListener(this, mural::KeyboardEvent::KEY_DOWN);
            app.addEventListener(this, mural::KeyboardEvent::KEY_PRESS);
            app.addEventListener(this, mural::KeyboardEvent::KEY_UP);
        }
        ~GameController() {}
        void update(Number dt) {}
        void render(NVGcontext *ctx) {}
        void handleEvent(mural::Event *evt) {
            mural::KeyboardEvent *key = dynamic_cast<mural::KeyboardEvent *>(evt);
            if (!key) {
                return;
            }

            if (key->type == mural::KeyboardEvent::KEY_DOWN) {
                printf("Key %s(%i) down\n", key->code.c_str(), key->keyCode);
            }
            else if (key->type == mural::KeyboardEvent::KEY_PRESS) {
                printf("Key %s(%i) pressed\n", key->code.c_str(), key->keyCode);
            }
            else if (key->type == mural::KeyboardEvent::KEY_UP) {
                printf("Key %s(%i) up\n", key->code.c_str(), key->keyCode);
            }
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
