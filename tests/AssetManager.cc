#include "../src/MuAssetsManager.h"
#include "../src/MuAppController.h"

#include "../src/MuCore.h"

#include <cstdio>
#include <string>

using namespace mural;

template<typename TYPE>
TYPE loadStringFromFile(const DataSourceRef &dataSource) {
    typedef typename TYPE::value_type T;
    Buffer loadedBuffer(dataSource);
    size_t dataSize = loadedBuffer.getDataSize();
    Buffer padded(dataSize + sizeof(T));
    memcpy(padded.getData(), loadedBuffer.getData(), dataSize);
    (static_cast<T *>(padded.getData()))[dataSize / sizeof(T)] = 0;
    return TYPE(static_cast<const T *>(padded.getData()));
}

// Create a controller class for testing
class GameController : public mural::MuAppController {
    public:
        GameController(): loaded(false) {}
        ~GameController() {}
        void update(Number dt) {
            if (!loaded) {
                test();
                loaded = true;
            }
        }
        void render(NVGcontext *ctx) {}
        void handleEvent(mural::Event *evt) {}
        void test() {
            // Search assets folder
            printf("Path of 'text_utf8.txt' is: %s\n\n", assetsManager.getAssetPath("text_utf8.txt").c_str());

            // Synchoronous content loading
            std::string u8 = loadStringFromFile<std::string>(assetsManager.loadAssetSync("text_utf8.txt"));
            printf("Sync load 'text_utf8.txt':\n%s\n", u8.c_str());

            // Asynchronous content loading
            assetsManager.loadAsset<std::string>("text_utf8.txt", [](DataSourceRef data) -> std::string {
                return loadStringFromFile<std::string>(data);
            }, [](std::string content) {
                printf("Async load 'text_utf8.txt':\n%s\n", content.c_str());
            });
            printf("This should print before async content loading completed\n");
        }
    private:
        bool loaded;
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

