#include "classwriter.h"

#include <iostream>

#include "constantinfo.h"
#include "utils.h"

ares::ClassWriter::ClassWriter(unsigned int offset)
        : m_Offset(offset) {}

ares::ClassWriter::~ClassWriter() = default;

void ares::ClassWriter::visit_class(ClassInfo &classInfo) {
    m_ByteCode = new uint8_t[classInfo.size()];
    m_Size = classInfo.size();

    ares::write_u32(classInfo.m_MagicNumber, m_ByteCode, m_Size, m_Offset);
    ares::write_u16(classInfo.m_MinorVersion, m_ByteCode, m_Size, m_Offset);
    ares::write_u16(classInfo.m_MajorVersion, m_ByteCode, m_Size, m_Offset);

    ares::write_u16(classInfo.m_ConstantPoolCount, m_ByteCode, m_Size, m_Offset);
    for (auto &constantPoolInfo : classInfo.m_ConstantPool) {
        if (constantPoolInfo == nullptr)
            continue;

        ClassWriter::visit_classpool_info(classInfo, *constantPoolInfo);
    }

    ares::write_u16(classInfo.m_AccessFlags, m_ByteCode, m_Size, m_Offset);
    ares::write_u16(classInfo.m_ThisClass, m_ByteCode, m_Size, m_Offset);
    ares::write_u16(classInfo.m_SuperClass, m_ByteCode, m_Size, m_Offset);

    ares::write_u16(classInfo.m_InterfacesCount, m_ByteCode, m_Size, m_Offset);
    for (auto index = 0; index < classInfo.m_InterfacesCount; index++)
        ares::write_u16(classInfo.m_Interfaces[index], m_ByteCode, m_Size, m_Offset);

    ares::write_u16(classInfo.m_FieldsCount, m_ByteCode, m_Size, m_Offset);
    for (const auto &fieldInfo : classInfo.m_Fields)
        ClassWriter::visit_class_field(classInfo, *fieldInfo);

    ares::write_u16(classInfo.m_MethodCount, m_ByteCode, m_Size, m_Offset);
    for (const auto &methodInfo : classInfo.m_Methods)
        ClassWriter::visit_class_method(classInfo, *methodInfo);

    ares::write_u16(classInfo.m_AttributesCount, m_ByteCode, m_Size, m_Offset);
    for (auto &attributeInfo : classInfo.m_Attributes)
        ClassWriter::visit_class_attribute(classInfo, *attributeInfo);
}

void ares::ClassWriter::visit_classpool_info(ClassInfo &,
                                             ConstantPoolInfo &info) {
    ares::write_u8((uint8_t &) info.m_Tag, m_ByteCode, m_Size, m_Offset);

    switch (info.m_Tag) {
        case ConstantPoolInfo::CLASS:
            ares::write_u16(info.m_Info.class_info.m_NameIndex, m_ByteCode, m_Size, m_Offset);
            break;
        case ConstantPoolInfo::METHOD_REF:
        case ConstantPoolInfo::FIELD_REF:
        case ConstantPoolInfo::INTERFACE_METHOD_REF:
            ares::write_u16(info.m_Info.field_method_info.m_ClassIndex, m_ByteCode,
                            m_Size, m_Offset);
            ares::write_u16(info.m_Info.field_method_info.m_NameAndTypeIndex, m_ByteCode,
                            m_Size, m_Offset);
            break;
        case ConstantPoolInfo::STRING:
            ares::write_u16(info.m_Info.string_info.m_StringIndex, m_ByteCode, m_Size, m_Offset);
            break;
        case ConstantPoolInfo::FLOAT:
        case ConstantPoolInfo::INTEGER:
            ares::write_u32(info.m_Info.integer_float_info.m_Bytes, m_ByteCode, m_Size, m_Offset);
            break;
        case ConstantPoolInfo::LONG:
        case ConstantPoolInfo::DOUBLE:
            ares::write_u32(info.m_Info.long_double_info.m_HighBytes, m_ByteCode, m_Size, m_Offset);
            ares::write_u32(info.m_Info.long_double_info.m_LowBytes, m_ByteCode, m_Size, m_Offset);
            break;
        case ConstantPoolInfo::NAME_AND_TYPE:
            ares::write_u16(info.m_Info.name_and_type_info.m_NameIndex, m_ByteCode, m_Size, m_Offset);
            ares::write_u16(info.m_Info.name_and_type_info.m_DescriptorIndex, m_ByteCode,
                            m_Size, m_Offset);
            break;
        case ConstantPoolInfo::UTF_8:
            ares::write_u16(info.m_Info.utf8_info.m_Length, m_ByteCode, m_Size, m_Offset);
            ares::write_u8_array(info.m_Info.utf8_info.m_Bytes, info.m_Info.utf8_info.m_Length,
                                 m_ByteCode, m_Size, m_Offset);
            break;
        case ConstantPoolInfo::METHOD_HANDLE:
            ares::write_u8(info.m_Info.method_handle_info.m_ReferenceKind, m_ByteCode,
                           m_Size, m_Offset);
            ares::write_u16(info.m_Info.method_handle_info.m_ReferenceIndex, m_ByteCode,
                            m_Size, m_Offset);
            break;
        case ConstantPoolInfo::METHOD_TYPE:
            ares::write_u16(info.m_Info.method_type_info.m_DescriptorIndex, m_ByteCode,
                            m_Size, m_Offset);
            break;
        case ConstantPoolInfo::INVOKE_DYNAMIC:
        case ConstantPoolInfo::DYNAMIC:
            ares::write_u16(info.m_Info.dynamic_info.m_BoostrapMethodAttrIndex, m_ByteCode,
                            m_Size, m_Offset);
            ares::write_u16(info.m_Info.dynamic_info.m_NameAndTypeIndex, m_ByteCode,
                            m_Size, m_Offset);
            break;
        case ConstantPoolInfo::PACKAGE:
        case ConstantPoolInfo::MODULE:
            ares::write_u16(info.m_Info.module_package_info.m_NameIndex, m_ByteCode, m_Size, m_Offset);
            break;
        default:
            abort();
    }
}

void ares::ClassWriter::visit_class_interface(ClassInfo &, uint16_t) {}

void ares::ClassWriter::visit_class_field(ClassInfo &classInfo,
                                          FieldInfo &fieldInfo) {
    ares::write_u16(fieldInfo.m_AccessFlags, m_ByteCode, m_Size, m_Offset);
    ares::write_u16(fieldInfo.m_NameIndex, m_ByteCode, m_Size, m_Offset);
    ares::write_u16(fieldInfo.m_DescriptorIndex, m_ByteCode, m_Size, m_Offset);

    ares::write_u16(fieldInfo.m_AttributesCount, m_ByteCode, m_Size, m_Offset);
    for (const auto &attributeInfo : fieldInfo.m_Attributes)
        ClassWriter::visit_field_attribute(classInfo, fieldInfo, *attributeInfo);
}

void ares::ClassWriter::visit_class_method(ClassInfo &classInfo,
                                           MethodInfo &methodInfo) {
    ares::write_u16(methodInfo.m_AccessFlags, m_ByteCode, m_Size, m_Offset);
    ares::write_u16(methodInfo.m_NameIndex, m_ByteCode, m_Size, m_Offset);
    ares::write_u16(methodInfo.m_DescriptorIndex, m_ByteCode, m_Size, m_Offset);

    ares::write_u16(methodInfo.m_AttributesCount, m_ByteCode, m_Size, m_Offset);
    for (const auto &attributeInfo : methodInfo.m_Attributes)
        ClassWriter::visit_method_attribute(classInfo, methodInfo, *attributeInfo);
}

void ares::ClassWriter::visit_class_attribute(ClassInfo &,
                                              AttributeInfo &attributeInfo) {
    ares::write_u16(attributeInfo.m_AttributeNameIndex, m_ByteCode, m_Size, m_Offset);
    ares::write_u32(attributeInfo.m_AttributeLength, m_ByteCode, m_Size, m_Offset);

    for (auto &info : attributeInfo.m_Info)
        ares::write_u8(info, m_ByteCode, m_Size, m_Offset);
}

void ares::ClassWriter::visit_field_attribute(ClassInfo &classInfo,
                                              FieldInfo &,
                                              AttributeInfo &attributeInfo) {
    ClassWriter::visit_class_attribute(classInfo, attributeInfo);
}

void ares::ClassWriter::visit_method_attribute(ClassInfo &classInfo,
                                               MethodInfo &,
                                               AttributeInfo &attributeInfo) {
    ClassWriter::visit_class_attribute(classInfo, attributeInfo);
}

uint8_t *ares::ClassWriter::byte_code() const {
    return m_ByteCode;
}

//==============================================================================
// BSD 3-Clause License
//
// Copyright (c) 2025, Timo Behrend
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//==============================================================================