env = Environment(
    CC='clang', CXX='clang++',
    CPPPATH=['src'],
    LIBS=['glfw3'],
    FRAMEWORKS=['Cocoa', 'OpenGL', 'IOKit', 'CoreVideo']
)

env.Program(target='app', source=[
    'src/nanovg/nanovg.c',
    'src/perf.c',
    'src/main.cc',
])
