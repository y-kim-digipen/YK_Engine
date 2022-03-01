//
// Created by yoonki on 2/26/22.
//

#include <cassert>
#include "BaseObject.h"
#include "engine/graphic_misc/BoundingVolume/BasicBoundingVolumes.h"

BaseObject::BaseObject(const std::string& name, ObjectTypes type) : mType(type), mCollider(nullptr), mObjectName(name) {}

void BaseObject::SetCollider(Collider *collider) {
    if(collider == nullptr)
    {
        if(mCollider != nullptr)
        {
            delete mCollider;
            return;
        }
    }
    if(mCollider != nullptr && mCollider->GetColliderType() == collider->GetColliderType())
    {
        return;
    }
    if(mCollider != nullptr)
    {
        delete mCollider;
        mCollider = nullptr;
    }
    mCollider = collider;
}

Collider *BaseObject::GetCollider() {
    return mCollider;
}

bool BaseObject::DoColliding(BaseObject *other) {
    assert(other != nullptr);
    if(mCollider == nullptr || other->GetCollider() == nullptr)
    {
        return false;
    }
    return mCollider->DoCollideWith(other->GetCollider());
}

bool BaseObject::IsPrimitiveType() {
    return mType == ObjectTypes::PRIMITIVE;
}

bool BaseObject::IsMeshType() {
    return mType == ObjectTypes::MESH;
}

std::string BaseObject::GetName() const {
    return mObjectName;
}




