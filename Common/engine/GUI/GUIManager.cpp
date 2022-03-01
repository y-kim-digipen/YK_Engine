/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: GUIManager.cpp
Purpose: Source file for GUIManager
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS350_1
Author: Yoonki Kim, y.kim,  180002421
Creation date: Feb 6, 2022
End Header --------------------------------------------------------*/

#include "GUIManager.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "engine/engine.h"


#include "engine/scene/SceneBase.h"
#include "ColorSet.h"

#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#include <random>
#include "engine/graphic_misc/BoundingVolume/BasicBoundingVolumes.h"

namespace GUI {
    void GUI_Manager::Init(GLFWwindow *m_pWindow) {
        p_glfwWindow = m_pWindow;
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void) io;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls

        float highDPIscaleFactor = DPI;

        io.Fonts->AddFontFromFileTTF("../../External/Font/Meera-Regular.ttf", 25.0f * highDPIscaleFactor, NULL, NULL);

        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(m_pWindow, true);
        ImGui_ImplOpenGL3_Init("#version 410");

        show_demo_window = false;
        textureNames.resize(4);
        textureNames[0] = engine::renderBuffer.GetAttachmentTextureName(GL_COLOR_ATTACHMENT0);
    }

    void GUI_Manager::Update() {
        PreRender();
        Render();
        PostRender();
    }

    void GUI_Manager::CleanUp() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void GUI_Manager::PreRender() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        RenderDockingWindow();
    }

    void GUI_Manager::Render() {
        if (show_demo_window) {
            ImGui::ShowDemoWindow(&show_demo_window);
        }
        if (show_object_list) {
            RenderObjectList();
        }
        if (show_light_list) {
            RenderLightList();
        }

        if (show_texture_0) {
            RenderTextureWindow("Slot0", textureNames[0], &show_texture_0);
        }
        if (show_texture_1) {
            RenderTextureWindow("Slot1", textureNames[1], &show_texture_1);
        }
        if (show_texture_2) {
            RenderTextureWindow("Slot2", textureNames[2], &show_texture_2);
        }
        if (show_texture_3) {
            RenderTextureWindow("Slot3", textureNames[3], &show_texture_3);
        }

        if (show_assignment_detail) {
            RenderAssignmentDetail();
        }
    }

    void GUI_Manager::PostRender() {
        ImGui::Render();
        int display_w, display_h;

        glfwGetFramebufferSize(p_glfwWindow, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
//        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
//        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
        ImGuiIO &io = ImGui::GetIO();
        (void) io;
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow *backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }

    void GUI_Manager::RenderDockingWindow() {
        // If you strip some features of, this demo is pretty much equivalent to calling DockSpaceOverViewport()!
        // In most cases you should be able to just call DockSpaceOverViewport() and ignore all the code below!
        // In this specific demo, we are not using DockSpaceOverViewport() because:
        // - we allow the host window to be floating/moveable instead of filling the viewport (when opt_fullscreen == false)
        // - we allow the host window to have padding (when opt_padding == true)
        // - we have a local menu bar in the host window (vs. you could use BeginMainMenuBar() + DockSpaceOverViewport() in your code!)
        // TL;DR; this demo is more complicated than what you would normally use.
        // If we removed all the options we are showcasing, this demo would become:
        //     void ShowExampleAppDockSpace()
        //     {
        //         ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
        //     }

        static bool opt_fullscreen = true;
        static bool opt_padding = false;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen) {
            const ImGuiViewport *viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                            ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        } else {
            dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
        }

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
        // and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        if (!opt_padding)
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        bool alwaysTrue = true;
        ImGui::Begin("DockSpace Demo", &alwaysTrue, window_flags);
        if (!opt_padding)
            ImGui::PopStyleVar();

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        // Submit the DockSpace
        ImGuiIO &io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        } else {
//            ShowDockingDisabledMessage();
        }

        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("Options")) {
                // Disabling fullscreen would allow the window to be moved to the front of other windows,
                // which we can't undo at the moment without finer window depth/z control.
                ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
                ImGui::MenuItem("Padding", NULL, &opt_padding);
                ImGui::Separator();

                if (ImGui::MenuItem("Flag: NoSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoSplit) !=
                                                         0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoSplit; }
                if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) !=
                                                          0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoResize; }
                if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "",
                                    (dockspace_flags & ImGuiDockNodeFlags_NoDockingInCentralNode) !=
                                    0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode; }
                if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) !=
                                                                0)) { dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar; }
                if (ImGui::MenuItem("Flag: PassthruCentralNode", "",
                                    (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0,
                                    opt_fullscreen)) { dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode; }
                ImGui::Separator();

//                if (ImGui::MenuItem("Close", NULL, false, p_open != NULL))
//                    *p_open = false;
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Windows")) {
                if (ImGui::BeginMenu("List")) {
                    ImGui::MenuItem("MeshObject", NULL, &show_object_list);
                    ImGui::MenuItem("Light", NULL, &show_light_list);
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Render")) {
                    ImGui::MenuItem("Slot0", NULL, &show_texture_0);
                    ImGui::MenuItem("Slot1", NULL, &show_texture_1);
                    ImGui::MenuItem("Slot2", NULL, &show_texture_2);
                    ImGui::MenuItem("Slot3", NULL, &show_texture_3);
//                    auto pWindow = AddWindow("Hello" + std::to_string(m_pWindows.size()));
//                    pWindow->AddContent("TextureContent", new TextureContent);

                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("others")) {
                    ImGui::MenuItem("Assignment Detail", NULL, &show_assignment_detail);
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }

//            HelpMarker(            ImGui::SetNextItemWidth(120);
//                    "When docking is enabled, you can ALWAYS dock MOST window into another! Try it now!" "\n"
//                    "- Drag from window title bar or their tab to dock/undock." "\n"
//                    "- Drag from window menu button (upper-left button) to undock an entire node (all windows)." "\n"
//                    "- Hold SHIFT to disable docking (if io.ConfigDockingWithShift == false, default)" "\n"
//                    "- Hold SHIFT to enable docking (if io.ConfigDockingWithShift == true)" "\n"
//                    "This demo app has nothing to do with enabling docking!" "\n\n"
//                    "This demo app only demonstrate the use of ImGui::DockSpace() which allows you to manually create a docking node _within_ another window." "\n\n"
//                    "Read comments in ShowExampleAppDockSpace() for more details.");

            ImGui::EndMenuBar();
        }

        ImGui::End();
    }

    void GUI_Manager::RenderObjectList() {

        ImGui::Begin("Objects", &show_object_list);
        {
            static float sliderSpeed = 0.1f;
            ImGui::InputFloat("SliderSpeed", (float *) &sliderSpeed);
            auto pCurrentScene = engine::GetCurrentScene();
            if (pCurrentScene == nullptr) {
                return;
            }
            auto &ObjList = pCurrentScene->GetObjectList();
            const auto &meshList = engine::GetMeshManager().GetNameList();

            for (auto &obj_itr: ObjList) {
                auto &objNameStr = obj_itr.first;
                ImGui::PushID(objNameStr.c_str());
                if (ImGui::CollapsingHeader(objNameStr.c_str())) {

                    if(obj_itr.second->IsMeshType())
                    {
                        MeshObject* currentObject = static_cast<MeshObject*>(obj_itr.second);
                        if (ImGui::TreeNode("Transform")) {
                            ImGui::DragFloat3("Position", (float *) &currentObject->m_position, sliderSpeed);
                            ImGui::DragFloat3("Scale", (float *) &currentObject->m_scale, sliderSpeed);
                            ImGui::DragFloat3("Rotation", (float *) &currentObject->m_rotation, sliderSpeed);
                            ImGui::TreePop();
                        }

                        if (ImGui::TreeNode("Material")) {
                            static float sliderSpeed = 0.01f;
                            ImGui::InputFloat("SliderSpeed", (float *) &sliderSpeed);
                            ImGui::ColorEdit3("baseColor", (float *) &currentObject->baseColor);
                            ImGui::DragFloat("metallic", (float *) &currentObject->metallic, sliderSpeed, 0.f, 1.f);
                            ImGui::DragFloat
                                    ("roughness", (float *) &currentObject->roughness, sliderSpeed, 0.f, 1.f);
                            ImGui::TreePop();
                        }

                        if (ImGui::TreeNode("Mesh")) {
                            const std::string usingMeshStr = currentObject->GetUsingMeshName();
                            if (ImGui::BeginCombo("MeshList", usingMeshStr.c_str())) {
                                for (const auto &meshName: meshList) {
                                    bool isSelected = (usingMeshStr == meshName);
                                    if (ImGui::Selectable(meshName.c_str(), isSelected)) {
                                        currentObject->SetMesh(meshName);
                                    }
                                    if (isSelected) {
                                        ImGui::SetItemDefaultFocus();
                                    }
                                }
                                ImGui::EndCombo();
                            }
                            ImGui::TreePop();
                        }

                        if (ImGui::TreeNode("Others")) {
                            if (ImGui::Checkbox("VertexNormalDrawing", &currentObject->mDoVertexNormalDrawing)) {
                                if (currentObject->mDoFaceNormalDrawing) {
                                    currentObject->mDoVertexNormalDrawing = false;
                                }
                            }
                            if (ImGui::Checkbox("FaceNormalDrawing", &currentObject->mDoFaceNormalDrawing)) {
                                if (currentObject->mDoVertexNormalDrawing) {
                                    currentObject->mDoFaceNormalDrawing = false;
                                }
                            }
                            ImGui::TreePop();
                        }
                    }
                    else if(obj_itr.second->IsPrimitiveType())
                    {
                        if (ImGui::TreeNode("Primitives")) {
                            Collider *primitiveObjectCollider = obj_itr.second->GetCollider();
                            switch (primitiveObjectCollider->GetColliderType()) {
                                case ColliderTypes::TRIANGLE: {
                                    Triangle *thisTriangle = static_cast<Triangle *>(primitiveObjectCollider);
                                    ImGui::DragFloat3("p0", (float *) &thisTriangle->mPoints[0], sliderSpeed);
                                    ImGui::DragFloat3("p1", (float *) &thisTriangle->mPoints[1], sliderSpeed);
                                    ImGui::DragFloat3("p2", (float *) &thisTriangle->mPoints[2], sliderSpeed);
                                    break;
                                }
                                case ColliderTypes::PLANE: {
                                    Plane *thisPlane = static_cast<Plane *>(primitiveObjectCollider);
                                    ImGui::DragFloat3("normal", (float *) &thisPlane->m_Normal, sliderSpeed);
                                    ImGui::DragFloat("D", (float *) &thisPlane->m_Normal.w, sliderSpeed);
                                    ImGui::DragFloat("scale", (float *) &thisPlane->planeScale, sliderSpeed, 1.f, FLT_MAX);
                                    break;
                                }
                                case ColliderTypes::POINT3D: {
                                    Point3D *thisPoint = static_cast<Point3D *>(primitiveObjectCollider);
                                    ImGui::DragFloat3("p0", (float *) &thisPoint->mCoordinate, sliderSpeed);
                                    break;
                                }
                                case ColliderTypes::RAY: {
                                    Ray *thisRay = static_cast<Ray*>(primitiveObjectCollider);
                                    ImGui::DragFloat3("start point", (float *) &thisRay->mStart, sliderSpeed);
                                    ImGui::DragFloat3("direction", (float *) &thisRay->mDir, sliderSpeed);
                                    ImGui::DragFloat("scale", (float *) &thisRay->lengthDir, sliderSpeed, 1.f, FLT_MAX);
                                    break;
                                }
                            }
                            ImGui::TreePop();
                        }
                    }
                }
                ImGui::PopID();
            }
        }
        ImGui::End();
    }

    void GUI_Manager::RenderLightList() {
        ImGui::Begin("Lights", &show_light_list);
        {
            static std::vector<std::string> lightTypeStr = {"PointLight"/*, "DirectionalLight"*/, "SpotLight"};
            auto pCurrentScene = engine::GetCurrentScene();
            if (pCurrentScene == nullptr) {
                return;
            }
            auto &LightList = pCurrentScene->GetLightList();

            for (auto &light_itr: LightList) {
                auto &lightNameStr = light_itr.first;
                ImGui::PushID(lightNameStr.c_str());
                if (ImGui::CollapsingHeader(lightNameStr.c_str())) {
                    auto currentLight = light_itr.second;
                    if (ImGui::TreeNode("Transforms")) {
                        static float sliderSpeed = 0.1f;
                        ImGui::InputFloat("SliderSpeed", (float *) &sliderSpeed);
                        ImGui::DragFloat3("Position", (float *) &currentLight->m_position, sliderSpeed);
                        ImGui::DragFloat3("Scale", (float *) &currentLight->m_scale, sliderSpeed);
                        ImGui::DragFloat3("Rotation", (float *) &currentLight->m_rotation, sliderSpeed);
                        ImGui::TreePop();
                    }
                    if (ImGui::TreeNode("Lighting Data")) {
                        auto &lightInfo = light_itr.second->std140_structure;
                        const std::string currentLightTypeStr = lightTypeStr[lightInfo.type];
                        if (ImGui::BeginCombo("Type", currentLightTypeStr.c_str())) {
                            //Lighting Type
                            int i = 0;
                            for (const auto &lightName: lightTypeStr) {
                                bool isSelected = (currentLightTypeStr == lightName);
                                if (ImGui::Selectable(lightName.c_str(), isSelected)) {
                                    lightInfo.type = i;
                                }
                                if (isSelected) {
                                    ImGui::SetItemDefaultFocus();
                                }
                                ++i;
                            }
                            ImGui::EndCombo();
                        }
                        ImGui::DragFloat("Ka", (float *) &lightInfo.Ka, 0.02, 0.f, 1.f);
                        ImGui::DragFloat("Kd", (float *) &lightInfo.Kd, 0.02, 0.f, 1.f);
                        ImGui::DragFloat("Ks", (float *) &lightInfo.Ks, 0.02, 0.f, 1.f);

                        glm::vec3 I_AColor = lightInfo.Ia / 256.f;
                        ImGui::ColorEdit3("Ia", (float *) &I_AColor.x);
                        lightInfo.Ia = I_AColor * 256.f;

                        glm::vec3 I_DColor = lightInfo.Id / 256.f;
                        ImGui::ColorEdit3("Id", (float *) &I_DColor.x);
                        lightInfo.Id = I_DColor * 256.f;

                        glm::vec3 I_SColor = lightInfo.Is / 256.f;
                        ImGui::ColorEdit3("Is", (float *) &I_SColor.x);
                        lightInfo.Is = I_SColor * 256.f;

                        ImGui::DragFloat("c1", &lightInfo.c1, 0.001f, 0.f, 1.f);
                        ImGui::DragFloat("c2", &lightInfo.c2, 0.001f, 0.f, 1.f);
                        ImGui::DragFloat("c3", &lightInfo.c3, 0.001f, 0.f, 1.f);

                        if (lightInfo.type > Light::LightType::POINT_LIGHT) {
                            ImGui::DragFloat3("LightDir", &lightInfo.dir.x, 0.001f, -1.f, 1.f);

                            float currentInnerRadAsDegree = lightInfo.theta * 180.f / PI;
                            float currentOuterRadAsDegree = lightInfo.phi * 180.f / PI;
                            ImGui::DragFloat("InnerConeAngle", &currentInnerRadAsDegree, 0.01f, 0.f,
                                             currentOuterRadAsDegree - 1.f);


                            ImGui::DragFloat("OuterConeAngle", &currentOuterRadAsDegree, 0.01f,
                                             currentInnerRadAsDegree + 1.f, 180.f);

                            lightInfo.theta = PI / 180.f * currentInnerRadAsDegree;
                            lightInfo.phi = PI / 180.f * currentOuterRadAsDegree;

                            ImGui::DragFloat("FallOffFactor", &lightInfo.P);
                        }
                        ImGui::TreePop();
                    }
                    if (ImGui::TreeNode("ShaderUniforms")) {
                        const std::string &objName = light_itr.second->mObjectName;
                        auto pShader = light_itr.second->m_pShader;
                        auto &uniforms = pShader->GetUniforms();
                        for (auto &uniformAttrib: uniforms) {
                            std::string name = uniformAttrib.first;
                            if (name.find("_GUIX") <= name.length()) {
                                continue;
                            }
                            switch (uniformAttrib.second.mType) {
                                case DataType::Bool: {
                                    GLboolean *value = &pShader->GetUniformValue<GLboolean>(objName, name);
                                    ImGui::NextColumn();
                                    ImGui::Text("%s", name.c_str());
                                    ImGui::NextColumn();
                                    ImGui::Checkbox(("##" + name).c_str(), reinterpret_cast<bool *>(value));
                                    break;
                                }
                                case DataType::Int: {
                                    GLint *value = &pShader->GetUniformValue<GLint>(objName, name);
                                    ImGui::NextColumn();
                                    ImGui::Text("%s", name.c_str());
                                    ImGui::NextColumn();
                                    ImGui::DragInt(("##" + name).c_str(), value);
                                    break;
                                }
                                case DataType::Float: {
                                    GLfloat *value = &pShader->GetUniformValue<GLfloat>(objName, name);
                                    ImGui::NextColumn();
                                    ImGui::Text("%s", name.c_str());
                                    ImGui::NextColumn();
                                    ImGui::DragFloat(("##" + name).c_str(), value, 0.1f);
                                    break;
                                }
                                case DataType::Vec2f: {
                                    glm::vec2 *value = &pShader->GetUniformValue<glm::vec2>(objName, name);
                                    ImGui::NextColumn();
                                    ImGui::Text("%s", name.c_str());
                                    ImGui::NextColumn();
                                    ImGui::DragFloat2(("##" + name).c_str(), &value->x, 0.1f);
                                    break;
                                }
                                case DataType::Vec3f: {
                                    glm::vec3 *value = &pShader->GetUniformValue<glm::vec3>(objName, name);
                                    ImGui::NextColumn();
                                    ImGui::Text("%s", name.c_str());
                                    ImGui::NextColumn();
                                    if ((name.find("color") < name.length()) | (name.find("Color") < name.length())) {
                                        ImGui::ColorEdit3(("##" + name).c_str(), &value->x);
                                    } else {

                                        ImGui::DragFloat3(("##" + name).c_str(), &value->x, 0.1f);
                                    }
                                    break;
                                }
                                case DataType::Vec4f: {
                                    glm::vec4 *value = &pShader->GetUniformValue<glm::vec4>(objName, name);
                                    ImGui::NextColumn();
                                    ImGui::Text("%s", name.c_str());
                                    ImGui::NextColumn();
                                    if ((name.find("color") < name.length()) | (name.find("Color") < name.length())) {
                                        ImGui::ColorEdit4(("##" + name).c_str(), &value->x);
                                    } else {
                                        ImGui::DragFloat4(("##" + name).c_str(), &value->x, 0.1f);
                                    }
                                    break;
                                }
                                case DataType::Mat3f: {
//glm::mat3* value = &pShader->GetUniformValue<glm::mat3>(name);
//ImGui::DragFloat3(name.c_str(), value->operator[](0));
                                    break;
                                }
                                case DataType::Mat4f: {
//                SetUniformMatrix4f(name.c_str(), GetUniformValue<glm::mat4>(name));
                                    break;
                                }
                                default: {
/*throw "Logic error";*/
                                    break;
                                }
                            }
                        }
                        ImGui::TreePop();
                    }
                }
                ImGui::PopID();
            }
        }
        ImGui::End();
    }

    void GUI_Manager::RenderTextureWindow(const std::string &slotName, std::string &textureName, bool *isOpen) {
        ImGui::Begin(slotName.c_str(), isOpen);

        auto &TextureComponents = engine::GetTextureManager().GetTextureObjects();
        if (ImGui::BeginCombo("Texture", textureName.c_str())) {
            for (auto &texture: TextureComponents) {
                const std::string textureStr = texture.first;
                bool isSelected = (textureName == textureStr);
                if (ImGui::Selectable(textureStr.c_str(), isSelected)) {
                    textureName = textureStr;

                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        // Using a Child allow to fill all the space of the window.
        // It also alows customization
        // Get the size of the child (i.e. the whole draw size of the windows).
        ImVec2 wsize = ImGui::GetWindowSize();
        // Because I use the texture from OpenGL, I need to invert the V from the UV.
        auto pTextureObj = engine::GetTextureManager().FindTextureByName(textureName);
        if (pTextureObj) {
            auto handle = (void *) pTextureObj->GetHandle();
            ImGui::Image(handle, wsize, ImVec2(0, 1), ImVec2(1, 0));
        }

        ImGui::End();
    }

    void GUI_Manager::RenderAssignmentDetail() {
        ImGui::Begin("Assignment Detail", &show_assignment_detail);

        static bool DoRearrangeOrbit = true;
        DoRearrangeOrbit = false;
        static bool firstStart = true;
        static bool firstStartDoRandLightCol = false;
        static bool firstStartDoRandomLightType = false;

        using std::placeholders::_1;
        static int numOrbitLights = 8;
//        numOrbitLights = engine::GetCurrentScene()->GetNumActiveLights();
        constexpr float orbitRadius = 1.5f;
        constexpr float orbitalMoveSphereRadius = 0.2f;

        static float firstOrbitLightRadian = 0.f;

        static auto OrbitsMoveUpdate = [&, currentRadian = 0.f](int i, MeshObject *obj) mutable {
            //axis y is fixed
            if (DoRearrangeOrbit) {
                obj->SetScale(glm::vec3(orbitalMoveSphereRadius));
                currentRadian = firstOrbitLightRadian + PI * 2.f / (numOrbitLights) * i;
            }
            auto pCentralObject = engine::GetCurrentScene()->GetObjectList().find("CentralObject")->second;
            assert(pCentralObject->IsMeshType());
            MeshObject* pCentralMeshObject = static_cast<MeshObject*>(pCentralObject);
            glm::vec3 center = pCentralMeshObject->GetPosition();
            glm::vec2 fixedYCenter = glm::vec2(center.x, center.z);
            fixedYCenter += orbitRadius * glm::vec2(std::cos(currentRadian), std::sin(currentRadian));
            obj->SetPosition(glm::vec3(fixedYCenter.x, center.y + 1.f, fixedYCenter.y));
            obj->SetRotation(glm::vec3(cos(-currentRadian), 0.f, sin(-currentRadian)));
            static_cast<Light *>(obj)->std140_structure.dir =
                    obj->GetPosition() + glm::vec3(0.f, 0.5f, 0.f) - pCentralMeshObject->GetPosition();
            currentRadian += 0.003f;
            firstOrbitLightRadian += numOrbitLights == 0 ? 0.f : 0.003f / numOrbitLights;
        };

        if (firstStart) {
            for (int i = 0; i < numOrbitLights; ++i) {
                std::random_device randomDevice;
                std::uniform_int_distribution<int> randomDistribution(0, 255);
                std::uniform_int_distribution<int> randomDistribution2(0, 100);
                const std::string &objName = "OrbitObject" + std::to_string(i);
                auto pLight = engine::GetCurrentScene()->AddLight(objName, "Sphere", "DiffuseShader");
                pLight->BindFunction(std::bind(OrbitsMoveUpdate, i, _1));
                glm::vec3 randomColor = glm::vec3(randomDistribution(randomDevice) / 255.f,
                                                  randomDistribution(randomDevice) / 255.f,
                                                  randomDistribution(randomDevice) / 255.f);
//           engine::GetShader(pLight->GetUsingShaderName())->GetUniformValue<glm::vec3>(pLight->GetName(), "diffuseColor")
//                   = randomColor;
                if (firstStartDoRandLightCol == false) {
                    randomColor = Color(0.8f).AsVec3();
                }
                pLight->SetColor(Color(randomColor.x, randomColor.y, randomColor.z));
                pLight->std140_structure.Ia = randomColor * 128.f;
                pLight->std140_structure.Id = randomColor * 180.f;
                pLight->std140_structure.Is = randomColor * 230.f;

                Light::LightType type = Light::LightType::SPOT_LIGHT;
                if (firstStartDoRandomLightType) {
                    if (randomDistribution2(randomDevice) < 50) {
                        type = Light::POINT_LIGHT;
                    }
                }

                pLight->std140_structure.type = type;
                if (type == Light::LightType::POINT_LIGHT) {
                    pLight->std140_structure.Ks = 0.2f;
                }

                if (pLight->std140_structure.type == Light::LightType::SPOT_LIGHT) {
                    pLight->SetMesh("Cube");
                }

            }
            firstStart = false;
            DoRearrangeOrbit = true;
        }

        if (ImGui::BeginTabBar("##TabBar")) {
            if (ImGui::BeginTabItem("Default")) {
                static std::vector<std::string> Scenarios{"Scenario1", "Scenario2", "Scenario3"};
                static std::string currentScenario = Scenarios.front();
//                ImGui::ColorEdit3("GlobalAmbient", &engine::GlobalAmbientColor.x);
//                ImGui::ColorEdit3("FogColor", &engine::FogColor.x);

                ImGui::Text("%s",
                            (std::to_string(engine::GetCurrentScene()->GetNumActiveLights()) + " Lights").c_str());
                ImGui::SameLine();
                if (ImGui::Button("AddLight")) {
                    const int i = engine::GetCurrentScene()->GetNumActiveLights();
                    if (i < ENGINE_SUPPORT_MAX_LIGHTS) {
                        numOrbitLights = i + 1;
                        std::random_device randomDevice;
                        std::uniform_int_distribution<int> randomDistribution(0, 255);
                        std::uniform_int_distribution<int> randomDistribution2(0, 100);
                        const std::string &objName = "OrbitObject" + std::to_string(i);
                        auto pLight = engine::GetCurrentScene()->AddLight(objName, "Sphere", "DiffuseShader");
                        pLight->BindFunction(std::bind(OrbitsMoveUpdate, i, _1));
                        glm::vec3 randomColor = glm::vec3(randomDistribution(randomDevice) / 255.f,
                                                          randomDistribution(randomDevice) / 255.f,
                                                          randomDistribution(randomDevice) / 255.f);
                        if (firstStartDoRandLightCol == false) {
                            randomColor = Color(0.8f).AsVec3();
                        }
                        pLight->SetColor(Color(randomColor.x, randomColor.y, randomColor.z));
                        pLight->std140_structure.Ia = randomColor * 128.f;
                        pLight->std140_structure.Id = randomColor * 180.f;
                        pLight->std140_structure.Is = randomColor * 230.f;

                        Light::LightType type = Light::LightType::SPOT_LIGHT;
                        if (firstStartDoRandomLightType) {
                            if (randomDistribution2(randomDevice) < 50) {
                                type = Light::POINT_LIGHT;
                            }
                        }

                        pLight->std140_structure.type = type;
                        if (type == Light::LightType::POINT_LIGHT) {
                            pLight->std140_structure.Ks = 0.2f;
                        }
                        if (pLight->std140_structure.type == Light::LightType::SPOT_LIGHT) {
                            pLight->SetMesh("Cube");
                        }


                        DoRearrangeOrbit = true;
                    }

                }
                ImGui::SameLine();
                if (ImGui::Button("RemoveLight")) {
                    const int i = engine::GetCurrentScene()->GetNumActiveLights() - 1;
                    if (i > 0) {
                        engine::GetCurrentScene()->RemoveLight("OrbitObject" + std::to_string(i));
                        numOrbitLights = i;
                        DoRearrangeOrbit = true;
                        if (i == 0) {
                            firstOrbitLightRadian = 0.f;
                        }
                    }
                }

                if (ImGui::BeginCombo("Scenario", currentScenario.c_str())) {
                    for (const auto &Scenario: Scenarios) {
                        bool isSelected = (currentScenario == Scenario);
                        if (ImGui::Selectable(Scenario.c_str(), isSelected)) {
                            currentScenario = Scenario;
                            engine::GetCurrentScene()->ClearLights();
                            firstStart = true;
                            numOrbitLights = 8;
                            if (Scenario == Scenarios[0]) {
                                firstStartDoRandLightCol = false;
                                firstStartDoRandomLightType = false;
                            } else if (Scenario == Scenarios[1]) {
                                firstStartDoRandLightCol = true;
                                firstStartDoRandomLightType = false;
                            } else if (Scenario == Scenarios[2]) {
                                firstStartDoRandLightCol = true;
                                firstStartDoRandomLightType = true;
                            }
                        }
                        if (isSelected) {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }

                static bool update = true;
                if (ImGui::Checkbox("UpdateOrbitMove", &update)) {
                    auto &lights = engine::GetCurrentScene()->GetLightList();
                    for (auto &light: lights) {
                        if (light.first.find("OrbitObject") <= light.first.length()) {
                            light.second->SetFunctionUpdate(update);
                        }
                    }
                }

                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Shader")) {
                const auto &shaderNameList = engine::GetShaderManager().GetNameList();
                for (auto &shaderName: shaderNameList) {
                    ImGui::PushID(shaderName.c_str());
                    Color textColor(1.f);
                    auto shaderObject = engine::GetShader(shaderName);
                    if (shaderObject->HasError()) {
                        textColor = Color(1.f, 0.f, 0.f);
                    }
                    ImVec4 color(textColor.r, textColor.g, textColor.b, textColor.a);
                    ImGui::TextColored(color, "%s", shaderObject->GetName().c_str());
                    ImGui::SameLine();
                    if (ImGui::Button("Reload")) {

                        shaderObject->Reload();
                        DoRearrangeOrbit = true;
                    }
                    ImGui::PopID();
                }
                ImGui::EndTabItem();
            }


            ImGui::EndTabBar();
        }

        ImGui::End();
    }
}
