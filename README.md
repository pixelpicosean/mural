Mural *NanoVG* branch (experimental)
----

[NanoVG](https://github.com/memononen/nanovg) has finished lots of Canvas API implementation, so I'm trying to use it as backend for Mural.

## Dependence information
- Boost: [CinderBoost](https://github.com/cinder/Cinder-Boost/tree/dev)

## Notes
- Since MinGW for OSX does not has C++11 thread support, the cross-compiling from OSX is going to be deprecated. A new build setting for Mingw-w64 will be created and use ONLY in Windows.

## Progress

- [x] SCons building system
- [x] Basic app setup
- [x] CPU & GPU Graph
- [x] Make build file support OSX, Windows
- [x] Foundation classes (Event, App...)
- [x] Mouse event
- [x] Keyboard event
- [x] Active class for async operations
- [x] Import Cinder specific Boost as dependence
- [x] Cinder like file system
- [x] AssetManager for async file io
- [ ] FBO to cache drawings as textures
- [ ] FBO based CanvasContext
- [ ] Add some signals to MuCore so that MuCore will not need to be modified when new module added
- [ ] Use a config to compile tests instead of each time
- [ ] Build file for Linux x86 and x64
- [ ] Build file for native Mingw-w64 instead of cross-compiling
- [ ] Separate source and build directory
- [ ] Use \` to toggle CPU & FrameTime Graph
- [ ] JavaScript bindings
- [ ] Include GLEW source with SCons build file for Windows(MinGW)
