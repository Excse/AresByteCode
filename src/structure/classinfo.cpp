//
// Created by timo on 16.11.20.
//

#include <iostream>
#include "../../include/classinfo.h"

#include "../../include/attributeinfo.h"
#include "../../include/constantinfo.h"
#include "../../include/methodinfo.h"
#include "../../include/fieldinfo.h"

ares::ClassInfo::ClassInfo() = default;

ares::ClassInfo::~ClassInfo() = default;

bool ares::ClassInfo::isIndexValid(unsigned int index) const {
    return index > 0 && index < m_ConstantPoolCount;
}

bool ares::ClassInfo::hasAccessFlag(ares::ClassInfo::AccessFlag accessFlags) const {
    return m_AccessFlags & accessFlags;
}

unsigned int ares::ClassInfo::getSize() const {
    auto size = 24 + 2 * m_InterfacesCount;
    for(const auto &constantPoolInfo : m_ConstantPool) {
        if(!constantPoolInfo)
            continue;
        size += constantPoolInfo->getSize();
    }
    for(const auto &fieldInfo : m_Fields)
        size += fieldInfo->getSize();
    for(const auto &methodInfo : m_Methods)
        size += methodInfo->getSize();
    for(const auto &attributeInfo : m_Attributes)
        size += attributeInfo->getSize();
    return size;
}