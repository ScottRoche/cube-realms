subdir('core')

engine_sources = []
engine_sources += core_sources

glfw3 = dependency('glfw3')

engine_lib = library('engine', engine_sources, dependencies : [glfw3])
engine_dep = declare_dependency(link_with : engine_lib, include_directories : '.')