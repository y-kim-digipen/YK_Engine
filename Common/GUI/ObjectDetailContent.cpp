/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: ObjectDetailContent.cpp
Purpose: Source file for ObjectDetailContent
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS300_2
Author: Yoonki Kim, y.kim,  180002421
Creation date: Nov 7, 2021
End Header --------------------------------------------------------*/
#include "ObjectDetailContent.h"
#include "Engine.h"
#include "ObjectComponents/ComponentManager.h"
#include "ObjectComponents/ComponentBase.h"

GUI::ObjectDetailContent::ObjectDetailContent(std::shared_ptr<Object> target) : GUI_Content() {
    m_pTargetObject = target;
}

void GUI::ObjectDetailContent::Render() {
    if(IsDead()){
        return;
    }
    if(ImGui::BeginTabBar("##TabBar")) {
        if (ImGui::BeginTabItem("Assets")) {
            //selectable shader
            const auto &shaderList = Engine::GetShaderManager().GetNameList();
            const std::string &currentShaderName = m_pTargetObject->GetUsingShaderName();
            if (ImGui::BeginCombo("ShaderList", currentShaderName.c_str())) {
                for (const auto &shaderName: shaderList) {
                    bool isSelected = (currentShaderName == shaderName);
                    if (ImGui::Selectable(shaderName.c_str(), isSelected)) {
                        m_pTargetObject->SetShader(shaderName);
                    }
                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            //selectable mesh
            const auto &meshList = Engine::GetMeshManager().GetNameList();
            const std::string &currentMeshName = m_pTargetObject->GetUsingMeshName();
            if (ImGui::BeginCombo("MeshList", currentMeshName.c_str())) {
                for (const auto &meshName: meshList) {
                    bool isSelected = (currentMeshName == meshName);
                    if (ImGui::Selectable(meshName.c_str(), isSelected)) {
                        m_pTargetObject->SetMesh(meshName);
                    }
                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Transforms")) {
            static float sliderSpeed = 0.1f;
            ImGui::InputFloat("SliderSpeed", (float *) &sliderSpeed);
            ImGui::DragFloat3("Position", (float *) &m_pTargetObject->m_position, sliderSpeed);
            ImGui::DragFloat3("Scale", (float *) &m_pTargetObject->m_scale, sliderSpeed);
            ImGui::DragFloat3("Rotation", (float *) &m_pTargetObject->m_rotation, sliderSpeed);
            ImGui::EndTabItem();
        }

        /////////
        if (ImGui::BeginTabItem("ShaderUniforms")) {
            const std::string& objName = m_pTargetObject->mObjectName;
            auto pShader = m_pTargetObject->m_pShader;
            auto &uniforms = pShader->GetUniforms();
            for (auto &uniformAttrib: uniforms) {
                std::string name = uniformAttrib.first;
                if(name.find("_GUIX") <= name.length())
                    {
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
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Texture")) {
            static std::vector<std::string> UVTypes {"Planar", "Cylindarical", "Spherical", "CubeMap"};
            const int curUVTypeAsInt = static_cast<int>(m_pTargetObject->mUVType);
            if (ImGui::BeginCombo("UV", UVTypes[curUVTypeAsInt].c_str())) {
                for (int i = 0; i < static_cast<int>(UVTypes.size()); ++i) {
                    bool isSelected = (curUVTypeAsInt == i);
                    if (ImGui::Selectable(UVTypes[i].c_str(), isSelected)) {
                        m_pTargetObject->mUVType = static_cast<Mesh::UVType>(i);
                    }
                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }


            ImGui::Checkbox("UsingTexture", &m_pTargetObject->mUsingTexture);
            ImGui::Checkbox("GPU_UV?", &m_pTargetObject->mUsingGPUUV);

            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Others")) {
            if(ImGui::Checkbox("VertexNormalDrawing", &m_pTargetObject->mDoVertexNormalDrawing)){
                if(m_pTargetObject->mDoFaceNormalDrawing){
                    m_pTargetObject->mDoVertexNormalDrawing = false;
                }
            }
            if(ImGui::Checkbox("FaceNormalDrawing", &m_pTargetObject->mDoFaceNormalDrawing)){
                if(m_pTargetObject->mDoVertexNormalDrawing){
                    m_pTargetObject->mDoFaceNormalDrawing = false;
                }
            }
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
}

void GUI::ObjectDetailContent::CleanUp() {
    m_pTargetObject.reset();
}

bool GUI::ObjectDetailContent::IsDead() {
    return m_pTargetObject == nullptr;
}

bool GUI::ObjectDetailContent::DoDestroyBaseWindow() {
    return IsDead();
}


