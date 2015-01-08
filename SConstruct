# Name of this app
APP_NAME = 'app'

vars = Variables()
# Supported platforms
vars.Add(EnumVariable('platform', 'Compile for which platform', 'osx', allowed_values=('osx', 'win')))
# 3 build modes
vars.Add(EnumVariable('mode', 'Building mode', 'debug', allowed_values=('debug', 'profile', 'release')))
settings = Environment(variables=vars)
Export('settings')
Help(vars.GenerateHelpText(settings))

# Build for OSX
envOSX = Environment(
    CC='clang', CXX='clang++',
    CXXFLAGS = ['-std=c++11', '-stdlib=libc++'],
    CPPPATH=['src'],
    LIBS=['glfw3'],
    FRAMEWORKS=['Cocoa', 'OpenGL', 'IOKit', 'CoreVideo']
)

# Build for Windows (MinGW)
envWin = Environment(
    CC='/usr/local/gcc-4.8.0-qt-4.8.4-for-mingw32/win32-gcc/bin/i586-mingw32-gcc',
    CXX='/usr/local/gcc-4.8.0-qt-4.8.4-for-mingw32/win32-gcc/bin/i586-mingw32-g++',
    AR='/usr/local/gcc-4.8.0-qt-4.8.4-for-mingw32/win32-gcc/bin/i586-mingw32-ar',
    RANLIB='/usr/local/gcc-4.8.0-qt-4.8.4-for-mingw32/win32-gcc/bin/i586-mingw32-ranlib',
    CXXFLAGS=['-std=c++11'],
    CPPPATH=['./include', '/usr/local/gcc-4.8.0-qt-4.8.4-for-mingw32/win32-gcc/i586-mingw32/include'],
    LIBPATH=['./lib', '/usr/local/gcc-4.8.0-qt-4.8.4-for-mingw32/win32-gcc/i586-mingw32/lib'],
    LIBS=['glfw3', 'GLEW', 'glu32', 'opengl32', 'gdi32', 'user32', 'kernel32']
)

# Build based on settings
if settings['platform'] == 'osx':
    env = envOSX
    TARGET = APP_NAME
elif settings['platform'] == 'win':
    env = envWin
    TARGET = APP_NAME + '.exe'

# basic flags according to build mode
if settings['mode'] == 'debug':
    env.Append(CCFLAGS = ['-Wall', '-g', '-O0', '-DDEBUG'])
elif settings['mode'] == 'release':
    env.Append(CCFLAGS = ['-Wall', '-O3', '-DNDEBUG'])
    # env.Append(LINKFLAGS = ['-s'])
elif settings['mode'] == 'profile':
    env.Append(CCFLAGS = ['-Wall', '-pg', '-O0', '-DNDEBUG'])

env.Program(target='bin/' + settings['mode'] + '/' + settings['platform'] + '/' + TARGET, source=[
    'src/nanovg/nanovg.c',
    'src/perf.c',
    'src/MuEvent.cc',
    'src/MuEventDispatcher.cc',
    'src/MuCore.cc',
    'src/main.cc'
])
