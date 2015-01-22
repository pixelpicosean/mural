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

#pragma once

#include "MuGlobals.h"
#include "MuArea.h"

namespace mural {

    /**
     * Surface represents image data, currently ONLY RGBA supported!
     */
    class Surface : public MuObject {
        public:
            typedef unsigned char PixelFormat;

            //! A null Surface.
            Surface();
            //! Allocates a Surface of size \a width X \a height.
            Surface(int32_t width, int32_t height);
            //! Constructs a surface from the memory pointed to by \a data. Does not assume ownership of the memory in \a data, which consequently should not be freed while the Surface is still in use.
            Surface(PixelFormat *data, int32_t width, int32_t height, int32_t rowBytes);
            //! Creates a clone of \a rhs. Matches rowBytes and channel order of \a rhs, but creates its own dataStore.
            Surface(const Surface &rhs);
            //! Surface move constructor.
            Surface(Surface &&rhs);

            //! Creates a SurfaceRef of size \a width X \a height, with an optional \a alpha channel. The default value for \a channelOrder selects a platform default.
            static std::shared_ptr<Surface> create(int32_t width, int32_t height) {
                return std::make_shared<Surface>(width, height);
            }
            //! Creates a SurfaceRef from the memory pointed to by \a data. Does not assume ownership of the memory in \a data, which consequently should not be freed while the Surface is still in use.
            static std::shared_ptr<Surface> create(PixelFormat *data, int32_t width, int32_t height, int32_t rowBytes) {
                return std::make_shared<Surface>(data, width, height, rowBytes);
            }
            //! Creates s SurfaceRef which is a clone of the Surface \a surface, and with its own dataStore
            static std::shared_ptr<Surface> create(const Surface &surface) {
                return std::make_shared<Surface>(surface);
            }

            Surface &operator=(const Surface &rhs);
            Surface& operator=(Surface &&rhs);

            //! Returns the width of the Surface in pixels
            int32_t getWidth() const { return width; }
            //! Returns the height of the Surface in pixels
            int32_t getHeight() const { return height; }
            //! Returns the size of the Surface in pixels
            ivec2 getSize() const { return ivec2(width, height); }
            //! Returns the bounding Area of the Surface in pixels: [0,0]-(width,height)
            Area getBounds() const { return Area(0, 0, width, height); }

            //! Returns the width of a row of the Surface measured in bytes
            int32_t getRowBytes() const { return rowBytes; }

            //! Returns a new Surface which is a duplicate. If \a copyPixels the pixel values are copied, otherwise the clone's pixels remain uninitialized
            Surface clone(bool copyPixels = true) const;
            //! Returns a new Surface which is a duplicate of an Area \a area. If \a copyPixels the pixel values are copied, otherwise the clone's pixels remain uninitialized
            Surface clone(const Area &area, bool copyPixels = true) const;

            PixelFormat *getData() {
                return data;
            }
            const PixelFormat *getData() const {
                return data;
            }
            //! Returns the shared_ptr to the underlying pixel data. Maybe be nullptr if the Surface does not own its data
            std::shared_ptr<PixelFormat> getDataStore() const { return dataStore; }

            //! Copies the Area \a srcArea of the Surface \a srcSurface to \a this Surface. The destination Area is \a srcArea offset by \a relativeOffset.
            void copyFrom(const Surface &srcSurface, const Area &srcArea, const ivec2 &relativeOffset = ivec2());

        private:
            int32_t width, height, rowBytes;
            PixelFormat *data;
            std::shared_ptr<PixelFormat> dataStore; // shared rather than unique because member Channels (r/g/b/a) share the same data store and may need to outlive their parent Surface

            void copyRawRgba(const Surface &srcSurface, const Area &srcArea, const ivec2 &absoluteOffset);
    };

}
