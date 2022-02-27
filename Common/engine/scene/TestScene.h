/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: TestScene.h
Purpose: Header file for TestScene
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS350_1
Author: Yoonki Kim, y.kim,  180002421
Creation date: Feb 6, 2022
End Header --------------------------------------------------------*/
#ifndef ENGINE_TESTSCENE_H
#define ENGINE_TESTSCENE_H

#include "SceneBase.h"

class TestScene : public SceneBase{
public:
    virtual void Init() override;
};

#endif //ENGINE_TESTSCENE_H
