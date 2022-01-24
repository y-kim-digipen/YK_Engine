/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: GUIManager.cpp
Purpose: Source file for GUIManager
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS300_2
Author: Yoonki Kim, y.kim,  180002421
Creation date: Nov 7, 2021
End Header --------------------------------------------------------*/
#include "GUIManager.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "GUIWindow.h"
#include "EngineInfo.h"
#include "engine/engine.h"
#include "TextureContent.h"

#include "engine/scene/SceneBase.h"
#include "ColorSet.h"

#include <GLFW/glfw3.h> // Will drag system OpenGL headers

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

        m_pWindows.try_emplace("engine Info Window", new EngineInfo("engine Info Window"));

        InitWindows();

        show_demo_window = true;
    }

    void GUI_Manager::Update() {
        PreRender();
        Render();
        PostRender();
    }

    void GUI_Manager::CleanUp() {
        for (auto pWindow: m_pWindows) {
            pWindow.second->CleanUp();
        }
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    GUI_Window *GUI_Manager::AddWindow(const std::string &name) {
        auto result = m_pWindows.try_emplace(name, new GUI_Window(name));
        if (!result.second) {
            return nullptr;
        }
        InitWindows();
        return result.first->second;
    }

    bool GUI_Manager::HasWindow(const std::string &window) {
        return m_pWindows.find(window) != m_pWindows.end();
    }

    void GUI_Manager::RemoveWindow(const std::string &windowNameStr) {
        if (!HasWindow(windowNameStr)) {
            return;
        }
        delete m_pWindows.find(windowNameStr)->second;
        m_pWindows.erase(windowNameStr);
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



        for (auto &pWindow: m_pWindows) {
            pWindow.second->PreRender();
            pWindow.second->Render();
            pWindow.second->PostRender();
            if(pWindow.second->mIsOpen == false)
            {
                deleting_que.push_back(pWindow.first);
            }
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
        for(auto str : deleting_que)
        {
            m_pWindows.erase(str);
        }
//        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void GUI_Manager::InitWindows() {
        for (auto pWindow: m_pWindows) {
            pWindow.second->Init();
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
                if (ImGui::BeginMenu("Show")) {
                    ImGui::MenuItem("Objects", NULL, &show_object_list);
                    ImGui::MenuItem("Lights", NULL, &show_light_list);
                    ImGui::EndMenu();
                }
                if (ImGui::MenuItem("AddTextureWindow")) {
                    auto pWindow = AddWindow("Hello" + std::to_string(m_pWindows.size()));
                    pWindow->AddContent("TextureContent", new TextureContent);

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
                    auto currentObject = obj_itr.second;
                    if (ImGui::TreeNode("Transform")) {
                        static float sliderSpeed = 0.1f;
                        ImGui::InputFloat("SliderSpeed", (float *) &sliderSpeed);
                        ImGui::DragFloat3("Position", (float *) &currentObject->m_position, sliderSpeed);
                        ImGui::DragFloat3("Scale", (float *) &currentObject->m_scale, sliderSpeed);
                        ImGui::DragFloat3("Rotation", (float *) &currentObject->m_rotation, sliderSpeed);
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
}
