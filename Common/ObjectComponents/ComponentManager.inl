//
// Created by yoonki on 9/11/21.
//
#include <iostream>
#include "ComponentManager.h"

template<typename ComponentType>
ComponentManager<ComponentType>::~ComponentManager() {
    Cleanup();
}


template<typename ComponentType>
void ComponentManager<ComponentType>::Cleanup() {
    //todo have to do something
    mNameList.clear();
//    for(auto& pComp : mComponentMap){
//        pComp.second->release();
//    }
    mComponentMap.clear();
}

template<typename ComponentType>
bool ComponentManager<ComponentType>::validate_existence(const std::string &nameStr) {
    return std::find(mNameList.begin(), mNameList.end(), nameStr) == mNameList.end();
}

template<typename ComponentType>
bool ComponentManager<ComponentType>::RemoveComponent(const std::string &nameStr) {
    if(validate_existence(nameStr) == false){
        return false;
    }
    mNameList.erase(std::find(mNameList.begin(), mNameList.end(), nameStr));
    mComponentMap.erase(nameStr);
    //for debugging purpose
    //todo change to meaningful error message
    if(validate_sizes() == false){
        throw(std::logic_error("Something went wrong! From ComponentManager.inl RemoveComponent Func"));
    }
    return true;
}

template<typename ComponentType>
bool ComponentManager<ComponentType>::validate_sizes() {
    return mNameList.size() == mNameList.size();
}

template<typename ComponentType>
size_t ComponentManager<ComponentType>::Size() const {
    if(validate_sizes() == false){
        throw(std::logic_error("Something went wrong! From ComponentManager.inl Size Func"));
    }
    return mNameList.size();
}

template<typename ComponentType>
std::shared_ptr<ComponentType> ComponentManager<ComponentType>::AddComponent(const std::string &name, ComponentType &&type, bool showGUI) {
    return AddComponent(name, std::make_shared<ComponentType>(type), showGUI);
}

template<typename ComponentType>
std::shared_ptr<ComponentType> ComponentManager<ComponentType>::AddComponent(const std::string &name, ComponentType *type, bool showGUI) {
    return AddComponent(name, std::shared_ptr<ComponentType>(type), showGUI);
}

template<typename ComponentType>
std::shared_ptr<ComponentType> ComponentManager<ComponentType>::AddComponent(const std::string &name, std::shared_ptr<ComponentType> type, bool showGUI) {
    if(validate_existence(name) == false){
        std::cerr << "Failed to add" << name << std::endl;
        return nullptr;
    }
    if(showGUI){
        mNameList.push_back(name);
    }
    std::cout << name << " successfully added" << std::endl;
    mComponentMap[name.c_str()] = type;
    return mComponentMap[name.c_str()];
}


template<typename ComponentType>
template<typename... Args>
std::shared_ptr<ComponentType> ComponentManager<ComponentType>::AddComponent(const std::string &name, Args... args, bool showGUI) {
    return AddComponent(name, std::make_shared<ComponentType>(ComponentType(args...)), showGUI);
}

template<typename ComponentType>
std::shared_ptr<ComponentType> ComponentManager<ComponentType>::GetComponent(const std::string &name) {
    auto foundItr = mComponentMap.template find(name);
    if(foundItr != mComponentMap.end()){
        return foundItr->second;
    }
    return nullptr;
}

template<typename ComponentType>
const std::vector<std::string> &ComponentManager<ComponentType>::GetNameList() {
    return mNameList;
}


//template<typename ComponentType>
//const std::vector<const char *const> &ComponentManager<ComponentType>::ListNames() {
//    return std::copy(mNameList.begin(), mNameList.end());
//}
