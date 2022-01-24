/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Object.h
Purpose: Header file for Object
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS300_2
Author: Yoonki Kim, y.kim,  180002421
Creation date: Nov 7, 2021
End Header --------------------------------------------------------*/

#ifndef ENGINE_OBJECT_H
#define ENGINE_OBJECT_H

#include <memory>
#include <functional>
#include <vector>

#include "engine/graphic_misc/Shader.h"
#include "engine/graphic_misc/Mesh.h"
#include "engine/graphic_misc/Color.h"

namespace GUI{
    class GUI_Manager;
}
//class CubeCaptureCamera;

class Object{
    friend class GUI::GUI_Manager;
public:
    Object(const std::string& name);
    Object(const std::string& name, std::shared_ptr<Mesh> pMesh, std::shared_ptr<Shader> pShader);
    Object(const std::string& name, const std::string& meshStr, const std::string& shaderStr);
    virtual ~Object();
    void Init();
    virtual void PreRender();
    void Render() const;
    void PostRender();

    void CleanUp() const;
    //todo implement this
    bool SetShader(const std::string& shaderStr);
    bool SetMesh(const std::string& meshStr);

    [[nodiscard]] std::string GetUsingMeshName() const;
    [[nodiscard]] std::string GetUsingShaderName() const;

    [[nodiscard]] bool IsRenderReady() const;

    //Transform settings
    glm::vec3 GetPosition();
    void SetPosition(glm::vec3 position);
    void AddPosition(glm::vec3 amount);

    glm::vec3 GetRotation();
    void SetRotation(glm::vec3 rotation);
    void AddRotation(glm::vec3 amount);

    glm::vec3 GetScale();
    void SetScale(glm::vec3 scale);
    void AddScale(glm::vec3 amount);

    void SetColor(Color newColor);

    void BindFunction(std::function<void(Object*)> func);
    void RemoveFunction();
    void SetFunctionUpdate(bool updateStatus);
    void SetTextureOption(bool usingTexture, bool usingGPUUV = false);

    glm::mat4 GetObjectToWorldMatrix() const;
    std::string GetName() const;
    void ChangeTexture(int slot, const std::string& textureName);
private:
    void RenderVertexNormal() const;
    void RenderFaceNormal() const;

    void SendMeshDataToShader();

protected:
    virtual void TryCalculateMatrix();
    virtual void RenderModel() const;

protected:
    std::shared_ptr<Mesh> m_pMesh;
    std::shared_ptr<Shader> m_pShader;
    std::string mObjectName;
    std::string mMeshName;
    std::string mShaderName;

    GLboolean m_MatrixCacheDirty;

    glm::mat4 mToWorldMatrix;
    glm::vec3 m_position;
    glm::vec3 m_scale;
    glm::vec3 m_rotation;

    Color mEmissiveColor;

    std::function<void(void)> mAdditionalFunction;
    bool mUpdateAdditionalFunction;

    bool mDoVertexNormalDrawing;
    bool mDoFaceNormalDrawing;
    bool mDoRender;
    bool mUsingTexture;
    bool mUsingGPUUV;
    bool mFitToBox;
    Mesh::UVType mUVType;

    std::vector<std::string> mTextureSlots;
};

#endif //ENGINE_OBJECT_H