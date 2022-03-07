/* Start Header -------------------------------------------------------
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: BaseObject.cpp
Purpose: Source files for Base Object
Language: C++, g++
Platform: gcc version 9.3.0/ Linux / Opengl 4.5 supported GPU required
Project: y.kim_CS350_2
Author: Yoonki Kim, y.kim,  180002421
Creation date: Mar 6, 2022
End Header --------------------------------------------------------*/
#include <cassert>
#include "BaseObject.h"
#include "engine/graphic_misc/BoundingVolume/BasicBoundingVolumes.h"

BaseObject::BaseObject(const std::string& name, ObjectTypes type) : mType(type), mCollider(nullptr), mObjectName(name) {}

void BaseObject::BindFunction(std::function<void(BaseObject *)> func) {
    mAdditionalFunction = std::bind(func, this);
    mUpdateAdditionalFunction = true;
}
void BaseObject::RemoveFunction() {
    mAdditionalFunction = nullptr;
}

void BaseObject::SetFunctionUpdate(bool updateStatus) {
    mUpdateAdditionalFunction = updateStatus;
}

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

void BaseObject::PostRender() {
    if (mAdditionalFunction != nullptr && mUpdateAdditionalFunction) {
        mAdditionalFunction();
    }
}






