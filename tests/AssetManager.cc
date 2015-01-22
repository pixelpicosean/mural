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
        GameController(): fired(false), loaded(false) {}
        ~GameController() {}
        void update(Number dt) {}
        void render(NVGcontext *ctx) {
            if (!fired) {
                test(ctx);
                fired = true;
            }

            // Drawing loaded assets
            nvgSave(ctx);

                int x = 960 / 2,
                    y = 640 / 2;

                int w, h;
                NVGpaint imagePaint;

                nvgImageSize(ctx, img, &w, &h);
                imagePaint = nvgImagePattern(ctx, x - w / 2, y - h / 2, w, h, 0, img, 1.0f);
                nvgBeginPath(ctx);
                nvgRect(ctx, x - w / 2, y - h / 2, w, h);
                nvgFillPaint(ctx, imagePaint);
                nvgFill(ctx);

                if (loaded) {
                    nvgImageSize(ctx, img2, &w, &h);
                    imagePaint = nvgImagePattern(ctx, x - w / 2, y - h / 2 - 100, w, h, 0, img2, 1.0f);
                    nvgBeginPath(ctx);
                    nvgRect(ctx, x - w / 2, y - h / 2 - 100, w, h);
                    nvgFillPaint(ctx, imagePaint);
                    nvgFill(ctx);

                    nvgFillColor(ctx, nvgRGBA(255, 255, 255, 255));
                    nvgFontFace(ctx, "FontB");
                    nvgFontSize(ctx, 64);
                    nvgTextAlign(ctx, NVG_ALIGN_MIDDLE | NVG_ALIGN_CENTER);
                    nvgText(ctx, x, y + 100, "Assets from Kenny", nullptr);
                }

            nvgRestore(ctx);
        }
        void handleEvent(mural::Event *evt) {}
        void test(NVGcontext *ctx) {
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
            img = nvgCreateImage(ctx, "assets/ship.png", NVG_IMAGE_GENERATE_MIPMAPS);
            int w, h;
            nvgImageSize(ctx, img, &w, &h);
            printf("Image(id: %d) loaded, size: [%d, %d]\n", img, w, h);
            // Load a image (async)
            assetsManager.loadAsset<Surface>("laser.png", [](DataSourceRef data) -> Surface {
                return Surface(data);
            }, [this, ctx](Surface surface) {
                img2 = nvgCreateImageRGBA(ctx, surface.getWidth(), surface.getHeight(), 0, surface.getData());
                int w, h;
                nvgImageSize(ctx, img2, &w, &h);
                loaded = true;
                printf("Image(id: %d) loaded, size: [%d, %d]\n", img2, w, h);
            });

            // Load a font
            assetsManager.loadAsset<Buffer>("Roboto-Bold.ttf", [](DataSourceRef data) -> Buffer {
                return Buffer(data);
            }, [ctx](Buffer data) {
                nvgCreateFontMem(ctx, "FontB", static_cast<unsigned char *>(data.getData()), data.getDataSize(), 0);
            });
        }
    private:
        bool fired, loaded;
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

