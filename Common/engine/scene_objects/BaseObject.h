//
// Created by yoonki on 2/26/22.
//

#ifndef ENGINE_BASEOBJECT_H
#define ENGINE_BASEOBJECT_H

#include <string>
#include <vector>
#include <algorithm>

class Collider;

enum class ObjectTypes
{
    MESH, PRIMITIVE
};

namespace GUI{
    class GUI_Manager;
}

class BaseObject
{
    friend class GUI::GUI_Manager;
public:
    BaseObject(const std::string& name, ObjectTypes type);
    virtual void Init() = 0;
    virtual void PreRender() = 0;
    virtual void Render() const = 0;
    virtual void PostRender() = 0;

    void SetCollider(Collider* collider);

    Collider* GetCollider();
    bool DoColliding(BaseObject* other);
    bool IsPrimitiveType();
    bool IsMeshType();

    [[nodiscard]] std::string GetName() const;

    void ResetCollidedObjects()
    {
        collidedObjects.clear();
    }

    bool IsCollidedWith(BaseObject* other)
    {
        return std::find(collidedObjects.begin(), collidedObjects.end(), other) != collidedObjects.end();
    }

    void AddCollided(BaseObject* other)
    {
        collidedObjects.push_back(other);
    }
protected:
    ObjectTypes mType;
    Collider* mCollider;
    std::string mObjectName;

    std::vector<BaseObject*> collidedObjects;
};

#endif //ENGINE_BASEOBJECT_H
