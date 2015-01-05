env = Environment(
    CC='clang', CXX='clang++',
    CPPPATH=['src'],
    LIBS=['glfw3'],
    FRAMEWORKS=['Cocoa', 'OpenGL', 'IOKit', 'CoreVideo']
)

env.Program(target='app', source=[
    'src/nanovg.c',
    'example/demo.c',
    'example/perf.c',
    'example/example_gl2.c'
])
