#include <iostream>
#include "../../include/classwriter.h"

#include "../../include/utils.h"

#include "../../include/constantinfo.h"

ares::ClassWriter::ClassWriter(unsigned int offset)
        : m_Offset(offset) {}

ares::ClassWriter::~ClassWriter() = default;

void ares::ClassWriter::visitClass(ares::ClassInfo &classInfo) {
    m_ByteCode = new uint8_t[classInfo.getSize()];
    m_Size = classInfo.getSize();

    ares::writeU32(classInfo.m_MagicNumber, m_ByteCode, m_Size, m_Offset);
    ares::writeU16(classInfo.m_MinorVersion, m_ByteCode, m_Size, m_Offset);
    ares::writeU16(classInfo.m_MajorVersion, m_ByteCode, m_Size, m_Offset);

    ares::writeU16(classInfo.m_ConstantPoolCount, m_ByteCode, m_Size, m_Offset);
    for (auto &constantPoolInfo : classInfo.m_ConstantPool) {
        if (constantPoolInfo == nullptr)
            continue;

        ClassWriter::visitClassCPInfo(classInfo, *constantPoolInfo);
    }

    ares::writeU16(classInfo.m_AccessFlags, m_ByteCode, m_Size, m_Offset);
    ares::writeU16(classInfo.m_ThisClass, m_ByteCode, m_Size, m_Offset);
    ares::writeU16(classInfo.m_SuperClass, m_ByteCode, m_Size, m_Offset);

    ares::writeU16(classInfo.m_InterfacesCount, m_ByteCode, m_Size, m_Offset);
    for (auto index = 0; index < classInfo.m_InterfacesCount; index++)
        ares::writeU16(classInfo.m_Interfaces[index], m_ByteCode, m_Size, m_Offset);

    ares::writeU16(classInfo.m_FieldsCount, m_ByteCode, m_Size, m_Offset);
    for (const auto &fieldInfo : classInfo.m_Fields)
        ClassWriter::visitClassField(classInfo, *fieldInfo);

    ares::writeU16(classInfo.m_MethodCount, m_ByteCode, m_Size, m_Offset);
    for (const auto &methodInfo : classInfo.m_Methods)
        ClassWriter::visitClassMethod(classInfo, *methodInfo);

    ares::writeU16(classInfo.m_AttributesCount, m_ByteCode, m_Size, m_Offset);
    for (auto &attributeInfo : classInfo.m_Attributes)
        ClassWriter::visitClassAttribute(classInfo, *attributeInfo);
}

void ares::ClassWriter::visitClassCPInfo(ares::ClassInfo &classInfo,
                                         ares::ConstantPoolInfo &info) {
    ares::writeU8((uint8_t &) info.m_Tag, m_ByteCode, m_Size, m_Offset);

    switch (info.m_Tag) {
        case ConstantPoolInfo::CLASS:
            ares::writeU16(info.m_Info.classInfo.m_NameIndex, m_ByteCode, m_Size, m_Offset);
            break;
        case ConstantPoolInfo::METHOD_REF:
        case ConstantPoolInfo::FIELD_REF:
        case ConstantPoolInfo::INTERFACE_METHOD_REF:
            ares::writeU16(info.m_Info.fieldMethodInfo.m_ClassIndex, m_ByteCode,
                           m_Size, m_Offset);
            ares::writeU16(info.m_Info.fieldMethodInfo.m_NameAndTypeIndex, m_ByteCode,
                           m_Size, m_Offset);
            break;
        case ConstantPoolInfo::STRING:
            ares::writeU16(info.m_Info.stringInfo.m_StringIndex, m_ByteCode, m_Size, m_Offset);
            break;
        case ConstantPoolInfo::FLOAT:
        case ConstantPoolInfo::INTEGER:
            ares::writeU32(info.m_Info.integerFloatInfo.m_Bytes, m_ByteCode, m_Size, m_Offset);
            break;
        case ConstantPoolInfo::LONG:
        case ConstantPoolInfo::DOUBLE:
            ares::writeU32(info.m_Info.longDoubleInfo.m_HighBytes, m_ByteCode, m_Size, m_Offset);
            ares::writeU32(info.m_Info.longDoubleInfo.m_LowBytes, m_ByteCode, m_Size, m_Offset);
            break;
        case ConstantPoolInfo::NAME_AND_TYPE:
            ares::writeU16(info.m_Info.nameAndTypeInfo.m_NameIndex, m_ByteCode, m_Size, m_Offset);
            ares::writeU16(info.m_Info.nameAndTypeInfo.m_DescriptorIndex, m_ByteCode,
                           m_Size, m_Offset);
            break;
        case ConstantPoolInfo::UTF_8:
            ares::writeU16(info.m_Info.utf8Info.m_Length, m_ByteCode, m_Size, m_Offset);
            ares::writeU8Array(info.m_Info.utf8Info.m_Bytes, info.m_Info.utf8Info.m_Length,
                               m_ByteCode, m_Size, m_Offset);
            break;
        case ConstantPoolInfo::METHOD_HANDLE:
            ares::writeU8(info.m_Info.methodHandleInfo.m_ReferenceKind, m_ByteCode,
                          m_Size, m_Offset);
            ares::writeU16(info.m_Info.methodHandleInfo.m_ReferenceIndex, m_ByteCode,
                           m_Size, m_Offset);
            break;
        case ConstantPoolInfo::METHOD_TYPE:
            ares::writeU16(info.m_Info.methodTypeInfo.m_DescriptorIndex, m_ByteCode,
                           m_Size, m_Offset);
            break;
        case ConstantPoolInfo::INVOKE_DYNAMIC:
        case ConstantPoolInfo::DYNAMIC:
            ares::writeU16(info.m_Info.dynamicInfo.m_BoostrapMethodAttrIndex, m_ByteCode,
                           m_Size, m_Offset);
            ares::writeU16(info.m_Info.dynamicInfo.m_NameAndTypeIndex, m_ByteCode,
                           m_Size, m_Offset);
            break;
        case ConstantPoolInfo::PACKAGE:
        case ConstantPoolInfo::MODULE:
            ares::writeU16(info.m_Info.modulePackageInfo.m_NameIndex, m_ByteCode, m_Size, m_Offset);
            break;
        default:
            abort();
    }
}

void ares::ClassWriter::visitClassInterface(ares::ClassInfo &classInfo, uint16_t interface) {}

void ares::ClassWriter::visitClassField(ares::ClassInfo &classInfo,
                                        ares::FieldInfo &fieldInfo) {
    ares::writeU16(fieldInfo.m_AccessFlags, m_ByteCode, m_Size, m_Offset);
    ares::writeU16(fieldInfo.m_NameIndex, m_ByteCode, m_Size, m_Offset);
    ares::writeU16(fieldInfo.m_DescriptorIndex, m_ByteCode, m_Size, m_Offset);

    ares::writeU16(fieldInfo.m_AttributesCount, m_ByteCode, m_Size, m_Offset);
    for (const auto &attributeInfo : fieldInfo.m_Attributes)
        ClassWriter::visitFieldAttribute(classInfo, fieldInfo, *attributeInfo);
}

void ares::ClassWriter::visitClassMethod(ares::ClassInfo &classInfo,
                                         ares::MethodInfo &methodInfo) {
    ares::writeU16(methodInfo.m_AccessFlags, m_ByteCode, m_Size, m_Offset);
    ares::writeU16(methodInfo.m_NameIndex, m_ByteCode, m_Size, m_Offset);
    ares::writeU16(methodInfo.m_DescriptorIndex, m_ByteCode, m_Size, m_Offset);

    ares::writeU16(methodInfo.m_AttributesCount, m_ByteCode, m_Size, m_Offset);
    for (const auto &attributeInfo : methodInfo.m_Attributes)
        ClassWriter::visitMethodAttribute(classInfo, methodInfo, *attributeInfo);
}

void ares::ClassWriter::visitClassAttribute(ares::ClassInfo &classInfo,
                                            ares::AttributeInfo &attributeInfo) {
    ares::writeU16(attributeInfo.m_AttributeNameIndex, m_ByteCode, m_Size, m_Offset);
    ares::writeU32(attributeInfo.m_AttributeLength, m_ByteCode, m_Size, m_Offset);

    for (auto &info : attributeInfo.m_Info)
        ares::writeU8(info, m_ByteCode, m_Size, m_Offset);
}

void ares::ClassWriter::visitFieldAttribute(ares::ClassInfo &classInfo,
                                            ares::FieldInfo &fieldInfo,
                                            ares::AttributeInfo &attributeInfo) {
    ClassWriter::visitClassAttribute(classInfo, attributeInfo);
}

void ares::ClassWriter::visitMethodAttribute(ares::ClassInfo &classInfo,
                                             ares::MethodInfo &methodInfo,
                                             ares::AttributeInfo &attributeInfo) {
    ClassWriter::visitClassAttribute(classInfo, attributeInfo);
}

uint8_t *ares::ClassWriter::getByteCode() const {
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