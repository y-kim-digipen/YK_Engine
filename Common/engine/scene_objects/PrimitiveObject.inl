//
// Created by yoonki on 2/26/22.
//
//
//#include "PrimitiveObject.h"
//#include "engine/scene/SceneBase.h"
//#include <glm/gtx/transform.hpp>
//#include "engine/engine.h"
//template<typename PrimitiveType>
//void PrimitiveObject<PrimitiveType>::PostRender()
//{
//    {
//        static bool initialSetting = true;
//        static std::vector<glm::vec3> orbitLines;
//        constexpr int segments = 100;
//        static GLuint vertexVBO;
//
//        if (initialSetting) {
//            const float radianMove = PI * 2.f / segments;
//            for (float radian = 0.f; radian <= PI * 2.f; radian += radianMove) {
//                orbitLines.emplace_back(glm::vec3(std::cos(radian), 0.5f, std::sin(radian)) * 0.5f);
//                orbitLines.emplace_back(
//                        glm::vec3(std::cos(radian + radianMove), 0.5f, std::sin(radian + radianMove)) * 0.5f);
//            }
//
//            glGenBuffers(1, &vertexVBO);
//            glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
//            glBufferData(GL_ARRAY_BUFFER, orbitLines.size() * sizeof(decltype(orbitLines)::value_type),
//                         orbitLines.data(), GL_STATIC_DRAW);
//
//            initialSetting = false;
//        }
//
//        //////////////Drawing Logic//////////////
//        const GLint shaderPID = engine::GetShader("FaceNormalDrawShader")->GetProgramID();
//
//        //setting&binding buffer
//        auto &attributeInfos = engine::GetShader("FaceNormalDrawShader")->GetAttribInfos();
//        const GLuint VAOID = engine::GetVAOManager().GetVAO(
//                engine::GetShader("FaceNormalDrawShader")->GetAttributeID());
//
//        glBindVertexArray(VAOID);
//        for (auto &attribute: attributeInfos) {
//            if (attribute.name == "FaceNormalPosition") {
//                glEnableVertexAttribArray(attribute.location);
//
//
//                glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
//                glVertexAttribPointer(attribute.location,
//                                      attribute.DataSize,
//                                      attribute.DataType,
//                                      GL_FALSE,
//                                      0,
//                                      (void *) 0);
//            }
//        }
//        //Drawing Logic
//        glUseProgram(shaderPID);
//        GLint vTransformLoc = glGetUniformLocation(shaderPID, "vertexTransform");
//        if (vTransformLoc < 0) {
//            std::cerr << "Unable to find uniform variable!" << std::endl;
//        }
//        const auto &pCam = engine::GetCurrentScene()->GetCurrentCamera();
//
//        glm::mat4 tempToWorld(1.f);
//        tempToWorld = glm::translate(tempToWorld, static_cast<PrimitiveType*>(mCollider)->GetCoordinate()[0]);
//
//        tempToWorld = glm::rotate(tempToWorld, 0.f, glm::vec3{1.f, 0.f, 0.f});
//        tempToWorld = glm::rotate(tempToWorld, 0.f, glm::vec3{0.f, 1.f, 0.f});
//        tempToWorld = glm::rotate(tempToWorld, 0.f, glm::vec3{0.f, 0.f, 1.f});
//
//        glm::mat4 viewMatrix = pCam->GetLookAtMatrix();
//        glm::mat4 projectionMatrix = pCam->GetPerspectiveMatrix();
//        glm::mat4 mvpMatrix = projectionMatrix * viewMatrix * tempToWorld;
//
//        glUniformMatrix4fv(vTransformLoc, 1, GL_FALSE, &mvpMatrix[0][0]);
//
//        //Draw
//        glDrawArrays(GL_LINES, 0, orbitLines.size());
//        //Clean
//
//        for (auto &attribute: attributeInfos) {
//            glDisableVertexAttribArray(attribute.location);
//        }
//
//        glBindVertexArray(0);
//        glUseProgram(0);
//    }
//}