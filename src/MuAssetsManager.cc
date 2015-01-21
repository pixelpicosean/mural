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

#include "MuAssetsManager.h"
#include "MuUtilities.h"

#include <functional>

namespace mural {

    AssetLoadExc::AssetLoadExc(const fs::path &relativePath):
        Exception(relativePath.string())
    {}

    AssetsManager::AssetsManager():
        assetDirectoriesInitialized(false)
    {}

    AssetsManager::~AssetsManager() {}

    DataSourceRef AssetsManager::loadAssetSync(const fs::path &relativePath) {
        fs::path assetPath = findAssetPath(relativePath);
        if (!assetPath.empty())
            return DataSourcePath::create(assetPath.string());
        else
            throw AssetLoadExc(relativePath);
    }

    fs::path AssetsManager::getAssetPath(const fs::path &relativePath) {
        return findAssetPath(relativePath);
    }

    void AssetsManager::addAssetDirectory(const fs::path &dirPath) {
        assetDirectories.push_back(dirPath);
    }

    void AssetsManager::prepareAssetLoading() {
        if (!assetDirectoriesInitialized) {
            // first search the local directory, then its parent, up to 5 levels up
            int parentCt = 0;
            for (fs::path curPath = appPath;
                    curPath.has_parent_path() || (curPath == appPath); //check at least the app path, even if it has no parent directory
                    curPath = curPath.parent_path(), ++parentCt) {
                if (parentCt >= 5)
                    break;

                const fs::path curAssetPath = curPath / "assets";
                if (fs::exists(curAssetPath) && fs::is_directory(curAssetPath)) {
                    assetDirectories.push_back(curAssetPath);
                    break;
                }
            }

            assetDirectoriesInitialized = true;
        }
    }

    fs::path AssetsManager::findAssetPath(const fs::path &relativePath) {
        prepareAssetLoading();
        for (std::vector<fs::path>::const_iterator assetDirIt = assetDirectories.begin(); assetDirIt != assetDirectories.end(); ++assetDirIt) {
            if (fs::exists(*assetDirIt / relativePath))
                return (*assetDirIt / relativePath);
        }

        // empty implies failure
        return fs::path();
    }

}
