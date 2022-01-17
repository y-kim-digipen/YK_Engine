/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: GUIObject.h
Purpose: Header file for EngineInfoContent
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS300_2
Author: Yoonki Kim, y.kim,  180002421
Creation date: Nov 7, 2021
End Header --------------------------------------------------------*/

#ifndef ENGINE_GUIOBJECT_H
#define ENGINE_GUIOBJECT_H

#include "imgui.h"
namespace GUI {
    class GUI_Object {
    public:
        virtual void Init() {}

        void Update() {
            PreRender();
            Render();
            PostRender();
        }

        virtual void CleanUp() {}

    protected:
        virtual void PreRender() {}

        virtual void Render() = 0;

        virtual void PostRender() {}

        bool mIsOpen;
    };
}
#endif //ENGINE_GUIOBJECT_H
