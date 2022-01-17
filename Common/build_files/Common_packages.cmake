set( CMAKE_CXX_STANDARD 17 )
set( OpenGL_GL_PREFERENCE GLVND)

find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
find_package( glfw3 REQUIRED )
find_package( OpenGL REQUIRED )
find_package( GLEW REQUIRED )

find_package(Threads REQUIRED)
find_package(X11 REQUIRED)

#add_link_options(-lSOIL)
#add_compile_options(-Wall -Wextra -pedantic -Werror)