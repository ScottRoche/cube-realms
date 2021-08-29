subdir('core')
subdir('renderer')

engine_sources = []
engine_sources += core_sources
engine_sources += renderer_sources

cc = meson.get_compiler('c')
dl_dep = cc.find_library('dl', required : true)
pthread_dep = cc.find_library('pthread', required : true)
Xxf86vm_dep = cc.find_library('Xxf86vm', required : true)

glfw3 = dependency('glfw3')
vulkan = dependency('vulkan')
Xrandr = dependency('xrandr')
X11 = dependency('x11')
Xi = dependency('xi')
Xext = dependency('xext')

if get_option('buildtype') == 'debug'
	add_project_arguments('-DDEBUG=1', language : 'c')
endif

engine_dependencies = [glfw3,
                       vulkan,
                       Xrandr,
                       X11,
                       Xi,
                       Xext,
                       dl_dep,
                       pthread_dep,
                       Xxf86vm_dep]

engine_lib = library('engine',
                     engine_sources,
                     dependencies : engine_dependencies)
engine_dep = declare_dependency(link_with : engine_lib)