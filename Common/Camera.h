/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Camera.h
Purpose: Header file for Camera
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS300_2
Author: Yoonki Kim, y.kim,  180002421
Creation date: Nov 7, 2021
End Header --------------------------------------------------------*/
#ifndef ENGINE_CAMERA_H
#define ENGINE_CAMERA_H

#include <glm/glm.hpp>
#include <GL/glew.h>

class Camera {
public:
    Camera(void);
    Camera(const glm::vec3& E, const glm::vec3& look, const glm::vec3& vp,
           float fov, float aspect, float near, float far);
    [[nodiscard]] glm::vec3 Eye(void) const;
    [[nodiscard]] glm::vec3 Right(void) const;
    [[nodiscard]] glm::vec3 Up(void) const;
    [[nodiscard]] glm::vec3 Back(void) const;
    [[nodiscard]] glm::vec3 ViewportGeometry(void) const;
    [[nodiscard]] float NearDistance(void) const;
    [[nodiscard]] float FarDistance(void) const;

    Camera& Zoom(float factor);
    Camera& Forward(float distance_increment);
    void SetPosition(glm::vec3 pos);
    Camera& Yaw(float angle);
    Camera& Pitch(float angle);
    Camera& Roll(float angle);

    glm::mat4 GetLookAtMatrix() const;
    glm::mat4 GetPerspectiveMatrix() const;
    glm::mat4 GetOrthogonalMatrix() const;

    glm::vec3 GetEyePosition() const;
    void SetEyePosition(const glm::vec3& pos);

//    virtual void SetupBuffer(Demo*);
    virtual void Update();
//    virtual void Unload() {}
private:
    glm::vec3 eye;
    glm::vec3 back, right, up;
    float width, height, near, far, distance;
    float fov;
    GLuint m_vaoid = static_cast<GLuint>(-1);
    GLuint m_ebo_hdl = static_cast<GLuint>(-1);
//    std::unique_ptr<Shader> mCameraFrustrumShader = std::make_unique<Shader>();
};



#endif //ENGINE_CAMERA_H
