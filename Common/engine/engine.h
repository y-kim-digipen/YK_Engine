/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: engine.h
Purpose: Header file for engine
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS350_1
Author: Yoonki Kim, y.kim,  180002421
Creation date: Feb 6, 2022
End Header --------------------------------------------------------*/
#ifndef SIMPLE_SCENE_ENGINE_H
#define SIMPLE_SCENE_ENGINE_H

#endif //SIMPLE_SCENE_ENGINE_H

#include <glm/glm.hpp>

#include <string>
#include <memory>
#include <vector>
#include "object_components/ComponentManager.h"
#include <GL/glew.h>


#include "GUI/GUIManager.h"

#include "engine/graphic_misc/Color.h"
#include "engine/graphic_misc/Mesh.h"
#include "engine/scene_objects/MeshObject.h"
#include "engine/graphic_misc/Shader.h"
#include "engine/graphic_misc/VAOManager.h"
#include "engine/graphic_misc/VBOManager.h"
#include "engine/graphic_misc/UBO.h"
#include "engine/graphic_misc/Environment.h"
#include "engine/graphic_misc/TextureManager.h"
#include "engine/graphic_misc/FBO.h"

#define ENGINE_SUPPORT_MAX_LIGHTS 16 /*Should be equal to shader max light variable*/
#define DPI  1.8f;
class SceneBase;
class GLFWwindow;

class engine
{
    friend GUI::GUI_Manager;
public:
    engine();

    //todo check it is useful, if not delete this function
    //static engine* GetInstance();
    static int InitWindow(glm::vec2 win_size, const std::string& title_name);

    //Initializing engine features except windows
    static void InitEngine();
    static void Update();
    static void CleanUp();

    static void ShutDown();

    static void SetClearColor(Color newClearColor);

    static bool IsRunning();

    static glm::vec2 GetWindowSize();

    static std::shared_ptr<Mesh> GetMesh(const std::string& meshStr);
    static std::shared_ptr<Shader> GetShader(const std::string& shaderStr);
    static GUI::GUI_Manager& GetGUIManager();
    static ComponentManager<Mesh>& GetMeshManager();
    static ComponentManager<Shader>& GetShaderManager();

    static VAOManager& GetVAOManager();
    static VBOManager& GetVBOManager();
    static TextureManager& GetTextureManager();

    static SceneBase* GetCurrentScene();
    static std::string GetTitleName();
    static float GetFPS();

    static void SkipFrame(int Frames);
private:
    static void PreRender();
    static void RenderToGBuffer();
    static void Render();
    static void PostRender();

    static void SetupFBO();
    static void SetupScenes();
    static void SetupShaders();
    static void SetupMeshes();

    static void SetupTextures();

    static void GLFWErrorCallback(int, const char* err_str);
    static void KeyboardInputCallback(GLFWwindow*, int key, [[maybe_unused]] int keyCode, int action, [[maybe_unused]] int modifier);
    //static void MouseButtonCallback(GLFWwindow*, int button, int action, [[maybe_unused]] int modifier);
    //static void MouseScrollCallback(GLFWwindow*, double, double yOffset)
private:
    inline static std::string mTitleStr;
    inline static GLFWwindow* m_pWindow;
    inline static glm::vec2 mWinSize;
    inline static Color mClearColor;

    inline static std::vector<SceneBase*> m_pScenes;

    inline static short mFocusedSceneIdx;

    inline static ComponentManager<Mesh> mMeshManager;
    inline static ComponentManager<Shader> mShaderManager;
    inline static ComponentManager<MeshObject> mDebugDrawObjectsManager;
    inline static TextureManager mTextureManager;

    inline static VAOManager mVAOManager;
    inline static VBOManager mVBOManager;

    inline static GUI::GUI_Manager mGUIManager;
    inline static float FPS;

    inline static int TargetFPS = 30;

    inline static UBO lightUBO;
    inline static UBO environmentUBO;

    inline static glm::vec3 GlobalAmbientColor = glm::vec3(0.3f);
    inline static glm::vec3 FogColor = glm::vec3(0.f);

    inline static int leftSkipFrames = 0;

public:
    inline static FBO gBuffer;
    inline static FBO renderBuffer;
    inline static FBO reflectionBuffer;

    inline static FBO finalRenderBuffer;
};
