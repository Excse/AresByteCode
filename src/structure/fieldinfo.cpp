//
// Created by timo on 16.11.20.
//

#include "fieldinfo.h"

#include <iostream>

#include "classinfo.h"
#include "../utils/utils.h"

ares::FieldInfo::FieldInfo() = default;

ares::FieldInfo::~FieldInfo() = default;

bool ares::FieldInfo::hasAccessFlag(ares::FieldInfo::AccessFlag accessFlags) const {
    return m_AccessFlags & accessFlags;
}