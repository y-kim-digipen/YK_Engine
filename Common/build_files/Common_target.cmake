
target_include_directories( ${TARGET_NAME} PUBLIC ${OPENGL_INCLUDE_DIR} )
target_include_directories( ${TARGET_NAME} PUBLIC ${GLFW_INCLUDE_DIRS} )
target_include_directories( ${TARGET_NAME} PUBLIC "../Common/" )
target_include_directories( ${TARGET_NAME} PUBLIC "/usr/local/include/" )

add_library( common_lib
        ../Common/shader.cpp
        ../Common/shader.hpp
#        ../Common/Utilities.cpp
#        ../Common/Utilities.h
        ../Common/Scene.cpp
        ../Common/Scene.h
#        ../Common/SceneManager.cpp
#        ../Common/SceneManager.h
        ../Common/OBJReader.cpp
        ../Common/OBJReader.h
        ../Common/Mesh.cpp
        ../Common/Mesh.h
#        ../Common/VertexDataManager.cpp
#        ../Common/VertexDataManager.h
#        ../Common/UniformBufferManager.cpp
#        ../Common/UniformBufferManager.h
#        ../Common/TextureManager.cpp
#        ../Common/TextureManager.h
#        ../Common/TextureManager430.cpp
#        ../Common/TextureManager430.h
#        ../Common/ShaderManager.cpp
#        ../Common/ShaderManager.h
#        ../Common/GeometryShaderManager.cpp
#        ../Common/GeometryShaderManager.h
#        ../Common/Light.cpp
#        ../Common/Light.h
        ../Common/Input/KeyCodes.h
        ../Common/Input/InputManager.cpp
        ../Common/Input/InputManager.h
        ../Common/Engine.cpp
        ../Common/Engine.h
        ../Common/Color.h


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
