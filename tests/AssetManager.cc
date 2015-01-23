#include "../src/MuAssetsManager.h"
#include "../src/MuSurface.h"
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
        void init() {
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

            // Load a image (sync)
            img = nvgCreateImage(g, "assets/ship.png", NVG_IMAGE_GENERATE_MIPMAPS);
            int w, h;
            nvgImageSize(g, img, &w, &h);
            printf("Image(id: %d) loaded, size: [%d, %d]\n", img, w, h);
            // Load a image (async)
            assetsManager.loadAsset<Surface>("laser.png", [](DataSourceRef data) -> Surface {
                return Surface(data);
            }, [this](Surface surface) {
                img2 = nvgCreateImageRGBA(g, surface.getWidth(), surface.getHeight(), 0, surface.getData());
                int w, h;
                nvgImageSize(g, img2, &w, &h);
                loaded = true;
                printf("Image(id: %d) loaded, size: [%d, %d]\n", img2, w, h);
            });

            // Load a font
            assetsManager.loadAsset<Buffer>("Roboto-Bold.ttf", [](DataSourceRef data) -> Buffer {
                return Buffer(data);
            }, [this](Buffer data) {
                nvgCreateFontMem(g, "FontB", static_cast<unsigned char *>(data.getData()), data.getDataSize(), 0);
            });
        }
        void render() {
            // Drawing loaded assets
            nvgSave(g);

                int x = 960 / 2,
                    y = 640 / 2;

                int w, h;
                NVGpaint imagePaint;

                nvgImageSize(g, img, &w, &h);
                imagePaint = nvgImagePattern(g, x - w / 2, y - h / 2, w, h, 0, img, 1.0f);
                nvgBeginPath(g);
                nvgRect(g, x - w / 2, y - h / 2, w, h);
                nvgFillPaint(g, imagePaint);
                nvgFill(g);

                if (loaded) {
                    nvgImageSize(g, img2, &w, &h);
                    imagePaint = nvgImagePattern(g, x - w / 2, y - h / 2 - 100, w, h, 0, img2, 1.0f);
                    nvgBeginPath(g);
                    nvgRect(g, x - w / 2, y - h / 2 - 100, w, h);
                    nvgFillPaint(g, imagePaint);
                    nvgFill(g);

                    nvgFillColor(g, nvgRGBA(255, 255, 255, 255));
                    nvgFontFace(g, "FontB");
                    nvgFontSize(g, 64);
                    nvgTextAlign(g, NVG_ALIGN_MIDDLE | NVG_ALIGN_CENTER);
                    nvgText(g, x, y + 100, "Assets from Kenny", nullptr);
                }

            nvgRestore(g);
        }
    private:
        bool loaded;
        int img, img2;
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

