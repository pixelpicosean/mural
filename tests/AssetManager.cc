#include "../src/MuGlobals.h"
#include "../src/MuAssetManager.h"

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

int main(int argc, char const *argv[]) {
    printf("Path of 'text_utf8.txt' is: %s\n\n", assetsManager.getAssetPath("text_utf8.txt").c_str());

    std::string u8 = loadStringFromFile<std::string>(assetsManager.loadAsset("text_utf8.txt"));
    printf("Content of 'text_utf8.txt' is:\n%s\n", u8.c_str());

    return 0;
}
