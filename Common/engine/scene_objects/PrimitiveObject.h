//
// Created by yoonki on 2/26/22.
//

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

#include "PrimitiveObject.inl"

#endif //ENGINE_PRIMITIVEOBJECT_H
