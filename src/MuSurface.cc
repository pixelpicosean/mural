/*
Copyright (c) 2015 Sean Bohan

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "MuSurface.h"
#include "nanovg/nanovg.h"

#include <memory>

namespace mural {

    Surface::Surface():
        width(0),
        height(0),
        rowBytes(0),
        data(nullptr)
    {}

    Surface::Surface(int32_t width, int32_t height):
        width(width), height(height)
    {
        rowBytes = width * sizeof(PixelFormat) * 4; // RGBA
        dataStore = std::shared_ptr<PixelFormat>(new PixelFormat[height * rowBytes], std::default_delete<PixelFormat[]>());
        data = dataStore.get();
    }

    Surface::Surface(PixelFormat *data, int32_t width, int32_t height, int32_t rowBytes):
        width(width), height(height), rowBytes(rowBytes), data(data)
    {}

    Surface::Surface(DataSourceRef dataSource) {
        Buffer buffer(dataSource);
        size_t dataSize = buffer.getDataSize();

        int n;
        dataStore = std::shared_ptr<PixelFormat>(nvgLoadImageFromMem(static_cast<PixelFormat *>(buffer.getData()), dataSize, &width, &height, &n, 4)); // RGBA
        data = dataStore.get();

        rowBytes = width * sizeof(PixelFormat) * 4; // RGBA
    }

    Surface::Surface(const Surface &rhs):
        width(rhs.width), height(rhs.height), rowBytes(rhs.rowBytes)
    {
        dataStore = std::shared_ptr<PixelFormat>(new PixelFormat[height * rowBytes], std::default_delete<PixelFormat[]>());
        data = dataStore.get();
        copyFrom(rhs, Area(0, 0, width, height));
    }

    Surface::Surface(Surface &&rhs):
        width(rhs.width), height(rhs.height), rowBytes(rhs.rowBytes)
    {
        dataStore = rhs.dataStore;
        rhs.dataStore = nullptr;
        data = dataStore.get();
    }

    Surface& Surface::operator=(const Surface &rhs) {
        width = rhs.width;
        height = rhs.height;
        rowBytes = rhs.rowBytes;
        dataStore = std::shared_ptr<PixelFormat>(new PixelFormat[height * rowBytes], std::default_delete<PixelFormat[]>());

        data = dataStore.get();

        copyFrom(rhs, Area(0, 0, width, height));

        return *this;
    }

    Surface& Surface::operator=(Surface &&rhs) {
        width = rhs.width;
        height = rhs.height;
        rowBytes = rhs.rowBytes;
        dataStore = rhs.dataStore;
        data = rhs.data;
        rhs.dataStore = nullptr;
        rhs.data = nullptr;

        return *this;
    }

    void Surface::copyFrom(const Surface &srcSurface, const Area &srcArea, const ivec2 &relativeOffset) {
        std::pair<Area, ivec2> srcDst = clippedSrcDst(srcSurface.getBounds(), srcArea, getBounds(), srcArea.getUL() + relativeOffset);

        copyRawRgba(srcSurface, srcDst.first, srcDst.second);
    }

    void Surface::copyRawRgba(const Surface &srcSurface, const Area &srcArea, const ivec2 &absoluteOffset) {
        const int32_t srcRowBytes = srcSurface.getRowBytes();

        // RGBA for source
        uint8_t srcRed = 0;
        uint8_t srcGreen = 1;
        uint8_t srcBlue = 2;
        uint8_t srcAlpha = 3;
        // RGBA for target
        uint8_t dstRed = 0;
        uint8_t dstGreen = 1;
        uint8_t dstBlue = 2;
        uint8_t dstAlpha = 3;

        int32_t width = srcArea.getWidth();

        for (int32_t y = 0; y < srcArea.getHeight(); ++y) {
            const PixelFormat *src = reinterpret_cast<const PixelFormat *>(reinterpret_cast<const uint8_t *>(srcSurface.getData() + srcArea.x1 * 4) + (srcArea.y1 + y) * srcRowBytes);
            PixelFormat *dst = reinterpret_cast<PixelFormat *>(reinterpret_cast<uint8_t*>(getData() + absoluteOffset.x * 4) + (y + absoluteOffset.y) * getRowBytes());
            for (int x = 0; x < width; ++x) {
                dst[dstRed] = src[srcRed];
                dst[dstGreen] = src[srcGreen];
                dst[dstBlue] = src[srcBlue];
                dst[dstAlpha] = src[srcAlpha];
                src += 4;
                dst += 4;
            }
        }
    }

}
