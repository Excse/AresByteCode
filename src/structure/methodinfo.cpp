//
// Created by timo on 16.11.20.
//

#include "methodinfo.h"

ares::MethodInfo::MethodInfo() = default;

ares::MethodInfo::~MethodInfo() = default;

bool ares::MethodInfo::hasAccessFlag(ares::MethodInfo::AccessFlag accessFlags) const {
    return m_AccessFlags & accessFlags;
}