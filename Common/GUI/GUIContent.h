/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: GUIContent.h
Purpose: Header file for GUIContent
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS300_2
Author: Yoonki Kim, y.kim,  180002421
Creation date: Nov 7, 2021
End Header --------------------------------------------------------*/
#ifndef ENGINE_GUICONTENT_H
#define ENGINE_GUICONTENT_H
#include "GUIObject.h"
namespace GUI {
    class GUI_Content {
    public:
//        virtual ~GUI_Content() = 0;
        virtual void Init() = 0;
        virtual void CleanUp() = 0;

        virtual void Render() = 0;

        virtual bool IsDead() { return false; }
        virtual bool DoDestroyBaseWindow() { return false; }
    };
}
#endif //ENGINE_GUICONTENT_H
