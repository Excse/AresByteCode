//
// Created by timo on 16.11.20.
//

#include "classinfo.h"

ares::ClassInfo::ClassInfo() = default;

ares::ClassInfo::~ClassInfo() = default;

bool ares::ClassInfo::isIndexValid(unsigned int index) const {
    return index > 0 && index < m_ConstantPoolCount;
}

bool ares::ClassInfo::hasAccessFlag(ares::ClassInfo::AccessFlag accessFlags) const {
    return m_AccessFlags & accessFlags;
}