/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: MeshObject.h
Purpose: Header file for MeshObject
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS350_1
Author: Yoonki Kim, y.kim,  180002421
Creation date: Feb 6, 2022
End Header --------------------------------------------------------*/

#ifndef ENGINE_MESHOBJECT_H
#define ENGINE_MESHOBJECT_H

#include <memory>
#include <functional>
#include <vector>

#include "BaseObject.h"

#include "engine/graphic_misc/Shader.h"
#include "engine/graphic_misc/Mesh.h"
#include "engine/graphic_misc/Color.h"
#include "engine/graphic_misc/BoundingVolume/ColliderTypes.h"

class Collider;
namespace GUI{
    class GUI_Manager;
}
//class CubeCaptureCamera;

class MeshObject : public BaseObject {
    friend class GUI::GUI_Manager;
public:
    MeshObject(const std::string& name);
    MeshObject(const std::string& name, std::shared_ptr<Mesh> pMesh, std::shared_ptr<Shader> pShader);
    MeshObject(const std::string& name, const std::string& meshStr, const std::string& shaderStr);

    virtual ~MeshObject();
    void Init() override;
    virtual void PreRender() override;
    void Render() const override;
    void PostRender() override;

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

    void BindFunction(std::function<void(MeshObject*)> func);
    void RemoveFunction();
    void SetFunctionUpdate(bool updateStatus);
    void SetTextureOption(bool usingTexture, bool usingGPUUV = false);

    [[nodiscard]] glm::mat4 GetObjectToWorldMatrix() const;

    void ChangeTexture(int slot, const std::string& textureName);

    virtual void TryCalculateMatrix();
    Collider* GetBoundingVolume();
private:
    void RenderVertexNormal() const;
    void RenderFaceNormal() const;

    void SendMeshDataToShader();

    void SendMaterialDataToShader() const;

protected:
    virtual void RenderModel() const;

protected:
    std::shared_ptr<Mesh> m_pMesh;
    std::shared_ptr<Shader> m_pShader;

    std::string mMeshName;
    std::string mShaderName;

    GLboolean m_MatrixCacheDirty;

    glm::mat4 mToWorldMatrix;
    glm::vec3 m_position;
    glm::vec3 m_scale;
    glm::vec3 m_rotation;

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
public:
    Color baseColor;
    float metallic = 0.f;
    float roughness = 0.5f;
};

#endif //ENGINE_MESHOBJECT_H