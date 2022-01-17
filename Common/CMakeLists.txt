cmake_minimum_required( VERSION 3.9 )
project( Engine )

include( ../Common/build_files/Common_packages.cmake )

set( TARGET_NAME Engine )

add_executable( ${TARGET_NAME}
        ../Common/main.cpp
        #IMGUI_BEGIN
        ../External/imgui-1.84.2/imconfig.h
        ../External/imgui-1.84.2/imgui.cpp
        ../External/imgui-1.84.2/imgui.h
        ../External/imgui-1.84.2/imgui_demo.cpp
        ../External/imgui-1.84.2/imgui_draw.cpp
        ../External/imgui-1.84.2/imgui_internal.h
        ../External/imgui-1.84.2/imgui_tables.cpp
        ../External/imgui-1.84.2/imgui_widgets.cpp
        ../External/imgui-1.84.2/imstb_rectpack.h
        ../External/imgui-1.84.2/imstb_textedit.h
        ../External/imgui-1.84.2/imstb_truetype.h
        ../External/imgui-1.84.2/imgui_impl_glfw.cpp
        ../External/imgui-1.84.2/imgui_impl_glfw.h
        ../External/imgui-1.84.2/imgui_impl_opengl3.cpp
        ../External/imgui-1.84.2/imgui_impl_opengl3.h
        ../External/imgui-1.84.2/imgui_impl_opengl3_loader.h
        #IMGUI_END
        ../Common/CommonConstants.h
        Shader.cpp
        Shader.h
        #        ../Common/Utilities.cpp
        #        ../Common/Utilities.h
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
        ../Common/SceneBase.h
        ../Common/ObjectComponents/ComponentBase.h
        ../Common/ObjectComponents/ComponentManager.h
        ../Common/ObjectComponents/ComponentManager.inl
        ../Common/TestScene.h
        ../Common/Object.cpp
        ../Common/Object.h
        ../Common/Camera.cpp
        ../Common/Camera.h
        ../Common/SceneBase.cpp
        ../Common/Light.h
        #GUI stuffs begin
        ../Common/GUI/GUIManager.h
        ../Common/GUI/GUIManager.cpp
        ../Common/GUI/GUIObject.h
        ../Common/GUI/GUIWindow.cpp
        ../Common/GUI/GUIWindow.h
        ../Common/GUI/GUIContent.h
        ../Common/GUI/CurrentCameraInfoContent.h
        ../Common/GUI/CurrentCameraInfoContent.cpp
        ../Common/GUI/EngineInfo.cpp
        ../Common/GUI/EngineInfo.h
        ../Common/GUI/ObjectListContent.cpp
        ../Common/GUI/ObjectListContent.h
        #GUI stuffs end

        GUI/ColorSet.h
        GUI/ObjectDetailContent.cpp
        GUI/ObjectDetailContent.h
        VAOManager.cpp
        VAOManager.h
        VBOManager.cpp
        VBOManager.h
        UBO.cpp
        UBO.h
        GUI/LightListContent.cpp
        GUI/LightListContent.h
        GUI/LightDetailContent.cpp
        GUI/LightDetailContent.h
        Environment.h
        TextureManager.cpp
        TextureManager.h
        TextureObject.cpp
        TextureObject.h
        GUI/EngineInfoContent.cpp GUI/EngineInfoContent.h TestScene.cpp)

# Set target properties
target_include_directories( ${TARGET_NAME} PUBLIC ${OPENGL_INCLUDE_DIR} )
target_include_directories( ${TARGET_NAME} PUBLIC ${GLFW_INCLUDE_DIRS} )
target_include_directories( ${TARGET_NAME} PUBLIC "../Common/" )
target_include_directories( ${TARGET_NAME} PUBLIC "../Common/shaders/" )
target_include_directories( ${TARGET_NAME} PUBLIC /usr/local/include/ )
target_include_directories( ${TARGET_NAME} PUBLIC "../External/imgui-1.84.2/" )

#target_include_directories( common_lib PUBLIC /usr/local/include/eigen3/
#        /usr/local/include/nanovg/src/ )

target_link_libraries( ${TARGET_NAME} ${GLEW_LIBRARIES}  ${GLFW_LIBRARIES} -lSOIL )
target_link_libraries( ${TARGET_NAME} ${OPENGL_opengl_LIBRARY}
        ${OPENGL_glu_LIBRARY} ${OPENGL_glx_LIBRARY} )
target_link_libraries(${CMAKE_PROJECT_NAME}
        ${CMAKE_THREAD_LIBS_INIT}
        ${X11_LIBRARIES}
        ${CMAKE_DL_LIBS}
        )

message( "GLFW Include : " ${GLFW_INCLUDE_DIRS} )
message( "GLFW Libs : " ${GLFW_LIBRARIES} )
message( "GLEW Include : " ${GLEW_INCLUDE_DIRS} )
message( "GLEW Libs : " ${GLEW_LIBRARIES} )
message( "OpenGL includes: " ${OPENGL_INCLUDE_DIR} )
message( "OpenGL libs: " ${OPENGL_opengl_LIBRARY} ", "
        ${OPENGL_glu_LIBRARY} ", "
        ${OPENGL_glx_LIBRARY} )