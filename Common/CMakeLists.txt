cmake_minimum_required( VERSION 3.9 )
project( Engine )

include( ../Common/build_files/Common_packages.cmake )

set( TARGET_NAME Engine )

add_executable( ${TARGET_NAME}
        engine/main.cpp
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
        engine/CommonConstants.h
        engine/graphic_misc/Shader.cpp
        engine/graphic_misc/Shader.h
        #        ../Common/Utilities.cpp
        #        ../Common/Utilities.h
        #        ../Common/SceneManager.cpp
        #        ../Common/SceneManager.h
        engine/graphic_misc/OBJReader.cpp
        engine/graphic_misc/OBJReader.h
        engine/graphic_misc/Mesh.cpp
        engine/graphic_misc/Mesh.h
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
        input/KeyCodes.h
        input/InputManager.cpp
        input/InputManager.h
        engine/Engine.cpp
        engine/Engine.h
        engine/graphic_misc/Color.h
        engine/scene/SceneBase.h
        object_components/ComponentBase.h
        object_components/ComponentManager.h
        object_components/ComponentManager.inl
        engine/scene/TestScene.h
        engine/scene_objects/Object.cpp
        engine/scene_objects/Object.h
        engine/scene_objects/Camera.cpp
        engine/scene_objects/Camera.h
        engine/scene/SceneBase.cpp
        engine/scene_objects/Light.h
        #GUI stuffs begin
        engine/GUI/GUIManager.h
        engine/GUI/GUIManager.cpp
        engine/GUI/GUIObject.h
        engine/GUI/GUIWindow.cpp
        engine/GUI/GUIWindow.h
        engine/GUI/GUIContent.h
        engine/GUI/CurrentCameraInfoContent.h
        engine/GUI/CurrentCameraInfoContent.cpp
        engine/GUI/EngineInfo.cpp
        engine/GUI/EngineInfo.h
        engine/GUI/ObjectListContent.cpp
        engine/GUI/ObjectListContent.h
        #GUI stuffs end

        engine/GUI/ColorSet.h
        engine/GUI/ObjectDetailContent.cpp
        engine/GUI/ObjectDetailContent.h
        engine/graphic_misc/VAOManager.cpp
        engine/graphic_misc/VAOManager.h
        engine/graphic_misc/VBOManager.cpp
        engine/graphic_misc/VBOManager.h
        engine/graphic_misc/UBO.cpp
        engine/graphic_misc/UBO.h
        engine/GUI/LightListContent.cpp
        engine/GUI/LightListContent.h
        engine/GUI/LightDetailContent.cpp
        engine/GUI/LightDetailContent.h
        engine/graphic_misc/Environment.h
        engine/graphic_misc/TextureManager.cpp
        engine/graphic_misc/TextureManager.h
        engine/graphic_misc/TextureObject.cpp
        engine/graphic_misc/TextureObject.h
        engine/GUI/EngineInfoContent.cpp engine/GUI/EngineInfoContent.h engine/scene/TestScene.cpp engine/graphic_misc/FBO.h engine/graphic_misc/FBO.cpp)

# Set target properties
target_include_directories( ${TARGET_NAME} PUBLIC ${OPENGL_INCLUDE_DIR} )
target_include_directories( ${TARGET_NAME} PUBLIC ${GLFW_INCLUDE_DIRS} )
target_include_directories( ${TARGET_NAME} PUBLIC "../Common/" )
target_include_directories( ${TARGET_NAME} PUBLIC "data/" )
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