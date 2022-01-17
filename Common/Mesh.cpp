/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Mesh.cpp
Purpose: Source file for Mesh
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS300_2
Author: Yoonki Kim, y.kim,  180002421
Creation date: Nov 7, 2021
End Header --------------------------------------------------------*/
#include <iostream>
#include <set>

#include <glm/gtc/epsilon.hpp>
#include <GL/glew.h>

#include "Engine.h"
#include "Mesh.h"
#include "Shader.h"
#include "VAOManager.h"

void Mesh::Init() {
    Engine::GetVBOManager().SetUpVBO(this);
}

// Initialize the data members in the mesh
void Mesh::ClearData()
{
    vertexBuffer.clear();
    vertexIndices.clear();
    vertexUVs.clear();
    vertexNormals.clear();
    vertexNormalDisplay.clear();
    vertexFaceNormals.clear();
    vertexFaceNormalsDisplay.clear();

    normalLength = 0.5f;

    return;
}

/////////////////////////////////////////////
/////////////////////////////////////////////
/////////////////////////////////////////////
GLfloat *Mesh::getVertexBuffer()
{
    return reinterpret_cast<GLfloat *>(vertexBuffer.data());
}

GLfloat *Mesh::getVertexNormals()
{
    return reinterpret_cast<GLfloat *>(vertexNormals.data());
}

GLfloat *Mesh::getVertexUVs()
{
    return reinterpret_cast<GLfloat *>(vertexUVs.data());
}

GLfloat *Mesh::getVertexNormalsForDisplay()
{
    return reinterpret_cast<GLfloat *>(vertexNormalDisplay.data());
}

GLuint *Mesh::getIndexBuffer()
{
    return vertexIndices.data();
}

////////////////////////////////////
////////////////////////////////////
////////////////////////////////////
unsigned int Mesh::getVertexBufferSize() const
{
    return (unsigned int) vertexBuffer.size();
}

unsigned int Mesh::getIndexBufferSize() const
{
    return (unsigned int) vertexIndices.size();
}

unsigned int Mesh::getTriangleCount() const
{
    return getIndexBufferSize() / 3;
}

unsigned int Mesh::getVertexCount() const
{
    return getVertexBufferSize();
}

unsigned int Mesh::getVertexNormalCount() const
{
    return vertexNormalDisplay.size();
}

unsigned int Mesh::getVertexIndicesCount() const {
    return vertexIndices.size();
}

unsigned int Mesh::getFaceNormalDisplayCount() const {
    return vertexFaceNormalsDisplay.size();
}

glm::vec3  Mesh::getModelScale()
{
    glm::vec3 scale = boundingBox[1] - boundingBox[0];

    if (scale.x == 0.0)
        scale.x = 1.0;

    if (scale.y == 0.0)
        scale.y = 1.0;

    if (scale.z == 0.0)
        scale.z = 1.0;

    return scale;
}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

glm::vec3  Mesh::getModelCentroid()
{
    return glm::vec3( boundingBox[0] + boundingBox[1] ) * 0.5f;
}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

glm::vec3 Mesh::getCentroidVector(glm::vec3 vVertex)
{
    return glm::normalize(vVertex - getModelCentroid());
}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

struct compareVec
{
    bool operator() (const glm::vec3& lhs, const glm::vec3& rhs) const
    {
        float V = glm::dot( lhs, rhs );
        bool bRetCode = glm::epsilonNotEqual( V, 1.0f, FLT_EPSILON);

        return bRetCode;
    }
};

/////////////////////////////////////////////////////////
int Mesh::calcVertexNormals(GLboolean bFlipNormals)
{
    int rFlag = -1;

    // vertices and indices must be populated
    if( vertexBuffer.empty() || vertexIndices.empty() )
    {
        std::cout << "Cannot calculate vertex normals for empty mesh." << std::endl;
        return rFlag;
    }

    // Pre-built vertex normals
    if( !vertexNormals.empty() )
    {
        calcVertexNormalsForDisplay(bFlipNormals);
        std::cout << "Vertex normals specified by the file. Skipping this step." << std::endl;
        return rFlag;
    }

    // Initialize vertex normals
    GLuint  numVertices = getVertexCount();
    vertexNormals.resize( numVertices, glm::vec3(0.0f) );
    vertexNormalDisplay.resize( numVertices * 2, glm::vec3(0.0f) );

    std::vector< std::set< glm::vec3, compareVec > >  vNormalSet;
    vNormalSet.resize( numVertices );

    // For every face
    uint index = 0;
    for (; index < vertexIndices.size(); )
    {
        GLuint a = vertexIndices.at(index++);
        GLuint b = vertexIndices.at(index++);
        GLuint c = vertexIndices.at(index++);

        glm::vec3  vA = vertexBuffer[a];
        glm::vec3  vB = vertexBuffer[b];
        glm::vec3  vC = vertexBuffer[c];

        // Edge vectors
        glm::vec3  E1 = vB - vA;
        glm::vec3  E2 = vC - vA;

        glm::vec3  N = glm::normalize( glm::cross( E1, E2 ) );

        if( bFlipNormals )
            N = N * -1.0f;

        // For vertex a
        vNormalSet.at( a ).insert( N );
        vNormalSet.at( b ).insert( N );
        vNormalSet.at( c ).insert( N );
    }

    // Now sum up the values per vertex
    float shrinkFactor = getModelScale().y;
    for( int index =0; index < static_cast<int>(vNormalSet.size()); ++index )
    {
        glm::vec3  vNormal(0.0f);

//        if( vNormalSet[index].size() <= 0 )
//            std::cout << "[Normal Set](" << index << ") Size = " << vNormalSet[index].size() << std::endl;

        setNormalLength(0.05f);

        auto nIt = vNormalSet[index].begin();
        while(nIt != vNormalSet[index].end())
        {
            vNormal += (*nIt);
            ++nIt;
        }

        // save vertex normal
        vertexNormals[index] = glm::normalize( vNormal );

        // save normal to display
        glm::vec3  vA = vertexBuffer[index];

        vertexNormalDisplay[2*index] = vA;
        vertexNormalDisplay[(2*index) + 1] = vA + shrinkFactor * ( normalLength * vertexNormals[index] );

    }

    // success
    rFlag = 0;

    return rFlag;
}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

void Mesh::calcVertexNormalsForDisplay([[maybe_unused]] GLboolean bFlipNormals)
{
    GLuint numVertices = getVertexCount();
    vertexNormalDisplay.resize(numVertices * 2, glm::vec3(0.0f));

    for (int iNormal = 0; iNormal < static_cast<int>(vertexNormals.size()); ++iNormal)
    {
        glm::vec3 normal = vertexNormals[iNormal] * normalLength;

        vertexNormalDisplay[2 * iNormal] = vertexBuffer[iNormal];
        vertexNormalDisplay[(2 * iNormal) + 1] = vertexBuffer[iNormal] + normal;
    }
}
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

int Mesh::calcFaceNormals(GLboolean bFlipNormals) {
    int rFlag = -1;

    // vertices and indices must be populated
    if( vertexBuffer.empty() || vertexIndices.empty() )
    {
        std::cout << "Cannot calculate vertex normals for empty mesh." << std::endl;
        return rFlag;
    }

    // Pre-built vertex normals
    if( !vertexFaceNormals.empty() )
    {
        calcVertexNormalsForDisplay(bFlipNormals);
        std::cout << "Vertex normals specified by the file. Skipping this step." << std::endl;
        return rFlag;
    }

    const int indexBufferSize = vertexIndices.size();
    vertexFaceNormals.reserve(indexBufferSize);
    vertexFaceNormalsDisplay.reserve(indexBufferSize / 3 * 2);

    int idx = 0;
    while(idx < indexBufferSize){
        glm::vec3 a = vertexBuffer[vertexIndices[idx++]];
        glm::vec3 b = vertexBuffer[vertexIndices[idx++]];
        glm::vec3 c = vertexBuffer[vertexIndices[idx++]];

        // Edge vectors
        glm::vec3  E1 = b - a;
        glm::vec3  E2 = c - a;

        glm::vec3  N = glm::normalize( glm::cross( E1, E2 ) );

        if( bFlipNormals )
            N = N * -1.0f;

        // For vertex a
        vertexFaceNormals.emplace_back( N );
    }

    float shrinkFactor = getModelScale().y;
    idx = 0;
    while(idx < indexBufferSize){
        glm::vec3 N = vertexFaceNormals[idx/3];
        glm::vec3  vA = vertexBuffer[vertexIndices[idx++]];
        glm::vec3  vB = vertexBuffer[vertexIndices[idx++]];
        glm::vec3  vC = vertexBuffer[vertexIndices[idx++]];


        glm::vec3 vD = (vA + vB + vC) / 3.f;

        // For vertex a
        vertexFaceNormalsDisplay.emplace_back( vD );
        vertexFaceNormalsDisplay.emplace_back( vD + shrinkFactor * normalLength * N );
    }
    return 0;
}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

GLfloat  & Mesh::getNormalLength()
{
    return normalLength;
}

void Mesh::setNormalLength(GLfloat nLength)
{
    normalLength = nLength;
}
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

int Mesh::calcUVs( Mesh::UVType uvType )
{
    int rFlag = -1;

    // clear any existing UV
    vertexUVs.clear();

    glm::vec3 delta = getModelScale();

    for( int nVertex = 0; nVertex < static_cast<int>(vertexBuffer.size()); ++nVertex )
    {
        glm::vec3 V = vertexBuffer[nVertex];
        glm::vec2 uv(0.0f);

        glm::vec3 normVertex = glm::vec3( (V.x - boundingBox[0].x ) / delta.x,
                                          (V.y - boundingBox[0].y ) / delta.y,
                                          (V.z - boundingBox[0].z ) / delta.z );

//        normVertex = (2.0f * normVertex) - glm::vec3(1.0f);

        glm::vec3 centroidVec = getCentroidVector(V);

        float theta(0.0f);
        float z(0.0f);
        float phi(0.0f);

        switch( uvType )
        {
            case Mesh::PLANAR_UV:
                uv.x = ( normVertex.x - (-1.0) ) / (2.0);
                uv.y = ( normVertex.y - (-1.0) ) / (2.0);
                break;

            case Mesh::CYLINDRICAL_UV:
                theta = glm::degrees( static_cast<float>( atan2(centroidVec.z, centroidVec.x ) ) );
                theta += 180.0f;

                z = (centroidVec.y + 1.0f) * 0.5f;

                uv.x = theta / 360.0f;
                uv.y = z;
                break;

            case Mesh::SPHERICAL_UV:
                theta = glm::degrees( static_cast<float>(glm::atan(centroidVec.z, centroidVec.x )) );
                theta += 180.0f;

                z = centroidVec.y;
                phi = glm::degrees(glm::acos(z / centroidVec.length() ));

                uv.x = theta / 360.0f;
                uv.y = 1.0f - ( phi / 180.0f );
                break;

            case Mesh::CUBE_MAPPED_UV:
                uv = calcCubeMap(centroidVec);
                break;
        }
        vertexUVs.push_back( uv );
    }
    preCalculatedUVs.insert(std::make_pair(uvType, vertexUVs));
    mCurrentUV = uvType;
    return rFlag;
}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

glm::vec2 Mesh::calcCubeMap(glm::vec3 vEntity)
{
    float x = vEntity.x;
    float y = vEntity.y;
    float z = vEntity.z;

    float absX = abs(x);
    float absY = abs(y);
    float absZ = abs(z);

    int isXPositive = x > 0 ? 1 : 0;
    int isYPositive = y > 0 ? 1 : 0;
    int isZPositive = z > 0 ? 1 : 0;

    float maxAxis, uc, vc;
    glm::vec2 uv = glm::vec2(0.0);

    // POSITIVE X
    if (bool(isXPositive) && (absX >= absY) && (absX >= absZ))
    {
        // u (0 to 1) goes from +z to -z
        // v (0 to 1) goes from -y to +y
        maxAxis = absX;
        uc = -z;
        vc = y;
    }

        // NEGATIVE X
    else if (!bool(isXPositive) && absX >= absY && absX >= absZ)
    {
        // u (0 to 1) goes from -z to +z
        // v (0 to 1) goes from -y to +y
        maxAxis = absX;
        uc = z;
        vc = y;
    }

        // POSITIVE Y
    else if (bool(isYPositive) && absY >= absX && absY >= absZ)
    {
        // u (0 to 1) goes from -x to +x
        // v (0 to 1) goes from +z to -z
        maxAxis = absY;
        uc = x;
        vc = -z;
    }

        // NEGATIVE Y
    else if (!bool(isYPositive) && absY >= absX && absY >= absZ)
    {
        // u (0 to 1) goes from -x to +x
        // v (0 to 1) goes from -z to +z
        maxAxis = absY;
        uc = x;
        vc = z;
    }

        // POSITIVE Z
    else if (bool(isZPositive) && absZ >= absX && absZ >= absY)
    {
        // u (0 to 1) goes from -x to +x
        // v (0 to 1) goes from -y to +y
        maxAxis = absZ;
        uc = x;
        vc = y;
    }

        // NEGATIVE Z
    else if (!bool(isZPositive) && absZ >= absX && absZ >= absY)
    {
        // u (0 to 1) goes from +x to -x
        // v (0 to 1) goes from -y to +y
        maxAxis = absZ;
        uc = -x;
        vc = y;
    }

    // Convert range from -1 to 1 to 0 to 1
    uv.s = 0.5f * (uc / maxAxis + 1.0f);
    uv.t = 0.5f * (vc / maxAxis + 1.0f);

    return uv;
}

Mesh::DrawType Mesh::GetDrawType() const {
    return drawType;
}

void Mesh::SetDrawType(Mesh::DrawType newDrawType) {
    drawType = newDrawType;
}

void Mesh::CleanUp() {
    ClearData();
}

glm::vec3 Mesh::GetMeshSize() {
    return boundingBox[1] - boundingBox[0];
}

Mesh::Mesh(std::string name) {
    mName = name;
}

std::string Mesh::GetName() {
    return mName;
}

std::pair<glm::vec3, glm::vec3> Mesh::GetBoundingBox() {
    return std::make_pair(boundingBox[0], boundingBox[1]);
}

void Mesh::MakeProcedural(ProceduralMeshType type, int stacks, int slices) {
    switch(type){
        case ProceduralMeshType::SPHERE:{
            MakeProceduralSphere(stacks, slices);
            break;
        }
        default:{
            throw;
        }
    }
    IndexingProceduralMesh(stacks, slices);
    calcVertexNormalsForDisplay();
}

void Mesh::IndexingProceduralMesh(int stacks, int slices) {
    GLushort i0 = 0, i1 = 0, i2 = 0;

    for (GLushort i = 0; i < stacks; ++i)
    {
        for (GLushort j = 0; j < slices; ++j)
        {

            /*  You need to compute the indices for the first triangle here */
            i0 = i * (slices + 1) + j;
            i1 = i0 + 1;
            i2 = i1 + slices + 1;

            vertexIndices.push_back(i0);
            vertexIndices.push_back(i2);
            vertexIndices.push_back(i1);

            /*  You need to compute the indices for the second triangle here */

            i1 = i2;
            i2 = i1 - 1;
            vertexIndices.push_back(i0);
            vertexIndices.push_back(i2);
            vertexIndices.push_back(i1);
        }
    }
}

void Mesh::MakeProceduralSphere(int stacks, int slices) {
    constexpr float rad = 0.5;

    for (int stack = 0; stack <= stacks; ++stack)
    {
        float row = (float)stack / stacks;
        float beta = PI * (row - .5f);
        for (int slice = 0; slice <= slices; ++slice)
        {
            float col = (float)slice / slices;

            float alpha = PI * 2.f - col * PI * 2.f;

            glm::vec3 pos = glm::vec3(rad * sin(alpha) * cos(beta), rad * sin(beta), rad * cos(alpha) * cos(beta));
            glm::vec3 nrm = glm::vec3(glm::normalize(pos));
            glm::vec2 uv = glm::vec2(col, 1 - row);

            vertexBuffer.emplace_back(pos);
            vertexNormals.emplace_back(nrm);
            vertexUVs.emplace_back(uv);
        }
    }
    boundingBox[0] = glm::vec3(-0.5, -0.5, -0.5);
    boundingBox[1] = glm::vec3(0.5, 0.5, 0.5);
    drawType = DrawType::TRIANGLES;
}

void Mesh::ChangeUVType(Mesh::UVType newType) {
    if(newType == mCurrentUV){
        return;
    }
    vertexUVs.clear();
    vertexUVs = preCalculatedUVs[newType];
    Engine::GetVBOManager().ChangeVBOData(mName, "vUV", GL_ARRAY_BUFFER, vertexUVs.size() * sizeof(glm::vec2), (GLvoid*)vertexUVs.data());
}

Mesh::UVType Mesh::GetCurrentUsingCPUMeshUV() const {
    return mCurrentUV;
}




