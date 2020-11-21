//
// Created by timo on 21.11.20.
//

#include "classpool.h"

#include "classwrapper.h"

ares::ClassPool::ClassPool(std::vector<std::shared_ptr<ClassInfo>> &classes) {
    for (const auto &clazz : classes) {
        auto classWrapper = std::make_shared<ClassWrapper>(this, clazz);
        m_Classes.emplace(classWrapper->getName(), classWrapper);
    }
}

ares::ClassPool::~ClassPool() = default;

std::shared_ptr<ares::ClassWrapper>
ares::ClassPool::getWrapper(const std::shared_ptr<ares::ClassInfo> &classInfo) {
    for (const auto &wrapper : m_Classes) {
        if (wrapper.second->m_ClassFile != classInfo)
            continue;
        return wrapper.second;
    }

    return nullptr;
}

std::shared_ptr<ares::ClassWrapper> ares::ClassPool::getWrapper(const std::string &className) {
    if(m_Classes.find(className) == m_Classes.end())
        return nullptr;
    return m_Classes[className];
}
