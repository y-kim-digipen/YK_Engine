/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Engine.cpp
Purpose: Source file for Engine
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS300_2
Author: Yoonki Kim, y.kim,  180002421
Creation date: Nov 7, 2021
End Header --------------------------------------------------------*/
#include "Engine.h"
#include <iostream>
#include <chrono>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <thread>
#include <GUI/LightListContent.h>

#include "Input/InputManager.h"
//todo delete this after testing scene
#include "TestScene.h"
#include "OBJReader.h"


//GUI things
#include "GUI/GUIWindow.h"
#include "GUI/CurrentCameraInfoContent.h"
#include "GUI/ObjectListContent.h"
#include "GUI/EngineInfoContent.h"
#include "Environment.h"


static auto CurrentTime = std::chrono::system_clock::now();
static auto LastTime = CurrentTime;

Engine::Engine() {
    mClearColor = Color(0.f);
    mWinSize = glm::vec2{};
}

int Engine::InitWindow(glm::vec2 win_size, const std::string& title_name) {
    mTitleStr = title_name;
    mWinSize = win_size * DPI;

    //GLFW settings
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

    glfwWindowHint(GLFW_SAMPLES, 1); // change for anti-aliasing
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_RED_BITS, 8);
    glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8);
    glfwWindowHint(GLFW_ALPHA_BITS, 8);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glfwInit();

    m_pWindow = glfwCreateWindow(static_cast<int>(mWinSize.x), static_cast<int>(mWinSize.y), mTitleStr.c_str(), nullptr, nullptr);
    m_pGUIWindow = glfwCreateWindow(static_cast<int>(mWinSize.x * 0.5f), static_cast<int>(mWinSize.y), "GUIWindow", nullptr, nullptr);
    if(m_pWindow == nullptr) {
        std::cerr << "Failed to open GLFW window. Check if your GPU is compatible." << std::endl;
        glfwTerminate();
        return -1;
    }
    //Set up callback functions
    glfwSetErrorCallback(&Engine::GLFWErrorCallback);
    glfwSetKeyCallback(m_pWindow, &Engine::KeyboardInputCallback);

    // OpenGL resource model - "glfwCreateWindow" creates the necessary data storage for the OpenGL
    // context but does NOT make the created context "current". We MUST make it current with the following
    // call
    glfwMakeContextCurrent(m_pWindow);

    // Initialize GLEW
    glewExperimental = static_cast<GLboolean>(true); // Needed for core profile
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW." << std::endl;
        glfwTerminate();
        return -1;
    }
    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(m_pWindow, GLFW_STICKY_KEYS, GL_TRUE);
    return 0;
}

void Engine::SetClearColor(Color newClearColor) {
    mClearColor = newClearColor;
    glClearColor(mClearColor.r, mClearColor.g, mClearColor.b, mClearColor.a);
}

void Engine::InitEngine() {
    OBJReader objReader;
    mFocusedSceneIdx = -1;
    InputManager::Init();
    mGUIManager.Init(m_pGUIWindow);

    SetupShaders();
    SetupMeshes();
    SetupTextures();
    //todo for now, it is for debugging

    SetupScenes();

    mFocusedSceneIdx = 0;
    SetClearColor(Color(0.0f));
    //debug part ended here

    for(auto& scene : m_pScenes){
        scene->Init();
    }
    SetupGUI();

    lightUBO.createUBO(ENGINE_SUPPORT_MAX_LIGHTS * sizeof(Light::std140_structure) + sizeof(int));
    lightUBO.bindBufferBaseToBindingPoint(1);

    environmentUBO.createUBO(sizeof(Environment::std140_structure));
    environmentUBO.bindBufferBaseToBindingPoint(2);

    std::cout << "Engine is initialized, ready to update" << std::endl;
}

void Engine::Update() {
//    const static float TargetDeltaTime = 1.f / FPS;
    auto deltaTime = std::chrono::duration_cast<std::chrono::duration<float>>((CurrentTime - LastTime)).count();
    //todo set FPS limit
//    if(deltaTime < TargetDeltaTime){
//        std::chrono::duration_cast<std::chrono::seconds>(TargetDeltaTime - deltaTime);
//        std::this_thread::sleep_for();
//
//        deltaTime = std::chrono::duration_cast<std::chrono::duration<float>>((CurrentTime - LastTime)).count();
//    }

    LastTime = CurrentTime;
    CurrentTime = std::chrono::system_clock::now();
    FPS = 1.f / deltaTime;

    InputManager::Update();
    //why it had to be called every frame?
//    glfwMakeContextCurrent(m_pWindow);

    SwapToMainWindow();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    if(mFocusedSceneIdx >= 0){
//        std::cout << leftSkipFrames << std::endl;
        if(leftSkipFrames > 0)
        {
            leftSkipFrames--;
        }
        else
        {
            PreRender();
            Render();
            PostRender();
        }
    }
    glfwSwapBuffers(m_pWindow);

    SwapToGUIWindow();
//    glfwMakeContextCurrent(m_pGUIWindow);
//
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    mGUIManager.Update();
    glfwSwapBuffers(m_pGUIWindow);

    glfwPollEvents();
}

void Engine::CleanUp() {
    //delete GetInstance();
    mGUIManager.CleanUp();
    mShaderManager.Cleanup();
    mMeshManager.Cleanup();
    for(auto pScene : m_pScenes){
        pScene->CleanUp();
        delete pScene;
        pScene = nullptr;
    }
    glfwTerminate();
}

std::shared_ptr<Mesh> Engine::GetMesh(const std::string &meshStr) {
    return mMeshManager.GetComponent(meshStr);
}

std::shared_ptr<Shader> Engine::GetShader(const std::string &shaderStr) {
    return mShaderManager.GetComponent(shaderStr);;
}

bool Engine::IsRunning() {
    //todo change glfwGetKey to my Input system
    return glfwGetKey(m_pWindow, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
    glfwWindowShouldClose(m_pWindow) == 0;
}

void Engine::PreRender() {
    lightUBO.bindUBO();

    GLsizeiptr offset = 0;
    GLint structureSize = Light::GetSTD140StructureSize();

    for (const auto& pointLight : GetCurrentScene()->GetLightList())
    {
        lightUBO.setBufferData(offset, pointLight.second->GetSTD140Structure(), structureSize);
        offset += structureSize;
    }

    GLint numActiveLights = GetCurrentScene()->GetLightList().size();
    lightUBO.setBufferData(structureSize * ENGINE_SUPPORT_MAX_LIGHTS, (void*)&numActiveLights, sizeof(int));

    environmentUBO.bindUBO();
//    GetCurrentScene()->GetEnvironment().std140_structure.I_Fog = mClearColor.AsVec3() * 256.f;
    GetCurrentScene()->GetEnvironment().std140_structure.zFar = GetCurrentScene()->GetCurrentCamera()->FarDistance();
    GetCurrentScene()->GetEnvironment().std140_structure.zNear = GetCurrentScene()->GetCurrentCamera()->NearDistance();
    GetCurrentScene()->GetEnvironment().std140_structure.GlobalAmbient = GlobalAmbientColor * 256.f;
    GetCurrentScene()->GetEnvironment().std140_structure.I_Fog = FogColor * 256.f;
    environmentUBO.setBufferData(0, GetCurrentScene()->GetEnvironment().GetSTD140Structure(), sizeof(Environment::std140));

    m_pScenes[mFocusedSceneIdx]->PreRender();
}

void Engine::Render() {
    m_pScenes[mFocusedSceneIdx]->Render();
}

void Engine::PostRender() {
    m_pScenes[mFocusedSceneIdx]->PostRender();
}

void Engine::GLFWErrorCallback([[maybe_unused]]int, const char *err_str) {
    std::cerr << "GLFW Error: " << err_str << std::endl;
}

void Engine::KeyboardInputCallback(GLFWwindow *, [[maybe_unused]] int key, [[maybe_unused]]int keyCode, int action, [[maybe_unused]] int modifier) {
    if (key < 0 || key > 1000)
    {
        return;
    }

    if (GLFW_PRESS == action)
    {
        InputManager::on_key_pressed(key);
    }
    else if (GLFW_REPEAT == action)
    {

    }
    else if (GLFW_RELEASE == action)
    {
        InputManager::on_key_released(key);
    }
}

glm::vec2 Engine::GetWindowSize() {
    return mWinSize;
}

GUI::GUI_Manager &Engine::GetGUIManager() {
    return mGUIManager;
}

void Engine::SetupScenes() {
    SceneBase* baseScene = new TestScene();
    baseScene->AddCamera();
    baseScene->GetCurrentCamera()->SetPosition(glm::vec3(0.f, 1.f, 5.f));
    baseScene->GetCurrentCamera()->Pitch(-HALF_PI * 0.2f);
    m_pScenes.emplace_back(baseScene);
}

void Engine::SetupShaders() {
    auto pShader = mShaderManager.AddComponent("TestShader", new Shader("TestShader"));

    pShader->CreateProgramAndLoadCompileAttachLinkShaders({
                                                                  {GL_VERTEX_SHADER,"../shaders/QuadVertexShader.vert"},
                                                                  {GL_FRAGMENT_SHADER,"../shaders/QuadFragmentShader.frag"} });

    pShader = mShaderManager.AddComponent("3D_DefaultShader", new Shader("3D_DefaultShader"));

    pShader->CreateProgramAndLoadCompileAttachLinkShaders({
                                                                  {GL_VERTEX_SHADER,"../shaders/SimpleVertexShader.vert"},
                                                                  {GL_FRAGMENT_SHADER,"../shaders/SimpleFragmentShader.frag"} });

    pShader = mShaderManager.AddComponent("DiffuseShader", new Shader("DiffuseShader"));

    pShader->CreateProgramAndLoadCompileAttachLinkShaders({
                                                                  {GL_VERTEX_SHADER,"../shaders/DiffuseShader.vert"},
                                                                  {GL_FRAGMENT_SHADER,"../shaders/DiffuseShader.frag"} });

    pShader = mShaderManager.AddComponent("NormalDrawShader", new Shader("NormalDrawShader"), false);
    pShader->CreateProgramAndLoadCompileAttachLinkShaders({
                                                                  {GL_VERTEX_SHADER,"../shaders/SimpleLineVertexShader.vert"},
                                                                  {GL_FRAGMENT_SHADER,"../shaders/SimpleLineFragmentShader.frag"} });

    pShader = mShaderManager.AddComponent("FaceNormalDrawShader", new Shader("FaceNormalDrawShader"), false);
    pShader->CreateProgramAndLoadCompileAttachLinkShaders({
                                                                  {GL_VERTEX_SHADER,"../shaders/FaceNormalVertexShader.vert"},
                                                                  {GL_FRAGMENT_SHADER,"../shaders/FaceNormalFragmentShader.frag"} });

    pShader = mShaderManager.AddComponent("PhongLighting", new Shader("PhongLighting"), true);
    pShader->CreateProgramAndLoadCompileAttachLinkShaders({
                                                                  {GL_VERTEX_SHADER,"../shaders/PhongLighting.vert"},
                                                                  {GL_FRAGMENT_SHADER,"../shaders/PhongLighting.frag"} });

    pShader = mShaderManager.AddComponent("PhongShader", new Shader("PhongShader"), true);
    pShader->CreateProgramAndLoadCompileAttachLinkShaders({
                                                                  {GL_VERTEX_SHADER,"../shaders/PhongShading.vert"},
                                                                  {GL_FRAGMENT_SHADER,"../shaders/PhongShading.frag"} });

    pShader = mShaderManager.AddComponent("Blinn-Phong Shader", new Shader("Blinn-Phong Shader"), true);
    pShader->CreateProgramAndLoadCompileAttachLinkShaders({
                                                                  {GL_VERTEX_SHADER,"../shaders/Blinn-PhongShading.vert"},
                                                                  {GL_FRAGMENT_SHADER,"../shaders/Blinn-PhongShading.frag"} });
    pShader->bindUniformBlockToBindingPoint("LightBlock", 1);
}

void Engine::SetupMeshes() {
    OBJReader objReader;



    auto pMesh = mMeshManager.AddComponent("Bunny", std::make_shared<Mesh>("Bunny"));
    pMesh->ClearData();
    objReader.ReadOBJFile("../models/bunny.obj", pMesh.get());
    pMesh->Init();

    pMesh = mMeshManager.AddComponent("Lucy", std::make_shared<Mesh>("Lucy"));
    pMesh->ClearData();
    objReader.ReadOBJFile("../models/lucy_princeton.obj", pMesh.get());
    pMesh->Init();

    pMesh = mMeshManager.AddComponent("Cube", std::make_shared<Mesh>("Cube"));
    pMesh->ClearData();
    objReader.ReadOBJFile("../models/cube2.obj", pMesh.get());
    pMesh->Init();

    pMesh = mMeshManager.AddComponent("Sphere", std::make_shared<Mesh>("Sphere"));
    pMesh->ClearData();
    objReader.ReadOBJFile("../models/sphere.obj", pMesh.get());
    pMesh->Init();

    pMesh = mMeshManager.AddComponent("SphereModified", std::make_shared<Mesh>("SphereModified"));
    pMesh->ClearData();
    objReader.ReadOBJFile("../models/sphere_modified.obj", pMesh.get());
    pMesh->Init();

//    pMesh = mMeshManager.AddComponent("Rhino", std::make_shared<Mesh>("Rhino"));
//    pMesh->ClearData();
//    objReader.ReadOBJFile("../models/rhino.obj", pMesh.get());
//    pMesh->Init();

    pMesh = mMeshManager.AddComponent("Cup", std::make_shared<Mesh>("Cup"));
    pMesh->ClearData();
    objReader.ReadOBJFile("../models/cup.obj", pMesh.get());
    pMesh->Init();

    pMesh = mMeshManager.AddComponent("StarWars", std::make_shared<Mesh>("StarWars"));
    pMesh->ClearData();
    objReader.ReadOBJFile("../models/starwars1.obj", pMesh.get());
    pMesh->Init();

    pMesh = mMeshManager.AddComponent("4Sphere", std::make_shared<Mesh>("4Sphere"));
    pMesh->ClearData();
    objReader.ReadOBJFile("../models/4Sphere.obj", pMesh.get());
    pMesh->Init();

    pMesh = mMeshManager.AddComponent("ProceduralSphere", std::make_shared<Mesh>("ProceduralSphere"));
    pMesh->ClearData();
    pMesh->MakeProcedural(Mesh::ProceduralMeshType::SPHERE, 20, 20);
    pMesh->Init();

    pMesh = mMeshManager.AddComponent("Plane", std::make_shared<Mesh>("Plane"));
    pMesh->ClearData();
    objReader.ReadOBJFile("../models/triangle.obj", pMesh.get());
    pMesh->Init();

    pMesh = mMeshManager.AddComponent("Bulb", std::make_shared<Mesh>("Bulb"));
    pMesh->ClearData();
    objReader.ReadOBJFile("../models/cube2.obj", pMesh.get());
    pMesh->Init();

}

SceneBase* Engine::GetCurrentScene() {
    return m_pScenes[mFocusedSceneIdx];
}

void Engine::SetupGUI() {
    using namespace GUI;
//    auto pGUIWindow = mGUIManager.AddWindow("Camera Settings");
//    pGUIWindow->AddContent("Position", new CurrentCameraInfoContent());
    auto pGUIWindow = mGUIManager.AddWindow("Object Lists");
    pGUIWindow->AddFlag(ImGuiWindowFlags_AlwaysAutoResize);
    pGUIWindow->AddContent("Object Lists", new ObjectListContent());

    pGUIWindow = mGUIManager.AddWindow("Light Lists");
    pGUIWindow->AddFlag(ImGuiWindowFlags_AlwaysAutoResize);
    pGUIWindow->AddContent("Light Lists", new LightListContent());

    pGUIWindow = mGUIManager.AddWindow("Global Setting");
    pGUIWindow->AddFlag(ImGuiWindowFlags_AlwaysAutoResize);
    pGUIWindow->AddContent("Engine Info", new EngineInfoContent());
}

std::string Engine::GetTitleName() {
    return mTitleStr;
}

float Engine::GetFPS() {
    return FPS;
}

ComponentManager<Mesh> &Engine::GetMeshManager() {
    return mMeshManager;
}

ComponentManager<Shader> &Engine::GetShaderManager() {
    return mShaderManager;
}

VAOManager &Engine::GetVAOManager() {
    return mVAOManager;
}

VBOManager &Engine::GetVBOManager() {
    return mVBOManager;
}

TextureManager &Engine::GetTextureManager() {
    return mTextureManager;
}

void Engine::SetupTextures() {
    mTextureManager.CreateTextureFromFile("../textures/metal_roof_diff_512x512.png", "tex_object0", GL_TEXTURE_2D, 0);
    mTextureManager.CreateTextureFromFile("../textures/metal_roof_spec_512x512.png", "tex_object1", GL_TEXTURE_2D, 1);
}

void Engine::SkipFrame(int Frames) {
    leftSkipFrames += Frames + 1;
}

void Engine::SwapToMainWindow() {
    glfwMakeContextCurrent(m_pWindow);
//    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_CULL_FACE);
//    glCullFace(GL_BACK);
//
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

void Engine::SwapToGUIWindow() {
    glfwMakeContextCurrent(m_pGUIWindow);

//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}







