//
// Created by timo on 16.11.20.
//

#include "../../include/methodinfo.h"

#include "../../include/attributeinfo.h"

ares::MethodInfo::MethodInfo() = default;

ares::MethodInfo::~MethodInfo() = default;

bool ares::MethodInfo::hasAccessFlag(ares::MethodInfo::AccessFlag accessFlags) const {
    return m_AccessFlags & accessFlags;
}

unsigned int ares::MethodInfo::getSize() const {
    auto size = 8;
    for(const auto &attributeInfo : m_Attributes)
        size += attributeInfo->getSize();
    return size;
}