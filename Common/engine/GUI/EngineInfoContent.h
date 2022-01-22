/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: EngineInfoContent.h
Purpose: Header file for EngineInfoContent
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS300_2
Author: Yoonki Kim, y.kim,  180002421
Creation date: Nov 7, 2021
End Header --------------------------------------------------------*/

#ifndef ENGINE_ENGINEINFOCONTENT_H
#define ENGINE_ENGINEINFOCONTENT_H
#include "GUIContent.h"
namespace GUI
{
    class EngineInfoContent : public GUI_Content {
    public:
        EngineInfoContent() = default;
        void Init() override {}
        void CleanUp() override{}

        void Render() override;

    };
}



#endif //ENGINE_ENGINEINFOCONTENT_H
