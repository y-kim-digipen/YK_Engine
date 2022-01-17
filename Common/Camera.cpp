/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Camera.cpp
Purpose: Source file for Camera
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS300_2
Author: Yoonki Kim, y.kim,  180002421
Creation date: Nov 7, 2021
End Header --------------------------------------------------------*/
#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

#include "Engine.h"
#include "Input/InputManager.h"
#include "CommonConstants.h"

Camera::Camera(void) : eye(0, 0, 4), back(0, 0, 1), right(1, 0, 0), up(0, 1, 0)
{
    fov = PI * 0.3f;

    const glm::vec2 windowSize = Engine::GetWindowSize();

    float aspect = windowSize.x / windowSize.y;
    near = 0.1f;
    far = 50;
    distance = near;
    height = 2.f * distance * tan(fov / 2.f);
    width = height * aspect;
}

Camera::Camera(const glm::vec3& E, const glm::vec3& look, const glm::vec3& vp,
               float fov, float aspect, float near, float far)
        : eye(E), near(near), far(far), distance(near), fov(fov)
{
    glm::vec3 tempLook(look);
    glm::vec3 tempVp(vp);

    back = -glm::normalize(tempLook);
    right = glm::normalize(glm::cross(tempLook, tempVp));
    up = cross(back, right);

    height = 2.f * distance * tan(fov / 2.f);
    width = height * aspect;
}
glm::vec3 Camera::Eye(void) const
{
    return eye;
}
glm::vec3 Camera::Right(void) const
{
    return right;
}
glm::vec3 Camera::Up(void) const
{
    return up;
}
glm::vec3 Camera::Back(void) const
{
    return back;
}
glm::vec3 Camera::ViewportGeometry(void) const
{
    return glm::vec3{ width,height,distance };
}
float Camera::NearDistance(void) const
{
    return near;
}
float Camera::FarDistance(void) const
{
    return far;
}
Camera& Camera::Zoom(float factor)
{
    fov *= factor;

    return *this;
}

Camera& Camera::Forward(float distance_increment)
{
    eye -= distance_increment * back;
    return *this;
}

void Camera::SetPosition(glm::vec3 pos)
{
    eye = glm::vec4(pos, 1);
}

Camera& Camera::Yaw(float angle)
{
    back = glm::rotate(glm::mat4(1), angle, up) * glm::vec4(back, 1.0);
    back = glm::normalize(back);
    right = glm::rotate(glm::mat4(1), angle, up) * glm::vec4(right, 1.0);
    right = glm::normalize(right);

    return *this;
}
Camera& Camera::Pitch(float angle)
{
    back = glm::rotate(glm::mat4(1), angle, right) * glm::vec4(back, 1.0);
    back = glm::normalize(back);
    up = glm::rotate(glm::mat4(1), angle, right) * glm::vec4(up, 1.0);
    up = glm::normalize(up);

    return *this;
}
Camera& Camera::Roll(float angle)
{
    up = glm::rotate(glm::mat4(1), angle, back) * glm::vec4(up,1.0);
    up = glm::normalize(up);
    right = glm::rotate(glm::mat4(1),angle, back) * glm::vec4(right, 1.0);
    right = glm::normalize(right);

    return *this;
}

glm::mat4 Camera::GetLookAtMatrix() const
{
    const glm::vec3 e { eye.x, eye.y, eye.z };
    const glm::vec3 b { back.x, back.y, back.z };
    const glm::vec3 u { up.x, up.y, up.z };
    return glm::lookAt(e, e - b, u);
}

glm::mat4 Camera::GetPerspectiveMatrix() const
{
    return glm::perspective(fov, width / height, near, far);
}

glm::mat4 Camera::GetOrthogonalMatrix() const
{
    const glm::vec3 e{ eye.x, eye.y, eye.z };
    const glm::vec2 halfSize = glm::vec2{ std::max(width, height) } /** 0.5f*/;
    return glm::ortho(e.x - halfSize.x, e.x + halfSize.x, e.y - halfSize.y, e.y + halfSize.y, near, far);
}

void Camera::Update()
{
    if (InputManager::OnKeyDown(GLFW_KEY_LEFT))
    {
        Yaw(0.03f);
    }
    if (InputManager::OnKeyDown(GLFW_KEY_RIGHT))
    {
        Yaw(-0.03f);
    }
    if (InputManager::OnKeyDown(GLFW_KEY_UP))
    {
        Pitch(0.03f);
    }
    if (InputManager::OnKeyDown(GLFW_KEY_DOWN))
    {
        Pitch(-0.03f);
    }
    if (InputManager::OnKeyDown(GLFW_KEY_W))
    {
        eye += -0.1f * back;
    }
    if (InputManager::OnKeyDown(GLFW_KEY_S))
    {
        eye += 0.1f * back;
    }
    if (InputManager::OnKeyDown(GLFW_KEY_D))
    {
        eye += 0.1f * right;
    }
    if (InputManager::OnKeyDown(GLFW_KEY_A))
    {
        eye += -0.1f * right;
    }
    if (InputManager::OnKeyDown(GLFW_KEY_SPACE))
    {
        eye += 0.1f * up;
    }
    if (InputManager::OnKeyDown(GLFW_KEY_LEFT_CONTROL))
    {
        eye += -0.1f * up;
    }
    if(InputManager::OnKeyDown(GLFW_KEY_Q))
    {
        Roll(0.03f);
    }
    if(InputManager::OnKeyDown(GLFW_KEY_E))
    {
        Roll(-0.03f);
    }
}

glm::vec3 Camera::GetEyePosition() const {
    return eye;
}

void Camera::SetEyePosition(const glm::vec3 &pos) {
    eye = pos;
}
