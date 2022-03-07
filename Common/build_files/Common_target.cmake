
target_include_directories( ${TARGET_NAME} PUBLIC ${OPENGL_INCLUDE_DIR} )
target_include_directories( ${TARGET_NAME} PUBLIC ${GLFW_INCLUDE_DIRS} )
target_include_directories( ${TARGET_NAME} PUBLIC "../Common/" )
target_include_directories( ${TARGET_NAME} PUBLIC "/usr/local/include/" )

add_library( common_lib
        )

target_link_libraries( common_lib ${GLEW_LIBRARIES}  ${GLFW_LIBRARIES}
        /usr/local/lib/libSOIL.a ${OPENGL_opengl_LIBRARY} ${OPENGL_glu_LIBRARY} ${OPENGL_glx_LIBRARY} )

#target_include_directories( common_lib PUBLIC /usr/local/include/eigen3/
#        /usr/local/include/nanovg/src/ )
#target_link_libraries( ${TARGET_NAME} /usr/local/lib/libSOIL.a )

target_link_libraries( ${TARGET_NAME} ${GLEW_LIBRARIES}  ${GLFW_LIBRARIES} )
target_link_libraries( ${TARGET_NAME} ${OPENGL_opengl_LIBRARY}
        ${OPENGL_glu_LIBRARY} ${OPENGL_glx_LIBRARY} )

target_link_libraries( ${TARGET_NAME} common_lib )

message( "GLFW Include : " ${GLFW_INCLUDE_DIRS} )
message( "GLFW Libs : " ${GLFW_LIBRARIES} )
message( "GLEW Include : " ${GLEW_INCLUDE_DIRS} )
message( "GLEW Libs : " ${GLEW_LIBRARIES} )
message( "OpenGL includes: " ${OPENGL_INCLUDE_DIR} )
message( "OpenGL libs: " ${OPENGL_opengl_LIBRARY} ", "
        ${OPENGL_glu_LIBRARY} ", "
        ${OPENGL_glx_LIBRARY} )
