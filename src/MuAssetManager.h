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
#include "MuFilesystem.h"
#include "MuDataSource.h"
#include "MuException.h"

#include <vector>

namespace mural {

    //! Exception for failed asset loading
    class AssetLoadExc : public Exception {
        public:
            AssetLoadExc(const fs::path &relativePath);

            virtual const char *what() const throw() {
                return mMessage;
            }

            char mMessage[4096];
    };

    class AssetManager : public MuObject {
        public:
            static AssetManager &getInstance() {
                static AssetManager instance;
                return instance;
            }

            //! Returns a DataSourceRef to the active App's's asset. Throws a AssetLoadExc on failure.
            DataSourceRef loadAsset(const fs::path &relativePath);
            //! Returns a fs::path to the active App's asset. Returns an empty path on failure.
            fs::path getAssetPath(const fs::path &relativePath);
            //! Adds an absolute path \a dirPath to the active App's list of directories which are searched for assets.
            void addAssetDirectory(const fs::path &dirPath);

        private:
            AssetManager();
            AssetManager(AssetManager const &) {}
            void operator=(AssetManager const &) {}

            ~AssetManager();

            void prepareAssetLoading();
            fs::path findAssetPath(const fs::path &relativePath);

        private:
            bool assetDirectoriesInitialized;

            fs::path appPath;
            std::vector<fs::path> assetDirectories;
    };

}

#define assetsManager mural::AssetManager::getInstance()
