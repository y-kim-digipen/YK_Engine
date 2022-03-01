#include "SceneBase.h"

template<typename ObjectType, typename... Args>
ObjectType *SceneBase::AddObject(const std::string &objectName, Args&&... args) {
    static_assert(std::is_base_of_v<BaseObject, ObjectType>,
                  "Trying to add object not a child of BaseObject class");

    ObjectType* newObject = new ObjectType(objectName, std::forward<Args>(args)...);
    m_pDeferredObjects.template emplace(objectName, newObject);

    return newObject;
}

