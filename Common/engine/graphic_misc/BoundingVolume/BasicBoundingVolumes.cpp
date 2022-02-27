//
// Created by yoonki on 2/23/22.
//
#include <glm/gtx/transform.hpp>

#include "BasicBoundingVolumes.h"
#include "engine/engine.h"
#include "engine/scene/SceneBase.h"

constexpr float PlaneEpsilon = 0.01;

ColliderTypes Collider::GetColliderType() const {
    return mType;
}

void BoundingVolume::Draw(const glm::vec3 &position, const glm::vec3 &scale) {
    auto pShader = engine::GetShader(mDrawerInfo.shaderStr);
    auto pMesh = engine::GetMesh(mDrawerInfo.meshStr);
    assert(pShader != nullptr && pMesh != nullptr);

    const GLint shaderPID = pShader->GetProgramID();

    //setting&binding buffer
    auto& attributeInfos = pShader->GetAttribInfos();
    const GLuint VAO_ID = engine::GetVAOManager().GetVAO(pShader->GetAttributeID());

    auto& VBOInfo = engine::GetVBOManager().GetVBOInfo(pMesh);

    glBindVertexArray(VAO_ID);
    for(auto& attribute : attributeInfos){
        glEnableVertexAttribArray(attribute.location);

        glBindBuffer(GL_ARRAY_BUFFER, VBOInfo.first[attribute.name]);
        glVertexAttribPointer( attribute.location,
                               attribute.DataSize,
                               attribute.DataType,
                               GL_FALSE,
                               0,
                               (void *) 0 );
    }
    //setup EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBOInfo.second);

    //Drawing Logic
    glUseProgram(shaderPID);

    const auto& pCam = engine::GetCurrentScene()->GetCurrentCamera();

    if(pShader->HasUniform("EmissiveColor"))
    {
        pShader->SetUniformVec3f("EmissiveColor", mDebuggingColor.AsVec3());
    }

    glm::mat4 viewMatrix = pCam->GetLookAtMatrix();
    glm::mat4 projectionMatrix = pCam->GetPerspectiveMatrix();
    glm::mat4 modelToWorldMatrix = GetModelToWorldMatrix();

    glm::mat4 mvpMatrix = projectionMatrix * viewMatrix * modelToWorldMatrix;
    glm::mat4 normalMatrix = modelToWorldMatrix;

    if(pShader->HasUniform("vertexTransform")) {
        pShader->SetUniformMatrix4f("vertexTransform", mvpMatrix);
    }
    if(pShader->HasUniform("vertexNormalTransform")) {
        pShader->SetUniformMatrix4f("vertexNormalTransform", normalMatrix);
    }

    glDrawElements(GL_TRIANGLES, pMesh->getVertexIndicesCount(), GL_UNSIGNED_INT, (void*) 0);

    for(auto& attribute : attributeInfos){
        glDisableVertexAttribArray(attribute.location);
    }
    glBindVertexArray(0);
    glUseProgram(0);
}

bool BoundingVolume::DoCollideWith(Collider *other)
{
    return false;
}

AABB::AABB()
{
    mDrawerInfo.meshStr = "Cube";
    mType = ColliderTypes::AABB;
}

AABB::~AABB() {}

void AABB::BuildFromVertices(const std::vector<glm::vec3> &vPos) {
    auto Min = glm::vec3( FLT_MAX );
    auto Max = glm::vec3( -FLT_MAX );
    for( unsigned int i = 0; i < vPos.size(); ++i )
    {
        glm::vec3 pt = vPos.at(i);
        Min.x = std::min( Min.x, pt.x );
        Min.y = std::min( Min.y, pt.y );
        Min.z = std::min( Min.z, pt.z );
        Max.x = std::max( Max.x, pt.x );
        Max.y = std::max( Max.y, pt.y );
        Max.z = std::max( Max.z, pt.z );
    }

    AABB_OriginalRatio = (Max - Min) / (Max - Min).y;
}

bool AABB::DoCollideWith(Collider *other) {
    switch(other->GetColliderType())
    {
        case ColliderTypes::AABB:
        {
            AABB* otherAABB = static_cast<AABB*>(other);

            glm::vec3 thisAABBCenter = GetCenter();
            glm::vec3 thisAABBHalfExtent = GetHalfExtent();
            glm::vec3 thisMinAABB = thisAABBCenter - thisAABBHalfExtent;
            glm::vec3 thisMaxAABB =  thisAABBCenter + thisAABBHalfExtent;

            glm::vec3 otherAABBCenter = otherAABB->GetCenter();
            glm::vec3 otherAABBHalfExtent = otherAABB->GetHalfExtent();
            glm::vec3 otherMinAABB = otherAABBCenter - otherAABBHalfExtent;
            glm::vec3 otherMaxAABB =  otherAABBCenter + otherAABBHalfExtent;

            for(unsigned int i = 0; i < 3; ++i)
            {
                if(thisMaxAABB[i] < otherMinAABB[i] || thisMinAABB[i] > otherMaxAABB[i])
                    return false;
            }
            return true;
        }
        case ColliderTypes::SPHERE:
        {
            Sphere* otherSphere = static_cast<Sphere*>(other);
            glm::vec3 dirSphereCenterToAABB =  otherSphere->GetCenter() - GetCenter();
            glm::vec3 halfExtent = GetHalfExtent();
            glm::vec3 closestPoint = GetCenter() + glm::vec3(std::clamp(dirSphereCenterToAABB.x, -halfExtent.x, halfExtent.x),
                                                             std::clamp(dirSphereCenterToAABB.y, -halfExtent.y, halfExtent.y),
                                                             std::clamp(dirSphereCenterToAABB.z, -halfExtent.z, halfExtent.z));
            float otherRadius = otherSphere->GetRadius();
            float squareDistCenterToCenter = glm::dot(closestPoint - otherSphere->GetCenter(), closestPoint - otherSphere->GetCenter());

            return squareDistCenterToCenter > otherRadius * otherRadius;
        }
        case ColliderTypes::POINT3D:
        {
            Point3D* otherPoint = static_cast<Point3D*>(other);
            glm::vec3 pointCoord = otherPoint->GetCoordinate();

            glm::vec3 AABBCenter = GetCenter();
            glm::vec3 AABBHalfExtent = GetHalfExtent();
            glm::vec3 minAABB = AABBCenter - AABBHalfExtent;
            glm::vec3 maxAABB =  AABBCenter + AABBHalfExtent;

            if((pointCoord.x < minAABB.x || pointCoord.x > maxAABB.x)
                || (pointCoord.y < minAABB.y || pointCoord.y > maxAABB.y)
                || (pointCoord.z < minAABB.z || pointCoord.z > maxAABB.z))
            {
                return false;
            }
            return true;
        }
        case ColliderTypes::RAY:
        {
            Ray* otherRay = static_cast<Ray*>(other);
            glm::vec3 rayStart = otherRay->GetStartPoint();
            glm::vec3 rayDir   = otherRay->GetDirection();
            glm::vec3 AABBCenter = GetCenter();
            glm::vec3 AABBHalfExtent = GetHalfExtent();
            glm::vec3 MinAABB = AABBCenter - AABBHalfExtent;
            glm::vec3 MaxAABB =  AABBCenter + AABBHalfExtent;

            glm::vec3 temp1 = (MinAABB - rayStart) / rayDir;
            glm::vec3 temp2 = (MaxAABB - rayStart) / rayDir;

            float t1 = temp1.x;
            float t2 = temp2.x;
            float t3 = temp1.y;
            float t4 = temp2.y;
            float t5 = temp1.z;
            float t6 = temp2.z;

            float tMin = std::max(
                    std::max(rayDir.x > 0 ? std::min(t1, t2) : std::max(t1, t2),
                             rayDir.y > 0 ? std::min(t3, t4) : std::max(t3, t4)),
                             rayDir.z > 0 ? std::min(t5, t6) : std::max(t5, t6));
            float tMax = std::min(
                    std::min(rayDir.x > 0 ? std::max(t1, t2) : std::min(t1, t2),
                             rayDir.y > 0 ? std::max(t3, t4) : std::min(t3, t4)),
                             rayDir.z > 0 ? std::max(t5, t6) : std::min(t5, t6));

            return tMin <= tMax;
            //todo consider di = 0 case, ignore that plane from calculation
        }
        case ColliderTypes::PLANE:
        {
            //x, y, z order
            Plane* otherPlane = static_cast<Plane*>(other);
            static std::array<char, 4> compareAxis = {0b111, 0b011, 0b010, 0b100};
            int minIdx = 0;
            float maxAbsCos = FLT_MIN;
            glm::vec4 otherPlaneData = otherPlane->GetPlaneData();
            glm::vec3 otherPlaneDir  = glm::vec3(otherPlaneData.x, otherPlaneData.y, otherPlaneData.z);
            glm::vec3 normalizedDir = glm::normalize(otherPlaneDir);

            glm::vec3 AABBCenter = GetCenter();
            glm::vec3 halfExtent = GetHalfExtent();

            for(int i = 0; i < 4; ++i)
            {
                bool posX = compareAxis[i] & 0b1;
                bool posY = compareAxis[i] & 0b10;
                bool posZ = compareAxis[i] & 0b100;

                glm::vec3 dirFromCenter = AABBCenter + glm::vec3{ posX ? halfExtent.x : -halfExtent.x, posY ? : halfExtent.y - halfExtent.y, posZ ? halfExtent.z : -halfExtent.z};
                dirFromCenter = glm::normalize(dirFromCenter);
                float cos = glm::dot(dirFromCenter, normalizedDir);
                cos = std::abs(cos);
                if(cos > maxAbsCos)
                {
                    maxAbsCos = cos;
                    minIdx = i;
                }
            }

            bool posX = compareAxis[minIdx] & 0b1;
            bool posY = compareAxis[minIdx] & 0b10;
            bool posZ = compareAxis[minIdx] & 0b100;

            glm::vec3 planeNormalStartPoint = normalizedDir * otherPlaneData.w;
            glm::vec3 p0 = AABBCenter + glm::vec3{ posX ? halfExtent.x : -halfExtent.x, posY ? : halfExtent.y - halfExtent.y, posZ ? halfExtent.z : -halfExtent.z};
            glm::vec3 p1 = AABBCenter - glm::vec3{ posX ? halfExtent.x : -halfExtent.x, posY ? : halfExtent.y - halfExtent.y, posZ ? halfExtent.z : -halfExtent.z};

            float res0 = glm::dot(p0, otherPlaneDir) + otherPlaneData.w;
            float res1 = glm::dot(p1, otherPlaneDir) + otherPlaneData.w;

            if(res0 > 0.f && res1 > 0.f)
                return true;
            else if(res0 < 0.f && res1 < 0.f)
                return true;
            return false;
        }
        default: {
            return false;
        }
    }
}

void AABB::UpdateTransformFromParentObject(Object *parent) {
    const glm::vec3 position = parent->GetPosition();
    const glm::vec3 rotation = parent->GetRotation();
    const glm::vec3 scale    = parent->GetScale();

    center = position;
    mScale = scale;
}

glm::mat4 AABB::GetModelToWorldMatrix() const {
    glm::mat4 tempToWorld{1.f};
    auto pMesh = engine::GetMesh(mDrawerInfo.meshStr);
    const auto meshBoundingBox = pMesh->GetBoundingBox();
    glm::vec3 meshSize = (meshBoundingBox.second - meshBoundingBox.first);
    glm::vec3 meshCenter = (meshBoundingBox.second + meshBoundingBox.first) * 0.5f;

    tempToWorld = glm::translate(tempToWorld, GetCenter());

    tempToWorld = glm::scale(tempToWorld, GetHalfExtent() * 2.f * mScale);

    //set the bounding box y height to 1
    tempToWorld = glm::scale(tempToWorld, 1.f / glm::vec3(meshSize.y));
    tempToWorld = glm::translate(tempToWorld, -meshCenter);
    return tempToWorld;
}

glm::vec3 AABB::GetCenter() const {
    return center;
}

glm::vec3 AABB::GetHalfExtent() const {
    return AABB_OriginalRatio * 0.5f;
}

Sphere::Sphere() {
    mDrawerInfo.meshStr = "ProceduralSphere";
    mType = ColliderTypes::SPHERE;
}

Sphere::~Sphere() {}

void Sphere::BuildFromVertices(const std::vector<glm::vec3> &vPos) {
//    glm::vec3 avg = glm::vec3(0.f);
//    const int numPos = vPos.size();
//    for( unsigned int i = 0; i < numPos; ++i )
//    {
//        avg += vPos[i];
//    }
//    avg /= numPos;
//
//    glm::mat3 covarianceMatrix;
//
//    float maxY = FLT_MIN;
//    float minY = FLT_MAX;
//    float xx, xy, xz, yy, yz, zz;
//    for( unsigned int i = 0; i < numPos; ++i )
//    {
//        const glm::vec3 currentPos = vPos[i];
//        if(currentPos.y > maxY)
//        {
//            maxY = currentPos.y;
//        }
//        if(currentPos.y < minY)
//        {
//            minY = currentPos.y;
//        }
//        xx += (currentPos.x - avg.x) * (currentPos.x - avg.x);
//        xy += (currentPos.x - avg.x) * (currentPos.y - avg.y);
//        xz += (currentPos.x - avg.x) * (currentPos.z - avg.z);
//        yy += (currentPos.y - avg.y) * (currentPos.y - avg.y);
//        yz += (currentPos.y - avg.y) * (currentPos.z - avg.z);
//        zz += (currentPos.z - avg.z) * (currentPos.z - avg.z);
//    }
//
//    xx /= numPos;
//    xy /= numPos;
//    xz /= numPos;
//    yy /= numPos;
//    yz /= numPos;
//    zz /= numPos;
//
//    covarianceMatrix[0][0] = xx;
//    covarianceMatrix[0][1] = xy;
//    covarianceMatrix[0][2] = xz;
//    covarianceMatrix[1][0] = xy;
//    covarianceMatrix[1][1] = yy;
//    covarianceMatrix[1][2] = yz;
//    covarianceMatrix[2][0] = xz;
//    covarianceMatrix[2][1] = yz;
//    covarianceMatrix[2][2] = zz;
//
//    glm::mat3 rotatedAxis = glm::mat3();
//    Jacobi(covarianceMatrix, rotatedAxis);
//
//    glm::vec3 e; int maxc = 0;
//    float maxf = std::abs(covarianceMatrix[0][0]);
//    float maxe = maxf;
//
//    if ((maxf = std::abs(covarianceMatrix[1][1])) > maxe) maxc = 1, maxe = maxf;
//    if ((maxf = std::abs(covarianceMatrix[2][2])) > maxe) maxc = 2, maxe = maxf;
//    e[0] = covarianceMatrix[0][maxc];
//    e[1] = covarianceMatrix[1][maxc];
//    e[2] = covarianceMatrix[2][maxc];
//
//    // Find the most extreme points along direction ‘e’
//    int imin, imax;
//    ExtremePointsAlongDirection(e, vPos, imin, imax);
//    glm::vec3 minpt = vPos[imin];
//    glm::vec3 maxpt = vPos[imax];
//    glm::vec3 v3Dist = maxpt - minpt;
//
//    float dist = glm::length(v3Dist);
//
//    float distCenterToMaxY = glm::abs(maxY - minY);
//    float factor = 1.f / distCenterToMaxY;
//
//
//    radius = dist * factor * 0.5f;

//    center = (minpt + maxpt) * 0.5f * factor * 0.5f;

    AABB asAABB;
    asAABB.BuildFromVertices(vPos);
    glm::vec3 temp = asAABB.GetHalfExtent();
    radius = glm::sqrt(glm::dot(asAABB.GetHalfExtent(), asAABB.GetHalfExtent()));
    std::cout << temp.x << std::endl;
}

bool Sphere::DoCollideWith(Collider *other) {
//    enum class ColliderTypes
//    {
//        AABB, SPHERE, POINT3D, PLANE, TRIANGLE, RAY
//    };
    switch(other->GetColliderType())
    {
        case ColliderTypes::AABB:
        {
            return other->DoCollideWith(this);
        }
        case ColliderTypes::SPHERE:
        {
            Sphere* otherSphere = static_cast<Sphere*>(other);
            float dist = glm::length(GetCenter() - otherSphere->GetCenter());
            float sumRadius = GetRadius() + otherSphere->GetRadius();
            return dist <= sumRadius;
        }
        case ColliderTypes::POINT3D:
        {
            Point3D* otherPoint = static_cast<Point3D*>(other);
            float dist = glm::length(GetCenter() - otherPoint->GetCoordinate());
            float squareRadius = GetRadius() * GetRadius();
            return (dist * dist) <= squareRadius;
        }
        case ColliderTypes::PLANE:
        {
            Plane* otherPlane = static_cast<Plane*>(other);
            glm::vec4 planeData = otherPlane->GetPlaneData();
            glm::vec3 dr = glm::vec3{planeData.x, planeData.y, planeData.z};
            glm::vec3 normalizedDr = glm::normalize(dr);
            glm::vec3 p0 = normalizedDr * planeData.w;

            glm::vec3 c = GetCenter();

            glm::vec3 vP0C = c - p0;

            glm::vec3 projectedC = c - glm::dot(vP0C, normalizedDr) * normalizedDr;

            float distCtoPc = glm::length(c - projectedC);

            return distCtoPc < GetRadius();
        }
       case ColliderTypes::RAY:
        {
            Ray* otherRay = static_cast<Ray*>(other);
            glm::vec3 dr = otherRay->GetDirection();
            glm::vec3 ps = otherRay->GetStartPoint();
            glm::vec3 cs = GetCenter();
            float r  = GetRadius();

            float s = (2.f * glm::dot(dr, cs)) * (2.f * dot(dr, cs)) - 4.f * (glm::dot(dr, dr) - 2.f * glm::dot(ps, cs) - r * r );
            return s > 0;
        }
        default: {
            return false;
        }
    }
}

void Sphere::SymSchur2(glm::mat3 &a, int p, int q, float &c, float& s) {
    if (std::abs(a[p][q]) > 0.0001f) {
        float r = (a[q][q] - a[p][p]) / (2.0f * a[p][q]);
        float t;
        if (r >= 0.0f)
            t = 1.0f / (r + std::sqrt(1.0f + r*r));
        else
            t = -1.0f / (-r + std::sqrt(1.0f + r*r));
        c = 1.0f / std::sqrt(1.0f + t*t);
        s=t*c;
    } else {
        c = 1.0f;
        s = 0.0f;
    }
}

void Sphere::Jacobi(glm::mat3 &covarianceMatrix, glm::mat3 &rotatedAxis) {
    constexpr int MaxIterations = 50;

    int i, j, n, p, q;
    float prevoff, c, s;
    glm::mat3 J, b, t;

    for (n = 0; n < MaxIterations; n++) {
        // Find largest off-diagonal absolute element a[p][q]
        p = 0, q = 1;
        for (i = 0; i < 3; i++) {
            for (j = 0; j < 3; j++) {
                if (i == j) continue;
                if (std::abs(covarianceMatrix[i][j]) > std::abs(covarianceMatrix[p][q])) {
                    p = i;
                    q = j;
                }
            }
        }

        // Compute the Jacobi rotation matrix J(p, q, theta)
        // (This code can be optimized for the three different cases of rotation)
        SymSchur2(covarianceMatrix, p, q, c, s);
        for (i = 0; i < 3; i++) {
            J[i][0] = J[i][1] = J[i][2] = 0.0f;
            J[i][i] = 1.0f;
        }
        J[p][p] = c;
        J[p][q] = s;
        J[q][p] = -s;
        J[q][q] = c;

        // Cumulate rotations into what will contain the eigenvectors
        rotatedAxis = rotatedAxis * J;

        // Make ‘a’ more diagonal, until just eigenvalues remain on diagonal
        covarianceMatrix = (glm::transpose(J) * covarianceMatrix) * J;

        // Compute “norm” of off-diagonal elements
        float off = 0.0f;
        for (i = 0; i < 3; i++) {
            for (j = 0; j < 3; j++) {
                if (i == j) continue;
                off += covarianceMatrix[i][j] * covarianceMatrix[i][j];
            }
        }
        /* off = sqrt(off); not needed for norm comparison */
        // Stop when norm no longer decreasing
        if (n > 2 && off >= prevoff)
            return;

        prevoff = off;
    }
}

void Sphere::ExtremePointsAlongDirection(glm::vec3 dir, const std::vector<glm::vec3> &pts, int &minIdx, int &maxIdx) {
    const int n = pts.size();
    float minproj = FLT_MAX, maxproj = -FLT_MAX;
    for(int i = 0; i < n; i++) {
        // Project vector from origin to point onto direction vector
        float proj = glm::dot(pts[i], dir);
        // Keep track of least distant point along direction vector
        if (proj < minproj) {
            minproj = proj;
            minIdx = i;
        }
        // Keep track of most distant point along direction vector
        if (proj > maxproj) {
            maxproj = proj;
            maxIdx = i;
        }
    }
}

void Sphere::UpdateTransformFromParentObject(Object *parent) {
    const glm::vec3 position = parent->GetPosition();
    const glm::vec3 rotation = parent->GetRotation();
    const glm::vec3 scale    = parent->GetScale();

    center = position;
    mScale = scale;
}

glm::mat4 Sphere::GetModelToWorldMatrix() const {
    glm::mat4 tempToWorld{1.f};
    auto pMesh = engine::GetMesh(mDrawerInfo.meshStr);
    const auto meshBoundingBox = pMesh->GetBoundingBox();
    glm::vec3 meshSize = (meshBoundingBox.second - meshBoundingBox.first);
    glm::vec3 meshCenter = (meshBoundingBox.second + meshBoundingBox.first) * 0.5f;

    tempToWorld = glm::translate(tempToWorld, GetCenter());

    tempToWorld = glm::scale(tempToWorld, glm::vec3(GetRadius() * 2.f * std::max(std::max(mScale.x, mScale.y), mScale.z)));

    //set the bounding box y height to 1
    tempToWorld = glm::scale(tempToWorld, 1.f / glm::vec3(meshSize.y));
    tempToWorld = glm::translate(tempToWorld, -meshCenter);
    return tempToWorld;
}

glm::vec3 Sphere::GetCenter() const {
    return center;
}

float Sphere::GetRadius() const {
    return radius;
}

bool Point3D::DoCollideWith(Collider *other) {
//    enum class ColliderTypes
//    {
//        AABB, SPHERE, POINT3D, PLANE, TRIANGLE, RAY
//    };
    switch(other->GetColliderType())
    {
        case ColliderTypes::AABB:
        {
            AABB* otherAABB = static_cast<AABB*>(other);
            return other->DoCollideWith(this);
        }
        case ColliderTypes::SPHERE:
        {
            Sphere* otherSphere = static_cast<Sphere*>(other);
            return other->DoCollideWith(this);
        }
        case ColliderTypes::PLANE:
        {
            Plane* otherPlane = static_cast<Plane*>(other);
            glm::vec4 planeData = otherPlane->GetPlaneData();
            glm::vec3 dr = glm::vec3{planeData.x, planeData.y, planeData.z};
            float w = glm::dot(planeData, glm::vec4(GetCoordinate().x, GetCoordinate().y, GetCoordinate().z, 1.f));

            return glm::abs(w) < PlaneEpsilon;
        }
        case ColliderTypes::TRIANGLE:
        {
            Triangle* otherTriangle = static_cast<Triangle*>(other);
            auto trianglePoints = otherTriangle->GetTrianglePoints();
            glm::vec3 v0 = GetCoordinate() - trianglePoints[0];
            glm::vec3 v1 = trianglePoints[1] - trianglePoints[0];
            glm::vec3 v2 = trianglePoints[2] - trianglePoints[0];


            float a = glm::dot(v1, v1);
            float b = glm::dot(v2, v1);
            float e = glm::dot(v0, v1);

            float c = glm::dot(v1, v2);
            float d = glm::dot(v2, v2);
            float f = glm::dot(v0, v2);

            float u = (e * d - b * f) / (a * d - b * c);
            float v = (a * f - e * c) / (a * d - b * c);

            if(u < 0 || u > 1 || v < 0 || v > 1)
            {
                return false;
            }
            return true;
        }
        default: {
            return false;
        }
    }
}

void Point3D::Draw(const glm::vec3 &position, const glm::vec3 &scale)
{
    const auto& pCam = engine::GetCurrentScene()->GetCurrentCamera();
    glm::mat4 viewMatrix = pCam->GetLookAtMatrix();
    glm::mat4 projectionMatrix = pCam->GetPerspectiveMatrix();
    glm::mat4 viewProjectionMatrix = projectionMatrix * viewMatrix;

    glm::vec4 convertedP = viewProjectionMatrix * glm::vec4(mCoordinate.x, mCoordinate.y, mCoordinate.z, 1.f);
    glm::vec3 convertedPWithProjection = glm::vec3(convertedP.x, convertedP.y, convertedP.z) / convertedP.w;

    glBegin(GL_POINT);
    glVertex3f(convertedPWithProjection.x, convertedPWithProjection.y, -1.0f);
    glEnd();
}

bool Triangle::DoCollideWith(Collider *other) {
//    enum class ColliderTypes
//    {
//        AABB, SPHERE, POINT3D, PLANE, TRIANGLE, RAY
//    };
    switch(other->GetColliderType())
    {
        case ColliderTypes::AABB:
        {
            AABB* otherAABB = static_cast<AABB*>(other);
            return otherAABB->DoCollideWith(this);
        }
        case ColliderTypes::SPHERE:
        {
            Sphere* otherSphere = static_cast<Sphere*>(other);
            return otherSphere->DoCollideWith(this);
        }
        case ColliderTypes::POINT3D:
        {
            Point3D* otherPoint = static_cast<Point3D*>(other);
            return otherPoint->DoCollideWith(this);
        }
        case ColliderTypes::RAY:
        {
            Ray* otherRay = static_cast<Ray*>(other);

            auto trianglePoints = GetTrianglePoints();

            glm::vec3 v1 = trianglePoints[1] - trianglePoints[0];
            glm::vec3 v2 = trianglePoints[2] - trianglePoints[0];

            glm::vec3 normal = glm::normalize(glm::cross(v1, v2));

            float d = glm::dot(trianglePoints[0], normal);

            Plane planeFromTriangle(glm::vec4(normal.x, normal.y, normal.z, d));

            if(!planeFromTriangle.DoCollideWith(otherRay))
            {
                return false;
            }

            float t = -(glm::dot(otherRay->GetStartPoint(), normal) + d) / glm::dot(glm::normalize(otherRay->GetDirection()), normal);
            glm::vec3 p = otherRay->GetStartPoint() + t * otherRay->GetDirection();

            Point3D point(p);

            return planeFromTriangle.DoCollideWith(&point);
        }
        default: {
            return false;
        }
    }
}

void Triangle::Draw(const glm::vec3 &position, const glm::vec3 &scale) {

}

bool Ray::DoCollideWith(Collider *other) {
//    enum class ColliderTypes
//    {
//        AABB, SPHERE, POINT3D, PLANE, TRIANGLE, RAY
//    };
    switch(other->GetColliderType())
    {
        case ColliderTypes::AABB:
        {
            AABB* otherAABB = static_cast<AABB*>(other);
            return otherAABB->DoCollideWith(this);
        }
        case ColliderTypes::SPHERE:
        {
            Sphere* otherSphere = static_cast<Sphere*>(other);
            return otherSphere->DoCollideWith(this);
        }
        case ColliderTypes::PLANE:
        {
            Plane* otherPlane = static_cast<Plane*>(other);
            glm::vec4 planeData = otherPlane->GetPlaneData();
            glm::vec3 normal = glm::vec3(planeData.x, planeData.y, planeData.z);
            float d = planeData.w;

            float t = -(glm::dot(GetStartPoint(), normal) + d) / glm::dot(glm::normalize(GetDirection()), normal);
            glm::vec3 p = GetStartPoint() + t * GetDirection();

            Point3D point(p);

            return point.DoCollideWith(otherPlane);
        }
        case ColliderTypes::TRIANGLE:
        {
            Triangle* otherTriangle = static_cast<Triangle*>(other);
            return otherTriangle->DoCollideWith(this);
        }
        default: {
            return false;
        }
    }
}

void Ray::Draw(const glm::vec3 &position, const glm::vec3 &scale) {

}

bool Plane::DoCollideWith(Collider *other) {
//    enum class ColliderTypes
//    {
//        AABB, SPHERE, POINT3D, PLANE, TRIANGLE, RAY
//    };
    switch(other->GetColliderType())
    {
        case ColliderTypes::AABB:
        {
            AABB* otherAABB = static_cast<AABB*>(other);
            return otherAABB->DoCollideWith(this);
        }
        case ColliderTypes::SPHERE:
        {
            Sphere* otherSphere = static_cast<Sphere*>(other);
            return otherSphere->DoCollideWith(this);
        }
        default: {
            return false;
        }
    }
}

void Plane::Draw(const glm::vec3 &position, const glm::vec3 &scale) {

}
