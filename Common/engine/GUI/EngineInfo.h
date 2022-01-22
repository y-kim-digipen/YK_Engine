/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: EngineInfo.h
Purpose: Header file for EngineInfo
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS300_2
Author: Yoonki Kim, y.kim,  180002421
Creation date: Nov 7, 2021
End Header --------------------------------------------------------*/

#ifndef ENGINE_ENGINEINFO_H
#define ENGINE_ENGINEINFO_H
#include "GUIWindow.h"

#include <string>
namespace GUI{
    class EngineInfo : public GUI_Window{
    public:
        EngineInfo(const std::string &titleName) : GUI_Window(titleName) {}

        void PreRender() final;
        void Render() final;
        void PostRender() final;
    };
}



#endif //ENGINE_ENGINEINFO_H
