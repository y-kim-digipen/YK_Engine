/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: BasicBoundingVolumes.h
Purpose: Header files for BasicBoundingVolumes
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS350_2
Author: Yoonki Kim, y.kim,  180002421
Creation date: Mar 6, 2022
End Header --------------------------------------------------------*/
#ifndef ENGINE_BASICBOUNDINGVOLUMES_H
#define ENGINE_BASICBOUNDINGVOLUMES_H

#include <vector>
#include <glm/glm.hpp>
#include "engine/scene_objects/MeshObject.h"
#include "ColliderTypes.h"

class MeshObject;

struct ColliderDrawerInfo
{
    std::string meshStr;
    std::string shaderStr = "DiffuseShader";
};

static Color DebugColorWhite = Color(1.f, 1.f, 1.f);

class Collider
{

public:
    virtual bool DoCollideWith(Collider* other) = 0;
    virtual void Draw(const glm::vec3& position, const glm::vec3& scale) = 0;
    [[nodiscard]] ColliderTypes GetColliderType() const;
    void ResetDebuggingColor()
    {
        mDebuggingColor = DebugColorWhite.AsVec3();
        numNestingColors = 0;
    }
    void SetDebuggingColor(Color newColor)
    {
        if(mDebuggingColor == DebugColorWhite.AsVec3())
        {
            mDebuggingColor = glm::vec3();
        }
        mDebuggingColor += newColor.AsVec3();
        numNestingColors++;
    }
    Color GetDebuggingColor()
    {
        mDebuggingColor /= numNestingColors;
        return Color(mDebuggingColor.x,mDebuggingColor.y, mDebuggingColor.z );
    }

protected:
    glm::vec3 mDebuggingColor = DebugColorWhite.AsVec3();
    int numNestingColors;
    ColliderDrawerInfo mDrawerInfo;
    ColliderTypes mType;
};

class BoundingVolume : public Collider
{
public:
    BoundingVolume() = default;
    virtual ~BoundingVolume() = default;
    virtual void BuildFromVertices(const std::vector<glm::vec3>& vPos) = 0;
    void Draw(const glm::vec3& position, const glm::vec3& scale) override;
    bool DoCollideWith(Collider* other) override;
    virtual void UpdateTransformFromParentObject(MeshObject* parent) = 0;

protected:
    [[nodiscard]] virtual glm::mat4 GetModelToWorldMatrix() const = 0;
protected:

    glm::vec3 mScale{1.f};
};

class AABB : public BoundingVolume
{
public:
    AABB();
    ~AABB() override;
    void BuildFromVertices(const std::vector<glm::vec3>& vPos) override;
    bool DoCollideWith(Collider* other) override;
    void UpdateTransformFromParentObject(MeshObject* parent) override;
    [[nodiscard]] glm::vec3 GetCenter() const;
    [[nodiscard]] glm::vec3 GetHalfExtent() const;
private:
    glm::vec3 AABB_OriginalRatio;
    glm::vec3 center;
    [[nodiscard]] glm::mat4 GetModelToWorldMatrix() const override;
};

class Sphere : public BoundingVolume
{
public:
    Sphere();
    ~Sphere() override;
    void BuildFromVertices(const std::vector<glm::vec3>& vPos) override;
    bool DoCollideWith(Collider* other) override;
    void UpdateTransformFromParentObject(MeshObject* parent) override;
    [[nodiscard]] glm::vec3 GetCenter() const;
    [[nodiscard]] float GetRadius() const;
private:
    void ExtremePointsAlongDirection(glm::vec3 dir, const std::vector<glm::vec3>& pts, int& minIdx, int& maxIdx);
    void SymSchur2(glm::mat3& a, int p, int q, float &c, float &s);
    void Jacobi(glm::mat3 &covarianceMatrix, glm::mat3 &rotatedAxis);
    void IncreaseSphereToPoint(glm::vec3 point);

    [[nodiscard]] glm::mat4 GetModelToWorldMatrix() const override;

    glm::vec3 center;
    float radius;
};

class Point3D : public Collider
{
public:
    Point3D() {
        mType = ColliderTypes::POINT3D;
        mDrawerInfo.meshStr = "ProceduralSphere";
    };
    Point3D(glm::vec3 coord) : Point3D() { SetCoordinate(coord);};
    bool DoCollideWith(Collider* other) override;
    void Draw(const glm::vec3& position, const glm::vec3& scale) override;
    glm::vec3 GetCoordinate(){
        return mCoordinate;
    }
    void SetCoordinate(glm::vec3 coord){
        mCoordinate = coord;
    }


    glm::vec3 mCoordinate;
};

class Triangle : public Collider
{
public:
    Triangle() {
        mType = ColliderTypes::TRIANGLE;
    };
    Triangle(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2) : Triangle()
    {
        SetCoordinate(0, p0);
        SetCoordinate(1, p1);
        SetCoordinate(2, p2);
    };
    bool DoCollideWith(Collider* other) override;
    void Draw(const glm::vec3& position, const glm::vec3& scale) override;
    std::array<glm::vec3, 3> GetTrianglePoints()
    {
        return mPoints;
    }
    void SetCoordinate(int slot, glm::vec3 p)
    {
        assert(slot >= 0 && slot <= 3);
        mPoints[slot] = p;
    }

    std::array<glm::vec3, 3> mPoints;
};

class Ray : public Collider
{
public:
    Ray() {
        mType = ColliderTypes::RAY;
    }
    Ray(glm::vec3 startPoint, glm::vec3 dir) : Ray() {
        SetStartPoint(startPoint);
        SetDirection(dir);
    }
    bool DoCollideWith(Collider* other) override;
    void Draw(const glm::vec3& position, const glm::vec3& scale) override;
    glm::vec3 GetStartPoint()
    {
        return mStart;
    }
    glm::vec3 GetDirection()
    {
        return mDir;
    }
    void SetStartPoint(glm::vec3 start)
    {
        mStart = start;
    }
    void SetDirection(glm::vec3 dir)
    {
        mDir = dir;
    }


    //ps
    glm::vec3 mStart;
    //dr
    glm::vec3 mDir;

    float lengthDir = 1.f;
};

class Plane : public Collider
{
public:
    Plane() {
        mType = ColliderTypes::PLANE;
    };
    Plane(glm::vec4 data) : Plane() {
        SetPlaneData(data);
    }
    bool DoCollideWith(Collider* other) override;
    void Draw(const glm::vec3& position, const glm::vec3& scale) override;
    glm::vec4 GetPlaneData(){
        return m_Normal;
    }
    void SetPlaneData(glm::vec4 data)
    {
        m_Normal = data;
    }

    // (n.x, n.y, n.z, d)
    glm::vec4 m_Normal;

    float planeScale = 5.f;
};



#endif //ENGINE_BASICBOUNDINGVOLUMES_H
