/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: PrimitiveObject.h
Purpose: Header file for PrimitiveObject(No mesh, just have collision types)
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS350_2
Author: Yoonki Kim, y.kim,  180002421
Creation date: Mar 6, 2022
End Header --------------------------------------------------------*/
#ifndef ENGINE_PRIMITIVEOBJECT_H
#define ENGINE_PRIMITIVEOBJECT_H

#include <cassert>
#include "BaseObject.h"
#include "engine/graphic_misc/BoundingVolume/BasicBoundingVolumes.h"

class BoundingVolume;

namespace GUI{
    class GUI_Manager;
}

template<typename PrimitiveType>
class PrimitiveObject : public BaseObject{
    friend class GUI::GUI_Manager;
public:
    static_assert(std::is_base_of_v<Collider, PrimitiveType>,
            "This primitive type is not a child of collider");
    static_assert(!std::is_base_of_v<BoundingVolume, PrimitiveType>,
            "Primitive type can not be the bounding volume");

    template<typename... Args>
    PrimitiveObject(const std::string& name, Args&&... args) : BaseObject(name, ObjectTypes::PRIMITIVE)
    {
        mCollider = new PrimitiveType(args...);
        assert(IsPrimitiveType());
    };

    void Init() override{}
    void PreRender() override{}
    void Render() const override{}
    void PostRender() override{
        BaseObject::PostRender();
        glDisable(GL_CULL_FACE);
        glLineWidth(2.f);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        mCollider->Draw(glm::vec3(), glm::vec3());
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glLineWidth(1.f);
        glEnable(GL_CULL_FACE);
    };

private:

};

#endif //ENGINE_PRIMITIVEOBJECT_H
