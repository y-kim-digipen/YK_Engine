/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: EngineInfoContent.cpp
Purpose: Source file for EngineInfoContent
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS300_2
Author: Yoonki Kim, y.kim,  180002421
Creation date: Nov 7, 2021
End Header --------------------------------------------------------*/

#include "EngineInfoContent.h"
#include "Engine.h"
#include "Object.h"
#include "SceneBase.h"

#include <random>

namespace GUI
{
    void EngineInfoContent::Render() {
        static bool DoRearrangeOrbit = true;
        DoRearrangeOrbit = false;
        static bool firstStart = true;
        static bool firstStartDoRandLightCol = false;
        static bool firstStartDoRandomLightType = false;

        using std::placeholders::_1;
        static int numOrbitLights = 8;
//        numOrbitLights = Engine::GetCurrentScene()->GetNumActiveLights();
        constexpr float orbitRadius = 1.5f;
        constexpr float orbitalMoveSphereRadius = 0.2f;

        static float firstOrbitLightRadian = 0.f;

        static auto OrbitsMoveUpdate = [&, currentRadian = 0.f](int i, Object *obj) mutable {
            //axis y is fixed
            if (DoRearrangeOrbit) {
                obj->SetScale(glm::vec3(orbitalMoveSphereRadius));
                currentRadian = firstOrbitLightRadian + PI * 2.f / (numOrbitLights) * i;
            }
            auto pCentralObject = Engine::GetCurrentScene()->GetObjectList().find("CentralObject")->second;
            glm::vec3 center = pCentralObject->GetPosition();
            glm::vec2 fixedYCenter = glm::vec2(center.x, center.z);
            fixedYCenter += orbitRadius * glm::vec2(std::cos(currentRadian), std::sin(currentRadian));
            obj->SetPosition(glm::vec3(fixedYCenter.x, center.y + 1.f, fixedYCenter.y));
            obj->SetRotation(glm::vec3(cos(-currentRadian), 0.f, sin(-currentRadian)));
            static_cast<Light *>(obj)->std140_structure.dir =
                    obj->GetPosition() + glm::vec3(0.f, 0.5f, 0.f) - pCentralObject->GetPosition();
            currentRadian += 0.003f;
            firstOrbitLightRadian += numOrbitLights == 0 ? 0.f : 0.003f / numOrbitLights;
        };

        if (firstStart) {
            for (int i = 0; i < numOrbitLights; ++i) {
                std::random_device randomDevice;
                std::uniform_int_distribution<int> randomDistribution(0, 255);
                std::uniform_int_distribution<int> randomDistribution2(0, 100);
                const std::string &objName = "OrbitObject" + std::to_string(i);
                auto pLight = Engine::GetCurrentScene()->AddLight(objName, "Sphere", "PhongShader");
                pLight->BindFunction(std::bind(OrbitsMoveUpdate, i, _1));
                glm::vec3 randomColor = glm::vec3(randomDistribution(randomDevice) / 255.f,
                                                  randomDistribution(randomDevice) / 255.f,
                                                  randomDistribution(randomDevice) / 255.f);
//           Engine::GetShader(pLight->GetUsingShaderName())->GetUniformValue<glm::vec3>(pLight->GetName(), "diffuseColor")
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

            }
            firstStart = false;
            DoRearrangeOrbit = true;
        }

        if (ImGui::BeginTabBar("##TabBar")) {
            if (ImGui::BeginTabItem("Default")) {
                static std::vector<std::string> Scenarios{"Scenario1", "Scenario2", "Scenario3"};
                static std::string currentScenario = Scenarios.front();
                ImGui::ColorEdit3("GlobalAmbient", &Engine::GlobalAmbientColor.x);
                ImGui::ColorEdit3("FogColor", &Engine::FogColor.x);

                ImGui::Text("%s", (std::to_string(Engine::GetCurrentScene()->GetNumActiveLights()) + " Lights").c_str());
                ImGui::SameLine();
                if (ImGui::Button("AddLight")) {
                    const int i = Engine::GetCurrentScene()->GetNumActiveLights();
                    if (i < ENGINE_SUPPORT_MAX_LIGHTS) {
                        numOrbitLights = i + 1;
                        std::random_device randomDevice;
                        std::uniform_int_distribution<int> randomDistribution(0, 255);
                        std::uniform_int_distribution<int> randomDistribution2(0, 100);
                        const std::string &objName = "OrbitObject" + std::to_string(i);
                        auto pLight = Engine::GetCurrentScene()->AddLight(objName, "Sphere", "PhongShader");
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

//                if(fir)
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

                        DoRearrangeOrbit = true;
                    }

                }
                ImGui::SameLine();
                if (ImGui::Button("RemoveLight")) {
                    const int i = Engine::GetCurrentScene()->GetNumActiveLights() - 1;
                    if (i > 0) {
                        Engine::GetCurrentScene()->RemoveLight("OrbitObject" + std::to_string(i));
                        Engine::GetGUIManager().RemoveWindow("OrbitObject" + std::to_string(i));
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
                            Engine::GetCurrentScene()->ClearLights();
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
                    auto &lights = Engine::GetCurrentScene()->GetLightList();
                    for (auto &light: lights) {
                        if (light.first.find("OrbitObject") <= light.first.length()) {
                            light.second->SetFunctionUpdate(update);
                        }
                    }
                }

                ImGui::EndTabItem();
            }
            if(ImGui::BeginTabItem("Shader"))
            {
                const auto& shaderNameList = Engine::GetShaderManager().GetNameList();
                for(auto& shaderName : shaderNameList)
                {
                    ImGui::PushID(shaderName.c_str());
                    Color textColor(0.f);
                    auto shaderObject = Engine::GetShader(shaderName);
                    if(shaderObject->HasError())
                    {
                        textColor = Color(1.f, 0.f, 0.f);
                    }
                    ImVec4 color(textColor.r, textColor.g, textColor.b, textColor.a);
                    ImGui::TextColored(color, "%s", shaderObject->GetName().c_str());
                    ImGui::SameLine();
                    if(ImGui::Button("Reload")) {
                        Engine::SwapToMainWindow();
                        shaderObject->Reload();
                        Engine::SwapToGUIWindow();
//                        firstStart = true;
                        DoRearrangeOrbit = true;
                    }
                    ImGui::PopID();
                }
                ImGui::EndTabItem();
            }


            ImGui::EndTabBar();
        }
    }

}
