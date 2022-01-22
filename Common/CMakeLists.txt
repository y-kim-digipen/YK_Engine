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
        engine/common_constants.h
        ../Common/engine/graphic_misc/Shader.cpp
        ../Common/engine/graphic_misc/Shader.h

        ../Common/engine/graphic_misc/OBJReader.cpp
        ../Common/engine/graphic_misc/OBJReader.h
        ../Common/engine/graphic_misc/Mesh.cpp
        ../Common/engine/graphic_misc/Mesh.h

        ../Common/engine/input/KeyCodes.h
        ../Common/engine/input/InputManager.cpp
        ../Common/engine/input/InputManager.h
        engine/engine.cpp
        engine/engine.h
        ../Common/engine/graphic_misc/Color.h
        ../Common/engine/scene/SceneBase.h
        ../Common/object_components/ComponentBase.h
        ../Common/object_components/ComponentManager.h
        ../Common/object_components/ComponentManager.inl
        ../Common/engine/scene/TestScene.h
        ../Common/engine/scene_objects/Object.cpp
        ../Common/engine/scene_objects/Object.h
        ../Common/engine/scene_objects/Camera.cpp
        ../Common/engine/scene_objects/Camera.h
        ../Common/engine/scene/SceneBase.cpp
        ../Common/engine/scene_objects/Light.h

        ../Common/engine/GUI/ColorSet.h
        ../Common/engine/GUI/ObjectDetailContent.cpp
        ../Common/engine/GUI/ObjectDetailContent.h
        ../Common/engine/graphic_misc/VAOManager.cpp
        ../Common/engine/graphic_misc/VAOManager.h
        ../Common/engine/graphic_misc/VBOManager.cpp
        ../Common/engine/graphic_misc/VBOManager.h
        ../Common/engine/graphic_misc/UBO.cpp
        ../Common/engine/graphic_misc/UBO.h

        ../Common/engine/graphic_misc/Environment.h
        ../Common/engine/graphic_misc/TextureManager.cpp
        ../Common/engine/graphic_misc/TextureManager.h
        ../Common/engine/graphic_misc/TextureObject.cpp
        ../Common/engine/graphic_misc/TextureObject.h

        ../Common/engine/scene/TestScene.cpp
        ../Common/engine/graphic_misc/FBO.h
        ../Common/engine/graphic_misc/FBO.cpp

        #GUI
        ../Common/engine/GUI/EngineInfoContent.cpp
        ../Common/engine/GUI/EngineInfoContent.h
        ../Common/engine/GUI/LightListContent.cpp
        ../Common/engine/GUI/LightListContent.h
        ../Common/engine/GUI/LightDetailContent.cpp
        ../Common/engine/GUI/LightDetailContent.h
        ../Common/engine/GUI/GUIManager.h
        ../Common/engine/GUI/GUIManager.cpp
        ../Common/engine/GUI/GUIObject.h
        ../Common/engine/GUI/GUIWindow.cpp
        ../Common/engine/GUI/GUIWindow.h
        ../Common/engine/GUI/GUIContent.h
        ../Common/engine/GUI/CurrentCameraInfoContent.h
        ../Common/engine/GUI/CurrentCameraInfoContent.cpp
        ../Common/engine/GUI/EngineInfo.cpp
        ../Common/engine/GUI/EngineInfo.h
        ../Common/engine/GUI/ObjectListContent.cpp
        ../Common/engine/GUI/ObjectListContent.h
        )

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