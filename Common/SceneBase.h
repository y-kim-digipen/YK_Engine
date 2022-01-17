/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: SceneBase.h
Purpose: Header file for SceneBase
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS300_2
Author: Yoonki Kim, y.kim,  180002421
Creation date: Nov 7, 2021
End Header --------------------------------------------------------*/

#ifndef ENGINE_SCENEBASE_H
#define ENGINE_SCENEBASE_H

#include <filesystem>

#include "Camera.h"
#include "Object.h"
#include "Light.h"
#include "Environment.h"

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

    std::shared_ptr<Object> AddObject(const std::string& objectName, const std::string& usingMesh, const std::string& usingShader);
    std::shared_ptr<Light> AddLight(const std::string& lightName, const std::string& usingMesh, const std::string& usingShader);
    void RemoveLight(const std::string& lightName);
    void ClearLights();

    unsigned GetNumActiveLights() const;

    std::shared_ptr<Object> GetObject(const std::string& objName);

    Environment& GetEnvironment();
protected:
    short mFocusedCameraIdx;
    std::vector<std::shared_ptr<Camera>> m_pCameras;

    Environment mEnvironment;

private:
    std::map<std::string, std::shared_ptr<Object>> m_pObjects;
    std::map<std::string, std::shared_ptr<Light>> m_pLights;
};

#endif //ENGINE_SCENEBASE_H
