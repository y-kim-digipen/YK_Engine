/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: LightDetailContent.h
Purpose: Header file for LightDetailContent
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS300_2
Author: Yoonki Kim, y.kim,  180002421
Creation date: Nov 7, 2021
End Header --------------------------------------------------------*/
#ifndef ENGINE_LIGHTDETAILCONTENT_H
#define ENGINE_LIGHTDETAILCONTENT_H
#include "GUIContent.h"
#include <memory>
#include "../Light.h"
namespace GUI{
    class LightDetailContent : public GUI_Content{
    public:
        explicit LightDetailContent(std::shared_ptr<Light> target);
        void Init() override {}
        void CleanUp() override;

        void Render() override;

        bool IsDead() override;
        bool DoDestroyBaseWindow() override;
    private:
        std::shared_ptr<Light> m_pTargetLight;
    };
}




#endif //ENGINE_LIGHTDETAILCONTENT_H
