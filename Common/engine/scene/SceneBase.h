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
#include "engine/scene_objects/MeshObject.h"
#include "engine/scene_objects/Light.h"
#include "engine/graphic_misc/Environment.h"

class BaseObject;

class SceneBase{
private:
    class DebugColorPicker{
    public:
        DebugColorPicker();
        void Reset();
        Color PickOne();

    private:
        int currentPointer;
        std::vector<Color> mSelectableColor;
    } mColorPicker;

public:
    virtual void Init();
    virtual ~SceneBase();
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
    [[nodiscard]] const std::map<std::string, BaseObject*>& GetObjectList() const;
    [[nodiscard]] const std::map<std::string, Light*>& GetLightList() const;

    template<typename ObjectType, typename... Args>
    ObjectType* AddObject(const std::string& objectName, Args&&... args);
    Light* AddLight(const std::string& lightName, const std::string& usingMesh, const std::string& usingShader);
    void RemoveLight(const std::string& lightName);
    void ClearLights();

    unsigned GetNumActiveLights() const;

    BaseObject* GetObject(const std::string& objName);

    Environment& GetEnvironment();

    void UseFBO(GLuint FBOHandle, GLuint viewportWidth, GLuint viewportHeight, bool clearBuffer, GLuint viewportStartX = 0, GLuint viewportStartY = 0) const;
protected:
    short mFocusedCameraIdx;
    std::vector<std::shared_ptr<Camera>> m_pCameras;

    Environment mEnvironment;

private:
    std::map<std::string, BaseObject*> m_pDeferredObjects;
    std::map<std::string, Light*> m_pForwardedObjects;

    MeshObject* Debug_FSQ;
    MeshObject* Result_FSQ;
    MeshObject* Reflect_FSQ;
    MeshObject* Combine_FSQ;
};

#include "SceneBase.inl"
#endif //ENGINE_SCENEBASE_H
