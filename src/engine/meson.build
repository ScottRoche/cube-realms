subdir('core')

engine_sources = []
engine_sources += core_sources

engine_lib = library('engine', engine_sources)
engine_dep = declare_dependency(link_with : engine_lib, include_directories : '.')