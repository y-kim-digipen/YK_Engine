/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: ObjectDetailContent.h
Purpose: Header file for ObjectDetailContent
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS300_2
Author: Yoonki Kim, y.kim,  180002421
Creation date: Nov 7, 2021
End Header --------------------------------------------------------*/
#ifndef ENGINE_OBJECTDETAILCONTENT_H
#define ENGINE_OBJECTDETAILCONTENT_H

#include <memory>

#include "Object.h"
#include "GUIContent.h"
namespace GUI{
    class ObjectDetailContent : public GUI_Content{
    public:
        explicit ObjectDetailContent(std::shared_ptr<Object> target);
        void Init() override {}
        void CleanUp() override;

        void Render() override;

        bool IsDead() override;
        bool DoDestroyBaseWindow() override;
    private:
        std::shared_ptr<Object> m_pTargetObject;
    };




}



#endif //ENGINE_OBJECTDETAILCONTENT_H
