//
// Created by timo on 16.11.20.
//

#include "../../include/attributeinfo.h"

ares::AttributeInfo::AttributeInfo() = default;

ares::AttributeInfo::~AttributeInfo() = default;

unsigned int ares::AttributeInfo::getSize() const {
    return 6 + m_Info.size();
}
