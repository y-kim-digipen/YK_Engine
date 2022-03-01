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
        pShader->SetUniformVec3f("EmissiveColor", mDebuggingColor);
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

            return squareDistCenterToCenter < otherRadius * otherRadius;
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
            glm::vec3 rayDir   = glm::normalize(otherRay->GetDirection());
            glm::vec3 AABBCenter = GetCenter();
            glm::vec3 AABBHalfExtent = GetHalfExtent();
            glm::vec3 MinAABB = AABBCenter - AABBHalfExtent;
            glm::vec3 MaxAABB =  AABBCenter + AABBHalfExtent;

            glm::vec3 dirFrag = glm::vec3(1.f) / glm::normalize(rayDir);

            float t1 = (MinAABB.x - rayStart.x) * dirFrag.x;
            float t2 = (MaxAABB.x - rayStart.x) * dirFrag.x;
            float t3 = (MinAABB.y - rayStart.y) * dirFrag.y;
            float t4 = (MaxAABB.y - rayStart.y) * dirFrag.y;
            float t5 = (MinAABB.z - rayStart.z) * dirFrag.z;
            float t6 = (MaxAABB.z - rayStart.z) * dirFrag.z;

            float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
            float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

            if(tmax < 0 || tmin > tmax)
            {
                return false;
            }
            return true;
            //todo consider di = 0 case, ignore that plane from calculation
        }
        case ColliderTypes::PLANE:
        {
//            //x, y, z order
            Plane* otherPlane = static_cast<Plane*>(other);

            bool allinSamePosOrNeg = true;
            bool lastPositiveSide = false;
            for(int i = 0; i < 8; ++i)
            {
                bool posX = i & 1;
                bool posY = i & 2;
                bool posZ = i & 4;
                glm::vec3 halfExtent = GetHalfExtent();
                glm::vec3 point = GetCenter() + glm::vec3{ posX ? halfExtent.x : -halfExtent.x,
                                                           posY ?  halfExtent.y : - halfExtent.y,
                                                           posZ ? halfExtent.z : -halfExtent.z};

                glm::vec3 planeNormal = glm::normalize(glm::vec3(otherPlane->GetPlaneData()));
                glm::vec4 normalizedPlaneData = glm::vec4(planeNormal.x, planeNormal.y, planeNormal.z, otherPlane->GetPlaneData().w);
                float result1 = glm::dot(glm::vec4(point.x, point.y, point.z, -1.f), normalizedPlaneData);
                bool OnPositiveSide = result1 >= 0.f;
                if(i != 0 && lastPositiveSide != OnPositiveSide)
                {
                    allinSamePosOrNeg = false;
                    break;
                }
                lastPositiveSide = OnPositiveSide;
            }
            return !allinSamePosOrNeg;
        }
        default: {
            return false;
        }
    }
}

void AABB::UpdateTransformFromParentObject(MeshObject *parent) {
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
        case ColliderTypes::RAY: {
            Ray *otherRay = static_cast<Ray *>(other);
            glm::vec3 dr = otherRay->GetDirection();
            glm::vec3 ps = otherRay->GetStartPoint();
            glm::vec3 cs = GetCenter();
            float r = GetRadius();

            glm::vec3 m = ps - cs;
            float b = glm::dot(m, glm::normalize(dr));
            float c = glm::dot(m, m) - r * r;

            if(c > 0.f && b > 0.f)
            {
                return false;
            }
            float discr = b * b - c;

            if(discr < 0)
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

void Sphere::UpdateTransformFromParentObject(MeshObject *parent) {
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

            if(u < -0.05 || u > 1.05 || v < -0.05 || v > 1.05)
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
    std::vector<glm::vec3> vPositions;
    GLuint vertexVBO;

    vPositions.push_back(GetCoordinate());

    glGenBuffers(1, &vertexVBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
    glBufferData(GL_ARRAY_BUFFER, vPositions.size() * sizeof(decltype(vPositions)::value_type),
                 vPositions.data(), GL_STATIC_DRAW);


    //////////////Drawing Logic//////////////
    auto pShader = engine::GetShader(mDrawerInfo.shaderStr);
    const GLint shaderPID = pShader->GetProgramID();


    //setting&binding buffer
    auto &attributeInfos = pShader->GetAttribInfos();
    const GLuint VAOID = engine::GetVAOManager().GetVAO(
            pShader->GetAttributeID());

    glBindVertexArray(VAOID);
    for (auto &attribute: attributeInfos) {
        if (attribute.name == "vPosition") {
            glEnableVertexAttribArray(attribute.location);


            glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
            glVertexAttribPointer(attribute.location,
                                  attribute.DataSize,
                                  attribute.DataType,
                                  GL_FALSE,
                                  0,
                                  (void *) 0);
        }
    }
    //Drawing Logic
    glUseProgram(shaderPID);

    if(pShader->HasUniform("EmissiveColor"))
    {
        pShader->SetUniformVec3f("EmissiveColor", mDebuggingColor);
    }

    GLint vTransformLoc = glGetUniformLocation(shaderPID, "vertexTransform");
    if (vTransformLoc < 0) {
        std::cerr << "Unable to find uniform variable!" << std::endl;
    }
    const auto &pCam = engine::GetCurrentScene()->GetCurrentCamera();

    glm::mat4 tempToWorld(1.f);
    glm::mat4 viewMatrix = pCam->GetLookAtMatrix();
    glm::mat4 projectionMatrix = pCam->GetPerspectiveMatrix();
    glm::mat4 mvpMatrix = projectionMatrix * viewMatrix * tempToWorld;

    glUniformMatrix4fv(vTransformLoc, 1, GL_FALSE, &mvpMatrix[0][0]);

    //Draw
    glPointSize(	5.f);
    glDrawArrays(GL_POINT, 0, vPositions.size());
    //Clean

    for (auto &attribute: attributeInfos) {
        glDisableVertexAttribArray(attribute.location);
    }
    glDeleteBuffers(1, &vertexVBO);
    glBindVertexArray(0);
    glUseProgram(0);
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

            return DoCollideWith(&point);
        }
        default: {
            return false;
        }
    }
}

void Triangle::Draw(const glm::vec3 &position, const glm::vec3 &scale) {
    std::vector<glm::vec3> vPositions;
    GLuint vertexVBO;

    vPositions.push_back(GetTrianglePoints()[0]);
    vPositions.push_back(GetTrianglePoints()[1]);
    vPositions.push_back(GetTrianglePoints()[2]);

    glGenBuffers(1, &vertexVBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
    glBufferData(GL_ARRAY_BUFFER, vPositions.size() * sizeof(decltype(vPositions)::value_type),
                 vPositions.data(), GL_STATIC_DRAW);


    //////////////Drawing Logic//////////////
    auto pShader = engine::GetShader(mDrawerInfo.shaderStr);
    const GLint shaderPID = pShader->GetProgramID();

    //setting&binding buffer
    auto &attributeInfos = pShader->GetAttribInfos();
    const GLuint VAOID = engine::GetVAOManager().GetVAO(
            pShader->GetAttributeID());

    glBindVertexArray(VAOID);
    for (auto &attribute: attributeInfos) {
        if (attribute.name == "vPosition") {
            glEnableVertexAttribArray(attribute.location);


            glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
            glVertexAttribPointer(attribute.location,
                                  attribute.DataSize,
                                  attribute.DataType,
                                  GL_FALSE,
                                  0,
                                  (void *) 0);
        }
    }
    //Drawing Logic
    glUseProgram(shaderPID);
    if(pShader->HasUniform("EmissiveColor"))
    {
        pShader->SetUniformVec3f("EmissiveColor", mDebuggingColor);
    }
    GLint vTransformLoc = glGetUniformLocation(shaderPID, "vertexTransform");
    if (vTransformLoc < 0) {
        std::cerr << "Unable to find uniform variable!" << std::endl;
    }
    const auto &pCam = engine::GetCurrentScene()->GetCurrentCamera();

    glm::mat4 tempToWorld(1.f);
    glm::mat4 viewMatrix = pCam->GetLookAtMatrix();
    glm::mat4 projectionMatrix = pCam->GetPerspectiveMatrix();
    glm::mat4 mvpMatrix = projectionMatrix * viewMatrix * tempToWorld;

    glUniformMatrix4fv(vTransformLoc, 1, GL_FALSE, &mvpMatrix[0][0]);

    //Draw
    glDrawArrays(GL_TRIANGLES, 0, vPositions.size());
    //Clean

    for (auto &attribute: attributeInfos) {
        glDisableVertexAttribArray(attribute.location);
    }
    glDeleteBuffers(1, &vertexVBO);
    glBindVertexArray(0);
    glUseProgram(0);
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
    std::vector<glm::vec3> vPositions;
    GLuint vertexVBO;

    glm::vec3 dr = glm::normalize(GetDirection());

    constexpr glm::vec3 axisX(1.f, 0.f, 0.f);
    constexpr glm::vec3 axisY(0.f, 1.f, 0.f);

    constexpr glm::vec3 arrowP1 = glm::vec3(0.0f, 0.1f, 0.0f);
    constexpr glm::vec3 arrowP2 = glm::vec3(0.0f, -0.1f, 0.0f);

    constexpr glm::vec3 arrowP1y = glm::vec3(0.1f, 0.0f, 0.0f);
    constexpr glm::vec3 arrowP2y = glm::vec3(-0.1f,0.0f,  -0.0f);

    glm::vec3 translatedArrowP1;
    glm::vec3 translatedArrowP2;

    if(dr != axisX && -dr != axisX)
    {
        glm::vec3 rotateAxis = cross(axisX, dr);
        float angle = atan2(length(rotateAxis), dot(axisX, dr));
        glm::mat3 rotateMatrix = glm::identity<glm::mat4>();
        rotateMatrix = glm::rotate(glm::mat4(rotateMatrix), angle, rotateAxis);

        translatedArrowP1 = rotateMatrix * arrowP1;
        translatedArrowP2 = rotateMatrix * arrowP2;
    }
    else
    {
        glm::vec3 rotateAxis = cross(axisY, dr);
        float angle = atan2(length(rotateAxis), dot(axisY, dr));
        glm::mat3 rotateMatrix = glm::identity<glm::mat4>();
        rotateMatrix = glm::rotate(glm::mat4(rotateMatrix), angle, rotateAxis);

        translatedArrowP1 = rotateMatrix * arrowP1;
        translatedArrowP2 = rotateMatrix * arrowP2;
    }
    vPositions.push_back(GetStartPoint());
    vPositions.push_back(GetStartPoint() + dr * lengthDir);

    vPositions.push_back(GetStartPoint() + dr * lengthDir );
    vPositions.push_back( GetStartPoint() + dr * lengthDir + (-0.1f * dr) + translatedArrowP1 );

    vPositions.push_back(GetStartPoint() + dr * lengthDir );
    vPositions.push_back( GetStartPoint() + dr * lengthDir + (-0.1f * dr) + translatedArrowP2 );


    glGenBuffers(1, &vertexVBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
    glBufferData(GL_ARRAY_BUFFER, vPositions.size() * sizeof(decltype(vPositions)::value_type),
                 vPositions.data(), GL_STATIC_DRAW);


    //////////////Drawing Logic//////////////
    auto pShader = engine::GetShader(mDrawerInfo.shaderStr);
    const GLint shaderPID = pShader->GetProgramID();

    //setting&binding buffer
    auto &attributeInfos = pShader->GetAttribInfos();
    const GLuint VAOID = engine::GetVAOManager().GetVAO(
            pShader->GetAttributeID());

    glBindVertexArray(VAOID);
    for (auto &attribute: attributeInfos) {
        if (attribute.name == "vPosition") {
            glEnableVertexAttribArray(attribute.location);


            glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
            glVertexAttribPointer(attribute.location,
                                  attribute.DataSize,
                                  attribute.DataType,
                                  GL_FALSE,
                                  0,
                                  (void *) 0);
        }
    }
    //Drawing Logic
    glUseProgram(shaderPID);
    if(pShader->HasUniform("EmissiveColor"))
    {
        pShader->SetUniformVec3f("EmissiveColor", mDebuggingColor);
    }
    GLint vTransformLoc = glGetUniformLocation(shaderPID, "vertexTransform");
    if (vTransformLoc < 0) {
        std::cerr << "Unable to find uniform variable!" << std::endl;
    }
    const auto &pCam = engine::GetCurrentScene()->GetCurrentCamera();

    glm::mat4 tempToWorld(1.f);
    glm::mat4 viewMatrix = pCam->GetLookAtMatrix();
    glm::mat4 projectionMatrix = pCam->GetPerspectiveMatrix();
    glm::mat4 mvpMatrix = projectionMatrix * viewMatrix * tempToWorld;

    glUniformMatrix4fv(vTransformLoc, 1, GL_FALSE, &mvpMatrix[0][0]);

    //Draw
    glDrawArrays(GL_LINES, 0, vPositions.size());
    //Clean

    for (auto &attribute: attributeInfos) {
        glDisableVertexAttribArray(attribute.location);
    }
    glDeleteBuffers(1, &vertexVBO);
    glBindVertexArray(0);
    glUseProgram(0);

    Point3D startPoint(GetStartPoint());
    startPoint.Draw(position, scale);
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
        case ColliderTypes::RAY:
        {
            Ray* otherRay = static_cast<Ray*>(other);

            glm::vec3 ps = otherRay->GetStartPoint();
            glm::vec3 dr = glm::normalize(otherRay->GetDirection());
            glm::vec3 n = glm::vec3(GetPlaneData());
            glm::vec3 normalizedDr = glm::normalize(dr);
            glm::vec3 p0 = normalizedDr * GetPlaneData().w;

            float t = glm::dot(-n, (ps - p0)) / glm::dot(n, dr);

            return t > 0;
        }
        default: {
            return false;
        }
    }
}

void Plane::Draw(const glm::vec3 &position, const glm::vec3 &scale) {
    constexpr float planeLength = 0.5f;

    constexpr glm::vec3 planeP0{0.f, -planeLength * 0.5f, -planeLength * 0.5f};
    constexpr glm::vec3 planeP1{0.f, -planeLength * 0.5f, +planeLength * 0.5f};
    constexpr glm::vec3 planeP2{0.f, planeLength * 0.5f, planeLength * 0.5f};
    constexpr glm::vec3 planeP3{0.f, planeLength * 0.5f, -planeLength * 0.5f};

    constexpr glm::vec3 planeP0y{-planeLength * 0.5f,0.f,  -planeLength * 0.5f};
    constexpr glm::vec3 planeP1y{-planeLength * 0.5f,0.f,  +planeLength * 0.5f};
    constexpr glm::vec3 planeP2y{planeLength * 0.5f, 0.f, planeLength * 0.5f};
    constexpr glm::vec3 planeP3y{planeLength * 0.5f, 0.f, -planeLength * 0.5f};

    constexpr glm::vec3 extendedPlaneP0{0.f, -planeLength * 0.7f, -planeLength * 0.7f};
    constexpr glm::vec3 extendedPlaneP1{0.f, -planeLength * 0.7f, planeLength * 0.7f};
    constexpr glm::vec3 extendedPlaneP2{0.f, planeLength * 0.7f, planeLength * 0.7f};
    constexpr glm::vec3 extendedPlaneP3{0.f, planeLength * 0.7f, -planeLength * 0.7f};

    constexpr glm::vec3 extendedPlaneP0y{-planeLength * 0.7f, 0.f,  -planeLength * 0.7f};
    constexpr glm::vec3 extendedPlaneP1y{-planeLength * 0.7f, 0.f,  planeLength * 0.7f};
    constexpr glm::vec3 extendedPlaneP2y{planeLength * 0.7f, 0.f, planeLength * 0.7f};
    constexpr glm::vec3 extendedPlaneP3y{planeLength * 0.7f, 0.f, -planeLength * 0.7f};

    constexpr glm::vec3 axisX = glm::vec3(1.f, 0.f, 0.f);
    constexpr glm::vec3 axisY = glm::vec3(0.f, 1.f, 0.f);

    std::vector<glm::vec3> vPositions;
    GLuint vertexVBO;

    glm::vec3 dr = glm::normalize(glm::vec3(GetPlaneData()));
    glm::vec3 p0 = dr * GetPlaneData().w;


    glm::vec3 translatedPlaneP0;
    glm::vec3 translatedPlaneP1;
    glm::vec3 translatedPlaneP2;
    glm::vec3 translatedPlaneP3;

    glm::vec3 translatedExtendedPlaneP0;
    glm::vec3 translatedExtendedPlaneP1;
    glm::vec3 translatedExtendedPlaneP2;
    glm::vec3 translatedExtendedPlaneP3;

    if(dr != axisX && -dr != axisX)

    {
        glm::vec3 rotateAxis = cross(axisX, dr);
        float angle = atan2(length(rotateAxis), dot(axisX, dr));

        glm::mat3 rotateMatrix = glm::identity<glm::mat4>();
        rotateMatrix = glm::rotate(glm::mat4(rotateMatrix), angle, rotateAxis);

        translatedPlaneP0 = p0 + rotateMatrix * planeScale * planeP0;
        translatedPlaneP1 = p0 + rotateMatrix * planeScale * planeP1;
        translatedPlaneP2 = p0 + rotateMatrix * planeScale * planeP2;
        translatedPlaneP3 = p0 + rotateMatrix * planeScale * planeP3;

        translatedExtendedPlaneP0 = p0 + rotateMatrix * planeScale * extendedPlaneP0;
        translatedExtendedPlaneP1 = p0 + rotateMatrix * planeScale * extendedPlaneP1;
        translatedExtendedPlaneP2 = p0 + rotateMatrix * planeScale * extendedPlaneP2;
        translatedExtendedPlaneP3 = p0 + rotateMatrix * planeScale * extendedPlaneP3;
    }
    else
    {
        glm::vec3 rotateAxis = cross(axisY, dr);
        float angle = atan2(length(rotateAxis), dot(axisY, dr));

        glm::mat3 rotateMatrix = glm::identity<glm::mat4>();
        rotateMatrix = glm::rotate(glm::mat4(rotateMatrix), angle, rotateAxis);

        translatedPlaneP0 = p0 + rotateMatrix * planeScale * planeP0y;
        translatedPlaneP1 = p0 + rotateMatrix * planeScale * planeP1y;
        translatedPlaneP2 = p0 + rotateMatrix * planeScale * planeP2y;
        translatedPlaneP3 = p0 + rotateMatrix * planeScale * planeP3y;

        translatedExtendedPlaneP0 = p0 + rotateMatrix * planeScale * extendedPlaneP0y;
        translatedExtendedPlaneP1 = p0 + rotateMatrix * planeScale * extendedPlaneP1y;
        translatedExtendedPlaneP2 = p0 + rotateMatrix * planeScale * extendedPlaneP2y;
        translatedExtendedPlaneP3 = p0 + rotateMatrix * planeScale * extendedPlaneP3y;
    }


    vPositions.push_back(translatedPlaneP0);
    vPositions.push_back(translatedPlaneP1);

    vPositions.push_back(translatedPlaneP1);
    vPositions.push_back(translatedPlaneP2);

    vPositions.push_back(translatedPlaneP2);
    vPositions.push_back(translatedPlaneP3);

    vPositions.push_back(translatedPlaneP3);
    vPositions.push_back(translatedPlaneP0);

    vPositions.push_back(translatedExtendedPlaneP0);
    vPositions.push_back(translatedExtendedPlaneP2);

    vPositions.push_back(translatedExtendedPlaneP1);
    vPositions.push_back(translatedExtendedPlaneP3);

    vPositions.push_back(p0);
    vPositions.push_back(p0 + planeScale * dr + (-0.7f * dr));


    glGenBuffers(1, &vertexVBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
    glBufferData(GL_ARRAY_BUFFER, vPositions.size() * sizeof(decltype(vPositions)::value_type),
                 vPositions.data(), GL_STATIC_DRAW);


    //////////////Drawing Logic//////////////
    auto pShader = engine::GetShader(mDrawerInfo.shaderStr);
    const GLint shaderPID = pShader->GetProgramID();

    //setting&binding buffer
    auto &attributeInfos = pShader->GetAttribInfos();
    const GLuint VAOID = engine::GetVAOManager().GetVAO(
            pShader->GetAttributeID());

    glBindVertexArray(VAOID);
    for (auto &attribute: attributeInfos) {
        if (attribute.name == "vPosition") {
            glEnableVertexAttribArray(attribute.location);


            glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
            glVertexAttribPointer(attribute.location,
                                  attribute.DataSize,
                                  attribute.DataType,
                                  GL_FALSE,
                                  0,
                                  (void *) 0);
        }
    }
    //Drawing Logic
    glUseProgram(shaderPID);
    if(pShader->HasUniform("EmissiveColor"))
    {
        pShader->SetUniformVec3f("EmissiveColor", mDebuggingColor);
    }
    GLint vTransformLoc = glGetUniformLocation(shaderPID, "vertexTransform");
    if (vTransformLoc < 0) {
        std::cerr << "Unable to find uniform variable!" << std::endl;
    }
    const auto &pCam = engine::GetCurrentScene()->GetCurrentCamera();

    glm::mat4 tempToWorld(1.f);

    glm::mat4 viewMatrix = pCam->GetLookAtMatrix();
    glm::mat4 projectionMatrix = pCam->GetPerspectiveMatrix();
    glm::mat4 mvpMatrix = projectionMatrix * viewMatrix * tempToWorld;

    glUniformMatrix4fv(vTransformLoc, 1, GL_FALSE, &mvpMatrix[0][0]);

    //Draw
    glDrawArrays(GL_LINES, 0, vPositions.size());
    //Clean

    for (auto &attribute: attributeInfos) {
        glDisableVertexAttribArray(attribute.location);
    }
    glDeleteBuffers(1, &vertexVBO);
    glBindVertexArray(0);
    glUseProgram(0);
}
