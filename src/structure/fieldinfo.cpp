//
// Created by timo on 16.11.20.
//

#include "fieldinfo.h"

#include "attributeinfo.h"

ares::FieldInfo::FieldInfo() = default;

ares::FieldInfo::~FieldInfo() = default;

bool ares::FieldInfo::hasAccessFlag(ares::FieldInfo::AccessFlag accessFlags) const {
    return m_AccessFlags & accessFlags;
}

unsigned int ares::FieldInfo::getSize() const {
    auto size = 8;
    for(const auto &attribute : m_Attributes)
        size += attribute->getSize();
    return size;
}