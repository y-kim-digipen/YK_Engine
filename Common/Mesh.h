/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Mesh.h
Purpose: Header file for Mesh
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS300_2
Author: Yoonki Kim, y.kim,  180002421
Creation date: Nov 7, 2021
End Header --------------------------------------------------------*/
#ifndef SIMPLE_OBJ_SCENE_MESH_H
#define SIMPLE_OBJ_SCENE_MESH_H

#include <vector>

#include <glm/glm.hpp>
#include <GL/glut.h>
#include <memory>

#include "VBOManager.h"
#include "ObjectComponents/ComponentBase.h"

class Mesh : public ComponentBase
{
public:
    enum class ProceduralMeshType{
        SPHERE,
        COUNT,
    };
public:
    friend class OBJReader;
    Mesh(std::string name);
    void Init();
    void CleanUp();

    void MakeProcedural(ProceduralMeshType type, int stacks, int slices);
    // Get attribute values buffer
    GLfloat *getVertexBuffer();             // attribute 0
    GLfloat *getVertexNormals();            // attribute 1
    GLfloat *getVertexUVs();                // attribute 2

    GLfloat *getVertexNormalsForDisplay();  // attribute 0

    [[nodiscard]] unsigned int getVertexBufferSize() const;
    [[nodiscard]] unsigned int getVertexCount() const;
    [[nodiscard]] unsigned int getVertexNormalCount() const;
    [[nodiscard]] unsigned int getFaceNormalDisplayCount() const;
    [[nodiscard]] unsigned int getVertexIndicesCount() const;

    // Get vertex index buffer
    GLuint *getIndexBuffer();
    [[nodiscard]] unsigned int getIndexBufferSize() const;
    [[nodiscard]] unsigned int getTriangleCount() const;

    glm::vec3   getModelScale();
    glm::vec3   getModelCentroid();
    glm::vec3   getCentroidVector( glm::vec3 vVertex );

    glm::vec3 GetMeshSize();
    std::pair<glm::vec3, glm::vec3> GetBoundingBox();
    std::string GetName();

    GLfloat  &  getNormalLength();
    void setNormalLength( GLfloat nLength );

    // initialize the data members
    void ClearData();

    // calculate vertex normals
    int calcVertexNormals(GLboolean bFlipNormals = false);

    // calculate the "display" normals
    void calcVertexNormalsForDisplay(GLboolean bFlipNormals = false);

    // calculate the face normals
    int calcFaceNormals(GLboolean bFlipNormals = false);

    // calculate texture coordinates
    enum UVType { PLANAR_UV = 0,
                  CYLINDRICAL_UV,
                  SPHERICAL_UV,
                  CUBE_MAPPED_UV};

    int         calcUVs( Mesh::UVType uvType = Mesh::PLANAR_UV );
    glm::vec2   calcCubeMap( glm::vec3 vEntity );

    //todo add more if needed
    enum DrawType{  POINT = GL_POINT,
                    POINTS = GL_POINTS,
                    LINE = GL_LINE,
                    LINES = GL_LINES,
                    LINE_STRIP = GL_LINE_STRIP,
                    LINE_LOOP = GL_LINE_LOOP,
                    TRIANGLES = GL_TRIANGLES,
                    TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
                    TRIANGLE_FAN = GL_TRIANGLE_FAN,
    };

    [[nodiscard]] DrawType GetDrawType() const;
    void SetDrawType(DrawType drawType);
    void ChangeUVType(Mesh::UVType newType);
    Mesh::UVType GetCurrentUsingCPUMeshUV() const;

private:
    void IndexingProceduralMesh(int stacks, int slices);
    void MakeProceduralSphere(int stacks, int slices);

private:
    std::vector<glm::vec3>    vertexBuffer;
    std::vector<GLuint>       vertexIndices;
    std::vector<glm::vec2>    vertexUVs;
    std::vector<glm::vec3>    vertexNormals, vertexNormalDisplay;
    std::vector<glm::vec3>    vertexFaceNormals, vertexFaceNormalsDisplay;

    std::map<Mesh::UVType, std::vector<glm::vec2>> preCalculatedUVs;

    glm::vec3               boundingBox[2];
    GLfloat                 normalLength;

    DrawType                drawType = DrawType::TRIANGLE_STRIP;

    Mesh::UVType            mCurrentUV;

    std::string mName;
public:
    friend void VBOManager::SetUpVBO(Mesh*);
};


#endif //SIMPLE_OBJ_SCENE_MESH_H
