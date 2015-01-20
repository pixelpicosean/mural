/*
 Copyright (c) 2010, The Barbarian Group
 All rights reserved.

 Copyright (c) Microsoft Open Technologies, Inc. All rights reserved.

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and
    the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
    the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/

#include "MuGlobals.h"
#include "MuUtilities.h"
#include "MuUnicode.h"
#include "MuFilesystem.h"

#include <vector>
#include <boost/algorithm/string.hpp>

#if PLATFORM == PLATFORM_WINDOWS
#include <shlobj.h>
#elif PLATFORM == PLATFORM_LINUX
#include <linux/limits.h>
#include <unistd.h>
#endif
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>

using std::vector;
using std::string;
using std::wstring;

namespace mural {

    fs::path getAppPath() {
    #if PLATFORM == PLATFORM_WINDOWS
        TCHAR exePathWin[MAX_PATH];
        GetModuleFileName(NULL, exePathWin, MAX_PATH);
        string exePath(exePathWin);
        int length = exePath.length();
        if (exePath.substr(length - 4, length) == ".exe") {
            exePath = exePath.substr(0, length - 4);
        }
    #elif PLATFORM == PLATFORM_MAC
        CFBundleRef mainBundle = CFBundleGetMainBundle();
        CFURLRef exeURL = CFBundleCopyExecutableURL(mainBundle);
        char exePath[PATH_MAX];
        if (!CFURLGetFileSystemRepresentation(exeURL, TRUE, (UInt8 *)exePath, PATH_MAX)) {
            CFRelease(exeURL);
            printf("Couldn't get exe name.");
            return "";
        }
        CFRelease(exeURL);
    #elif PLATFORM == PLATFORM_LINUX
        string exePath = ReadWholeFile("/proc/self/cmdline");
        string splitChars = "";
        splitChars.push_back('\0'); // have to construct null-containing strings carefully
        exePath = split(exePath, splitChars)[0];
    #endif

        return expandPath(exePath);
    }

    fs::path expandPath(const fs::path &path) {
        return fs::canonical(path);
    }

    fs::path getHomeDirectory() {
#if PLATFORM == PLATFORM_WINDOWS
        return "./";
#elif PLATFORM == PLATFORM_MAC
        return getenv("HOME");
#elif PLATFORM == PLATFORM_LINUX
        return getenv("HOME");
#endif
    }

    fs::path getDocumentsDirectory() {
#if PLATFORM == PLATFORM_WINDOWS
        char myDocs[MAX_PATH];
        HRESULT result = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, myDocs);
        if (result != S_OK) {
            printf("ERROR: Couldn't get path to \"My Documents\" directory.");
            return string("");
        }
        else {
            return string(myDocs);
        }
#elif PLATFORM == PLATFORM_MAC
        string path = getenv("HOME");
        return path + "/Documents/";
#elif PLATFORM == PLATFORM_LINUX
        string path = getenv("HOME");
        return path + "/Documents/";
#endif
    }

    fs::path getTemporaryDirectory() {
        return "./";
    }

    fs::path getTemporaryFilePath(const std::string &prefix) {
        return "./";
    }

    std::string getPathDirectory(const std::string &path) {
        return "./";
    }

    std::string getPathFileName(const std::string &path) {
        return "./";
    }

    std::string getPathExtension(const std::string &path) {
        return "./";
    }

    bool createDirectories(const fs::path &path, bool createParents) {
        return false;
    }

    void deleteFileAsync(const fs::path &path, std::function<void (fs::path)> callback) {}

    void deleteFile(const fs::path &path) {}

    std::vector<std::string> split(const std::string &str, char separator, bool compress) {
        return split(str, string(1, separator), compress);
    }

    std::vector<std::string> split(const std::string &str, const std::string &separators, bool compress) {
        vector<string> result;

        boost::algorithm::split(result, str, boost::is_any_of(separators),
                                compress ? boost::token_compress_on : boost::token_compress_off);

        return result;
    }

    string loadString(DataSourceRef dataSource) {
        Buffer loadedBuffer(dataSource);
        size_t dataSize = loadedBuffer.getDataSize();
        Buffer padded(dataSize + 1);
        memcpy(padded.getData(), loadedBuffer.getData(), dataSize);
        (static_cast<uint8_t *>(padded.getData()))[dataSize] = 0;
        return string(static_cast<const char *>(padded.getData()));
    }

    void sleep(float milliseconds) {}

    int16_t swapEndian(int16_t val) {
        return (int16_t)((((uint16_t)(val) & (uint16_t) 0x00ffU) << 8) |
                         (((uint16_t)(val) & (uint16_t) 0xff00U) >> 8));
    }

    uint16_t swapEndian(uint16_t val) {
        return (uint16_t)((((uint16_t)(val) & (uint16_t) 0x00ffU) << 8) |
                          (((uint16_t)(val) & (uint16_t) 0xff00U) >> 8));
    }

    int32_t swapEndian(int32_t val) {
        return (int32_t)((((uint32_t)(val) & (uint32_t) 0x000000FFU) << 24) |
                         (((uint32_t)(val) & (uint32_t) 0x0000FF00U) <<  8) |
                         (((uint32_t)(val) & (uint32_t) 0x00FF0000U) >>  8) |
                         (((uint32_t)(val) & (uint32_t) 0xFF000000U) >> 24));
    }

    uint32_t swapEndian(uint32_t val) {
        return (uint32_t)((((uint32_t)(val) & (uint32_t) 0x000000FFU) << 24) |
                          (((uint32_t)(val) & (uint32_t) 0x0000FF00U) <<  8) |
                          (((uint32_t)(val) & (uint32_t) 0x00FF0000U) >>  8) |
                          (((uint32_t)(val) & (uint32_t) 0xFF000000U) >> 24));
    }

    float swapEndian(float val) {
        uint32_t temp = swapEndian(* reinterpret_cast<uint32_t *>(&val));
        return *(reinterpret_cast<float *>(&temp));
    }

    double swapEndian(double val) {
        union {
            double d;
            struct {
                uint32_t a;
                uint32_t b;
            } i;
        } s1, s2;
        s1.d = val;
        s2.i.a = swapEndian(s1.i.b);
        s2.i.b = swapEndian(s1.i.a);
        return s2.d;
    }

    void swapEndianBlock(uint16_t *blockPtr, size_t blockSizeInBytes) {
        size_t blockSize = blockSizeInBytes / sizeof(uint16_t);

        for (size_t b = 0; b < blockSize; b++) {
            *blockPtr = swapEndian(*blockPtr);
            blockPtr++;
        }
    }

    void swapEndianBlock(float *blockPtr, size_t blockSizeInBytes) {
        size_t blockSize = blockSizeInBytes / sizeof(float);

        for (size_t b = 0; b < blockSize; b++) {
            *(reinterpret_cast<uint32_t *>(blockPtr)) = swapEndian(*(reinterpret_cast<uint32_t *>(blockPtr)));
            blockPtr++;
        }
    }

}
