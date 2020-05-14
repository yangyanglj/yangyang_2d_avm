#包含文件
#所有的cpp文件包含到source list当中
set(SRC_LIST main.cpp)

add_executable(srv_lut_gen ${SRC_LIST})

set(OPENGL_LIBS GL glfw3 X11 Xxf86vm Xrandr pthread Xi dl Xinerama Xcursor m)

target_link_libraries(srv_lut_gen ${OPENGL_LIBS})


