/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: TestScene.cpp
Purpose: Source file for TestScene
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS350_1
Author: Yoonki Kim, y.kim,  180002421
Creation date: Feb 6, 2022
End Header --------------------------------------------------------*/
#include "TestScene.h"
#include <glm/gtx/transform.hpp>
#include <random>
#include "engine/engine.h"
#include "engine/graphic_misc/BoundingVolume/BasicBoundingVolumes.h"
#include "engine/scene_objects/PrimitiveObject.h"

using std::placeholders::_1;
void TestScene::Init() {
    SceneBase::Init();
    constexpr float orbitRadius = 1.5f;
    [[maybe_unused]]static auto DrawOrbit = [&, initialSetting = true](MeshObject *obj) mutable {
        static std::vector<glm::vec3> orbitLines;
        constexpr int segments = 100;
        static GLuint vertexVBO;

        if (initialSetting) {
            const float radianMove = PI * 2.f / segments;
            for (float radian = 0.f; radian <= PI * 2.f; radian += radianMove) {
                orbitLines.emplace_back(glm::vec3(std::cos(radian), 0.5f, std::sin(radian)) * 0.5f);
                orbitLines.emplace_back(
                        glm::vec3(std::cos(radian + radianMove), 0.5f, std::sin(radian + radianMove)) * 0.5f);
            }

            glGenBuffers(1, &vertexVBO);
            glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
            glBufferData(GL_ARRAY_BUFFER, orbitLines.size() * sizeof(decltype(orbitLines)::value_type),
                         orbitLines.data(), GL_STATIC_DRAW);

            initialSetting = false;
        }

        //////////////Drawing Logic//////////////
        const GLint shaderPID = engine::GetShader("FaceNormalDrawShader")->GetProgramID();

        //setting&binding buffer
        auto &attributeInfos = engine::GetShader("FaceNormalDrawShader")->GetAttribInfos();
        const GLuint VAOID = engine::GetVAOManager().GetVAO(
                engine::GetShader("FaceNormalDrawShader")->GetAttributeID());

        glBindVertexArray(VAOID);
        for (auto &attribute: attributeInfos) {
            if (attribute.name == "FaceNormalPosition") {
                glEnableVertexAttribArray(attribute.location);


                glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
                glVertexAttribPointer(attribute.location,
                                      attribute.DataSize,
                                      attribute.DataType,
                                      GL_FALSE,
                                      0,
                                      (void *) 0);
            }
        }
        //Drawing Logic
        glUseProgram(shaderPID);
        GLint vTransformLoc = glGetUniformLocation(shaderPID, "vertexTransform");
        if (vTransformLoc < 0) {
            std::cerr << "Unable to find uniform variable!" << std::endl;
        }
        const auto &pCam = engine::GetCurrentScene()->GetCurrentCamera();

        glm::mat4 tempToWorld(1.f);
        tempToWorld = glm::translate(tempToWorld, obj->GetPosition());

        tempToWorld = glm::rotate(tempToWorld, obj->GetRotation().x, glm::vec3{1.f, 0.f, 0.f});
        tempToWorld = glm::rotate(tempToWorld, obj->GetRotation().y, glm::vec3{0.f, 1.f, 0.f});
        tempToWorld = glm::rotate(tempToWorld, obj->GetRotation().z, glm::vec3{0.f, 0.f, 1.f});
        tempToWorld = glm::scale(tempToWorld, obj->GetScale() * orbitRadius * 2.f);

        glm::mat4 viewMatrix = pCam->GetLookAtMatrix();
        glm::mat4 projectionMatrix = pCam->GetPerspectiveMatrix();
        glm::mat4 mvpMatrix = projectionMatrix * viewMatrix * tempToWorld;

        glUniformMatrix4fv(vTransformLoc, 1, GL_FALSE, &mvpMatrix[0][0]);

        //Draw
        glDrawArrays(GL_LINES, 0, orbitLines.size());
        //Clean

        for (auto &attribute: attributeInfos) {
            glDisableVertexAttribArray(attribute.location);
        }

        glBindVertexArray(0);
        glUseProgram(0);
    };

    SceneBase::AddCamera();
    SceneBase::Init();

//    auto pPlaneObj = AddObject<MeshObject>("Plane1", "Plane", "DeferredRender");
//    pPlaneObj->SetRotation(glm::vec3(-HALF_PI, 0.f, 0.f));
//    pPlaneObj->SetScale(glm::vec3(100.f, 100.f, 100.f));
//    pPlaneObj->SetPosition(glm::vec3(0.f, -0.5f, 0.f));
//    pPlaneObj->SetTextureOption(false);
//
//    pPlaneObj = AddObject<MeshObject>("Plane2", "Plane", "DeferredRender");
//    pPlaneObj->SetScale(glm::vec3(100.f, 100.f, 100.f));
//    pPlaneObj->SetPosition(glm::vec3(0.f, -0.5f, -40.f));
//    pPlaneObj->SetTextureOption(false);

    auto pObj = AddObject<MeshObject>("CentralObject", "Lucy", "DeferredRender");
//    pObj->BindFunction(DrawOrbit);
    Sphere* objCollider = new Sphere();
    objCollider->BuildFromVertices(engine::GetMesh(static_cast<MeshObject*>(pObj)->GetUsingMeshName())->getVertexBufferVectorForm());
    pObj->SetCollider(objCollider);
    pObj->SetPosition(glm::vec3(0.f, 0.f, 0.f));
    pObj->BindFunction([](BaseObject* obj){
        static float t = 0.f;
        MeshObject* pObj = static_cast<MeshObject*>(obj);
        glm::vec3 currentPos = pObj->GetPosition();
        pObj->SetPosition(glm::vec3(0.f, currentPos.y, currentPos.z) + glm::vec3(2 * cos(t), 0.f, 0.f));
        t+= 0.01f;
    });

    pObj = AddObject<MeshObject>("CentralObject2", "Bunny", "DeferredRender");
    static_cast<MeshObject*>(pObj)->SetPosition(glm::vec3(0.f, 0.f, 1.f));
    AABB* objCollider2 = new AABB();
    objCollider2->BuildFromVertices(engine::GetMesh(static_cast<MeshObject*>(pObj)->GetUsingMeshName())->getVertexBufferVectorForm());
    pObj->SetCollider(objCollider2);
    pObj->SetPosition(glm::vec3(2.f, 0.f, 0.f));

    auto triangleObj = AddObject<PrimitiveObject<Triangle>>("Triangle", glm::vec3(-0.3f, -0.2f, -2.9f), glm::vec3(0.7f, 0.0f, -1.2f), glm::vec3(0.f, 1.f, 1.9f));
    auto pointObj = AddObject<PrimitiveObject<Point3D>>("Point", glm::vec3(0.f, 0.5f, 0.0f));
    pointObj->BindFunction([](BaseObject* obj){
        static float t = 0.f;
        Point3D* point = static_cast<Point3D*>(obj->GetCollider());
        point->mCoordinate.x = cos(t) * 2.f;
        point->mCoordinate.z = sin(t) * 2.f;
        t+= 0.02f;
    });


    auto pointObj2 = AddObject<PrimitiveObject<Point3D>>("Point2", glm::vec3(0.f, 0.5f, 0.0f));
    pointObj2->BindFunction([](BaseObject* obj){
        static float t = 0.f;
        Point3D* point = static_cast<Point3D*>(obj->GetCollider());
        point->mCoordinate.x = sin(t) * 2.f;
        point->mCoordinate.z = cos(t) * 2.f;
        point->mCoordinate.y = cos(t) * sin(t) * 2.f;
        t+= 0.01f;
    });

    auto rayObj  = AddObject<PrimitiveObject<Ray>>("Ray", glm::vec3(0.f, 0.0f, 0.0f), glm::vec3(1.f,0.f, 1.f));
    rayObj->BindFunction([](BaseObject* obj){
        static float t = 0.f;

        Ray* ray = static_cast<Ray*>(obj->GetCollider());
        ray->mDir.x = cos(t);
        ray->mDir.z = sin(t);
        t+= 0.01f;
    });
    auto planeObj = AddObject<PrimitiveObject<Plane>>("Plane", glm::vec4(1.f, 0.3f, 0.0f, 0.f));
    planeObj->BindFunction([](BaseObject* obj)
    {
        static float t = 0.f;

        Plane* plane = static_cast<Plane*>(obj->GetCollider());
        plane->m_Normal.w = 10.f * cos(t);
        t+= 0.003f;
    });

    for(int i = 0; i <= 5; ++i)
    {
        for(int j = 0; j <= 5; ++j)
        {
            Color objectColor{1.f, 0.5f, 0.5f};
            float metallic = 1.f / 5  * i;
            float roughness = 1.f / 5 * j;
            glm::vec3 position {-2.f * 5 / 2.f + (10.f / 5) * i, 6.f / 5.f * j, -30.f};
            auto ballObj = AddObject<MeshObject>("4Sphere" + std::to_string(i) + std::to_string(j), "4Sphere", "DeferredRender");
            ballObj->baseColor = objectColor;
            ballObj->metallic = metallic;
            ballObj->roughness = roughness;
            ballObj->SetVisibleOnEditor(false);

            ballObj->SetPosition(position);
        }
    }
}


