/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: TestScene.cpp
Purpose: Source file for TestScene
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS300_2
Author: Yoonki Kim, y.kim,  180002421
Creation date: Nov 7, 2021
End Header --------------------------------------------------------*/
#include "TestScene.h"
#include <glm/gtx/transform.hpp>
#include <random>
#include "Engine.h"
using std::placeholders::_1;
void TestScene::Init()
{
SceneBase::Init();
//        constexpr int numOrbitLights = 3;
constexpr float orbitRadius = 1.5f;
//        constexpr float orbitalMoveSphereRadius = 0.2f;
//        static auto OrbitsMoveUpdate = [&, initialSetting = true, currentRadian = 0.f, max = numOrbitLights](int i, Object* obj) mutable {
//            //axis y is fixed
//            if(initialSetting){
//                obj->SetScale(glm::vec3(orbitalMoveSphereRadius));
//                currentRadian = PI * 2.f / max * i;
//                initialSetting = false;
//            }
//            if(i > max){
//                std::cerr << "Orbit sphere hit max capacity!!" << std::endl;
//                return;
//            }
//            auto pCentralObject = GetObjectList().find("CentralObject")->second;
//            glm::vec3 center = pCentralObject->GetPosition();
//            glm::vec2 fixedYCenter = glm::vec2(center.x, center.z);
//            fixedYCenter += orbitRadius * glm::vec2(std::cos(currentRadian), std::sin(currentRadian));
//            obj->SetPosition(glm::vec3(fixedYCenter.x, center.y + 1.f, fixedYCenter.y));
//            obj->SetRotation(glm::vec3(cos(-currentRadian),0.f,sin(-currentRadian)));
//            static_cast<Light*>(obj)->std140_structure.dir = obj->GetPosition() + glm::vec3(0.f, 0.5f, 0.f) - pCentralObject->GetPosition();
//            currentRadian += 0.003f;
//        };

[[maybe_unused]]static auto DrawOrbit = [&, initialSetting = true](Object* obj) mutable {
    static std::vector<glm::vec3> orbitLines;
    constexpr int segments = 100;
    static GLuint vertexVBO;

    if(initialSetting){
//                orbitLines.reserve(segments * 2);
//                glm::vec3 center = obj->GetPosition();
        const float radianMove = PI * 2.f / segments;
        for(float radian = 0.f; radian <= PI * 2.f /*- radianMove*/; radian += radianMove){
            orbitLines.emplace_back(glm::vec3(std::cos(radian), 0.5f , std::sin(radian)) * 0.5f);
            orbitLines.emplace_back(glm::vec3( std::cos(radian + radianMove), 0.5f, std::sin(radian + radianMove)) * 0.5f);
        }

        glGenBuffers(1, &vertexVBO);
        glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
        glBufferData(GL_ARRAY_BUFFER, orbitLines.size() * sizeof(decltype(orbitLines)::value_type), orbitLines.data(), GL_STATIC_DRAW);

        initialSetting = false;
    }

    //////////////Drawing Logic//////////////
    const GLint shaderPID = Engine::GetShader("FaceNormalDrawShader")->GetProgramID();

    //setting&binding buffer
    auto& attributeInfos = Engine::GetShader("FaceNormalDrawShader")->GetAttribInfos();
    const GLuint VAOID = Engine::GetVAOManager().GetVAO( Engine::GetShader("FaceNormalDrawShader")->GetAttributeID());

    glBindVertexArray(VAOID);
    for(auto& attribute : attributeInfos){
        if(attribute.name == "FaceNormalPosition"){
            glEnableVertexAttribArray(attribute.location);


            glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
            glVertexAttribPointer( attribute.location,
                                   attribute.DataSize,
                                   attribute.DataType,
                                   GL_FALSE,
                                   0,
                                   (void *) 0 );
        }
    }
    //Drawing Logic
    glUseProgram(shaderPID);
    GLint vTransformLoc = glGetUniformLocation(shaderPID, "vertexTransform");
    if(vTransformLoc < 0){
        std::cerr << "Unable to find uniform variable!" << std::endl;
    }
    const auto& pCam = Engine::GetCurrentScene()->GetCurrentCamera();

    //Get matricies
//            glm::mat4 modelToWorldMatrix = obj->GetObjectToWorldMatrix();


    glm::mat4 tempToWorld(1.f);
    tempToWorld = glm::translate(tempToWorld, obj->GetPosition());

    tempToWorld = glm::rotate(tempToWorld, obj->GetRotation().x, glm::vec3{ 1.f, 0.f, 0.f });
    tempToWorld = glm::rotate(tempToWorld, obj->GetRotation().y, glm::vec3{ 0.f, 1.f, 0.f });
    tempToWorld = glm::rotate(tempToWorld, obj->GetRotation().z, glm::vec3{ 0.f, 0.f, 1.f });
    tempToWorld = glm::scale(tempToWorld, obj->GetScale() * orbitRadius * 2.f);

    glm::mat4 viewMatrix = pCam->GetLookAtMatrix();
    glm::mat4 projectionMatrix = pCam->GetPerspectiveMatrix();
    glm::mat4 mvpMatrix = projectionMatrix * viewMatrix * tempToWorld;

    glUniformMatrix4fv(vTransformLoc, 1, GL_FALSE, &mvpMatrix[0][0]);


    //Draw
    glDrawArrays(GL_LINES, 0, orbitLines.size());
    //Clean
    for(auto& attribute : attributeInfos){
        glDisableVertexAttribArray(attribute.location);
    }
    glBindVertexArray(0);
    glUseProgram(0);
};


SceneBase::AddCamera();
//for initializing camera
SceneBase::Init();

/*auto pCentralObj = */AddObject("CentralObject", "Bunny", "PhongShader");
//        pCentralObj->BindFunction(DrawOrbit);

//        for(int i = 0; i < numOrbitLights; ++i){
//            std::random_device randomDevice;
//            std::uniform_int_distribution<int> randomDistribution(0, 255);
//            const std::string& objName = "OrbitObject" + std::to_string(i);
//            auto pLight = AddLight(objName, "Sphere", "PhongShader");
//            pLight->BindFunction(std::bind(OrbitsMoveUpdate,  i, _1));
//            glm::vec3 randomColor = glm::vec3(randomDistribution(randomDevice) / 255.f, randomDistribution(randomDevice) / 255.f, randomDistribution(randomDevice) / 255.f);
////           Engine::GetShader(pLight->GetUsingShaderName())->GetUniformValue<glm::vec3>(pLight->GetName(), "diffuseColor")
////                   = randomColor;
//           pLight->SetColor(Color(randomColor.x, randomColor.y, randomColor.z));
//           pLight->std140_structure.Ia = randomColor * 128.f;
//           pLight->std140_structure.Id = randomColor * 180.f;
//           pLight->std140_structure.Is = randomColor * 230.f;
//
//           pLight->std140_structure.type = Light::LightType::SPOT_LIGHT;
//        }

auto pPlaneObj = AddObject("Plane1", "Plane", "PhongShader");
pPlaneObj->SetRotation(glm::vec3(-HALF_PI,0.f, 0.f));
pPlaneObj->SetScale(glm::vec3(100.f, 100.f, 100.f));
pPlaneObj->SetPosition(glm::vec3(0.f, -0.45f, 0.f));
pPlaneObj->SetTextureOption(false);
}


