/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: SceneBase.h
Purpose: Header file for SceneBase
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS350_1
Author: Yoonki Kim, y.kim,  180002421
Creation date: Feb 6, 2022
End Header --------------------------------------------------------*/

#ifndef ENGINE_SCENEBASE_H
#define ENGINE_SCENEBASE_H

#include <filesystem>

#include "engine/scene_objects/Camera.h"
#include "engine/scene_objects/Object.h"
#include "engine/scene_objects/Light.h"
#include "engine/graphic_misc/Environment.h"

class SceneBase{
public:
    virtual void Init();
    virtual ~SceneBase() = default;
    //todo implement this if needed
    //void InitFromFile(const std::filesystem::path& filePath);

    virtual void PreRender();
    virtual void Render() const;
    virtual void PostRender();

    virtual void CleanUp();

    template<typename... Args>
    void AddCamera(Args... arg);
//    void AddCamera(void);
    void AddCamera(std::shared_ptr<Camera> cam = std::make_shared<Camera>());

    [[nodiscard]] std::shared_ptr<Camera> GetCurrentCamera();
    [[nodiscard]] const std::map<std::string, std::shared_ptr<Object>>& GetObjectList() const;
    [[nodiscard]] const std::map<std::string, std::shared_ptr<Light>>& GetLightList() const;

    std::shared_ptr<Object> AddObject(const std::string& objectName, const std::string& usingMesh, const std::string& usingShader = "DeferredRender");
    std::shared_ptr<Light> AddLight(const std::string& lightName, const std::string& usingMesh, const std::string& usingShader);
    void RemoveLight(const std::string& lightName);
    void ClearLights();

    unsigned GetNumActiveLights() const;

    std::shared_ptr<Object> GetObject(const std::string& objName);

    Environment& GetEnvironment();

    void UseFBO(GLuint FBOHandle, GLuint viewportWidth, GLuint viewportHeight, bool clearBuffer, GLuint viewportStartX = 0, GLuint viewportStartY = 0) const;
protected:
    short mFocusedCameraIdx;
    std::vector<std::shared_ptr<Camera>> m_pCameras;

    Environment mEnvironment;

private:
    std::map<std::string, std::shared_ptr<Object>> m_pDeferredObjects;
    std::map<std::string, std::shared_ptr<Light>> m_pForwardedObjects;

    Object* Debug_FSQ;
    Object* Result_FSQ;
    Object* Reflect_FSQ;
    Object* Combine_FSQ;
};

#endif //ENGINE_SCENEBASE_H
