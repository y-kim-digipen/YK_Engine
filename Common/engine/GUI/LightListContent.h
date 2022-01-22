/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: LightListContent.h
Purpose: Header file for LightListContent
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS300_2
Author: Yoonki Kim, y.kim,  180002421
Creation date: Nov 7, 2021
End Header --------------------------------------------------------*/
#ifndef ENGINE_LIGHTLISTCONTENT_H
#define ENGINE_LIGHTLISTCONTENT_H
#include "GUIContent.h"
namespace GUI
{
    class LightListContent : public GUI_Content {
    private:
        virtual void Init() {}
        virtual void CleanUp() {}
        void Render() final;
    };
}

#endif //ENGINE_LIGHTLISTCONTENT_H
