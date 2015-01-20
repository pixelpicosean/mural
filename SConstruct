# Name of this app
APP_NAME = 'app'

vars = Variables()
# Supported platforms
vars.Add(EnumVariable('platform', 'Compile for which platform', 'osx', allowed_values=('osx', 'win')))
# 3 build modes
vars.Add(EnumVariable('mode', 'Building mode', 'debug', allowed_values=('debug', 'profile', 'release')))
settings = Environment(variables=vars)
Help(vars.GenerateHelpText(settings))

# Source files (main.cc not included)
SOURCES = [
    'src/linebreak/linebreak.c',
    'src/linebreak/linebreakdata.c',
    'src/linebreak/linebreakdef.c',

    'src/nanovg/nanovg.c',
    'src/perf.c',
    'src/MuEvent.cc',
    'src/MuInputEvent.cc',
    'src/MuEventDispatcher.cc',
    'src/MuTimer.cc',
    'src/MuCore.cc',
    'src/MuUnicode.cc'
]

# Build for OSX
envOSX = Environment(
    CC='clang', CXX='clang++',
    CXXFLAGS = ['-std=c++11', '-stdlib=libc++', '-mmacosx-version-min=10.8'],
    CPPPATH=['src', 'src/boost', 'src/zlib-1.2.3'],
    LIBPATH=['lib/macosx'],
    LIBS=['glfw3', 'z', 'boost_date_time', 'boost_filesystem', 'boost_system'],
    FRAMEWORKS=['Cocoa', 'OpenGL', 'IOKit', 'CoreVideo']
)

# Build for Windows (MinGW)
envWin = Environment(
    CC='/usr/local/gcc-4.8.0-qt-4.8.4-for-mingw32/win32-gcc/bin/i586-mingw32-gcc',
    CXX='/usr/local/gcc-4.8.0-qt-4.8.4-for-mingw32/win32-gcc/bin/i586-mingw32-g++',
    AR='/usr/local/gcc-4.8.0-qt-4.8.4-for-mingw32/win32-gcc/bin/i586-mingw32-ar',
    RANLIB='/usr/local/gcc-4.8.0-qt-4.8.4-for-mingw32/win32-gcc/bin/i586-mingw32-ranlib',
    CXXFLAGS=['-std=c++11'],
    CPPPATH=['src', 'include', 'src/boost', '/usr/local/gcc-4.8.0-qt-4.8.4-for-mingw32/win32-gcc/i586-mingw32/include'],
    LIBPATH=['lib', '/usr/local/gcc-4.8.0-qt-4.8.4-for-mingw32/win32-gcc/i586-mingw32/lib'],
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

# Export to sub-directories
Export('env', 'SOURCES')

# Compile the executable file
MAIN_SOURCE_LIST = list(SOURCES)
MAIN_SOURCE_LIST.append('src/main.cc')
env.Program(
    target='bin/' + settings['mode'] + '/' + settings['platform'] + '/' + TARGET,
    source=MAIN_SOURCE_LIST
)

# Compile tests
TESTS = [
    'MouseEvent',
    'KeyboardEvent',
    'Active',
    'Timer'
]

for t in TESTS:
    l = list(SOURCES)
    l.append('tests/' + t + '.cc');
    env.Program(target=('tests/bin/' + t), source=l)
