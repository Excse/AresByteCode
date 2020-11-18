//
// Created by timo on 16.11.20.
//

#include "classfile.h"

ares::ClassFile::ClassFile() = default;

ares::ClassFile::~ClassFile() = default;

bool ares::ClassFile::isIndexValid(unsigned int index) const {
    return index > 0 && index < m_ConstantPoolCount;
}

bool ares::ClassFile::hasAccessFlag(ares::ClassFile::AccessFlag accessFlags) const {
    return m_AccessFlags & accessFlags;
}