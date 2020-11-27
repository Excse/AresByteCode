//
// Created by timo on 16.11.20.
//

#include "../../include/constantinfo.h"

ares::ConstantPoolInfo::ConstantPoolInfo() = default;

ares::ConstantPoolInfo::~ConstantPoolInfo() = default;

unsigned int ares::ConstantPoolInfo::getSize() const {
    switch (m_Tag) {
        case UTF_8:
            return 3 + m_Info.utf8Info.m_Length;
        case INTEGER:
        case FLOAT:
        case FIELD_REF:
        case METHOD_REF:
        case INTERFACE_METHOD_REF:
        case NAME_AND_TYPE:
        case DYNAMIC:
        case INVOKE_DYNAMIC:
            return 5;
        case LONG:
        case DOUBLE:
            return 9;
        case STRING:
        case CLASS:
        case METHOD_TYPE:
        case MODULE:
        case PACKAGE:
            return 3;
        case METHOD_HANDLE:
            return 4;
        default:
            abort();
    }
}
