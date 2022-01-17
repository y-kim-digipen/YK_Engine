/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: TestScene.h
Purpose: Header file for TestScene
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS300_2
Author: Yoonki Kim, y.kim,  180002421
Creation date: Nov 7, 2021
End Header --------------------------------------------------------*/
#ifndef ENGINE_TESTSCENE_H
#define ENGINE_TESTSCENE_H


#include "SceneBase.h"


class TestScene : public SceneBase{
public:
    virtual void Init() override;


    //void InitFromFile(const std::filesystem::path& filePath);

//    virtual void PreRender() override;
//    virtual void Render() override;
//    virtual void PostRender() override;
//
//    virtual void CleanUp() override;

};

#endif //ENGINE_TESTSCENE_H
