/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: VBOManger.h
Purpose: Header file for VBOManger
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS300_2
Author: Yoonki Kim, y.kim,  180002421
Creation date: Nov 7, 2021
End Header --------------------------------------------------------*/


#ifndef ENGINE_VBOMANAGER_H
#define ENGINE_VBOMANAGER_H
#include <map>
#include <memory>

#include <GL/gl.h>

class Mesh;
class VBOManager {
public:
    void SetUpVBO(Mesh* pMesh);
    void CleanUp();
    void ChangeVBOData(const std::string& meshName, const std::string& attribName, GLenum bufferType, GLuint bufferSize, GLvoid* bufferData);
    std::pair<std::map<std::string, GLuint>, GLuint>& GetVBOInfo(std::shared_ptr<Mesh> pMesh);
private:
    //<MeshName, <<name, VBO>, EBO>>
    std::map<std::string, std::pair<std::map<std::string, GLuint>, GLuint>> mVBOInfos;
};


#endif //ENGINE_VBOMANAGER_H
