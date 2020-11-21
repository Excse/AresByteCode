//
// Created by timo on 16.11.20.
//

#include "classwriter.h"

#include "../utils/utils.h"

#include "../structure/constantinfo.h"

ares::ClassWriter::ClassWriter(unsigned int size, unsigned int offset)
        : m_Offset(offset) {
    m_ByteCode.resize(size);
}

ares::ClassWriter::~ClassWriter() = default;

void ares::ClassWriter::visitClass(ares::ClassInfo &classFile) {
    ares::writeU32(classFile.m_MagicNumber, m_ByteCode, m_Offset);
    ares::writeU16(classFile.m_MinorVersion, m_ByteCode, m_Offset);
    ares::writeU16(classFile.m_MajorVersion, m_ByteCode, m_Offset);

    ares::writeU16(classFile.m_ConstantPoolCount, m_ByteCode, m_Offset);
    for (auto &constantPoolInfo : classFile.m_ConstantPool) {
        if (constantPoolInfo == nullptr)
            continue;

        ClassWriter::visitClassCPInfo(classFile, *constantPoolInfo);
    }

    ares::writeU16(classFile.m_AccessFlags, m_ByteCode, m_Offset);
    ares::writeU16(classFile.m_ThisClass, m_ByteCode, m_Offset);
    ares::writeU16(classFile.m_SuperClass, m_ByteCode, m_Offset);

    ares::writeU16(classFile.m_InterfacesCount, m_ByteCode, m_Offset);
    for (auto index = 0; index < classFile.m_InterfacesCount; index++)
        ares::writeU16(classFile.m_Interfaces[index], m_ByteCode, m_Offset);

    ares::writeU16(classFile.m_FieldsCount, m_ByteCode, m_Offset);
    for (const auto &fieldInfo : classFile.m_Fields)
        ClassWriter::visitClassField(classFile, *fieldInfo);

    ares::writeU16(classFile.m_MethodCount, m_ByteCode, m_Offset);
    for (const auto &methodInfo : classFile.m_Methods)
        ClassWriter::visitClassMethod(classFile, *methodInfo);

    ares::writeU16(classFile.m_AttributesCount, m_ByteCode, m_Offset);
    for (auto &attributeInfo : classFile.m_Attributes)
        ClassWriter::visitClassAttribute(classFile, *attributeInfo);
}

void ares::ClassWriter::visitClassCPInfo(ares::ClassInfo &classFile,
                                         ares::ConstantPoolInfo &info) {
    ares::writeU8((uint8_t &) info.m_Tag, m_ByteCode, m_Offset);

    switch (info.m_Tag) {
        case ConstantPoolInfo::CLASS:
            ares::writeU16(info.m_Info.classInfo.m_NameIndex, m_ByteCode, m_Offset);
            break;
        case ConstantPoolInfo::METHOD_REF:
        case ConstantPoolInfo::FIELD_REF:
        case ConstantPoolInfo::INTERFACE_METHOD_REF:
            ares::writeU16(info.m_Info.fieldMethodInfo.m_ClassIndex, m_ByteCode, m_Offset);
            ares::writeU16(info.m_Info.fieldMethodInfo.m_NameAndTypeIndex, m_ByteCode, m_Offset);
            break;
        case ConstantPoolInfo::STRING:
            ares::writeU16(info.m_Info.stringInfo.m_StringIndex, m_ByteCode, m_Offset);
            break;
        case ConstantPoolInfo::FLOAT:
        case ConstantPoolInfo::INTEGER:
            ares::writeU32(info.m_Info.integerFloatInfo.m_Bytes, m_ByteCode, m_Offset);
            break;
        case ConstantPoolInfo::LONG:
        case ConstantPoolInfo::DOUBLE:
            ares::writeU32(info.m_Info.longDoubleInfo.m_HighBytes, m_ByteCode, m_Offset);
            ares::writeU32(info.m_Info.longDoubleInfo.m_LowBytes, m_ByteCode, m_Offset);
            break;
        case ConstantPoolInfo::NAME_AND_TYPE:
            ares::writeU16(info.m_Info.nameAndTypeInfo.m_NameIndex, m_ByteCode, m_Offset);
            ares::writeU16(info.m_Info.nameAndTypeInfo.m_DescriptorIndex, m_ByteCode, m_Offset);
            break;
        case ConstantPoolInfo::UTF_8:
            ares::writeU16(info.m_Info.utf8Info.m_Length, m_ByteCode, m_Offset);
            ares::writeU8Array(info.m_Info.utf8Info.m_Bytes, info.m_Info.utf8Info.m_Length,
                               m_ByteCode, m_Offset);
            break;
        case ConstantPoolInfo::METHOD_HANDLE:
            ares::writeU8(info.m_Info.methodHandleInfo.m_ReferenceKind, m_ByteCode, m_Offset);
            ares::writeU16(info.m_Info.methodHandleInfo.m_ReferenceIndex, m_ByteCode, m_Offset);
            break;
        case ConstantPoolInfo::METHOD_TYPE:
            ares::writeU16(info.m_Info.methodTypeInfo.m_DescriptorIndex, m_ByteCode, m_Offset);
            break;
        case ConstantPoolInfo::INVOKE_DYNAMIC:
        case ConstantPoolInfo::DYNAMIC:
            ares::writeU16(info.m_Info.dynamicInfo.m_BoostrapMethodAttrIndex, m_ByteCode, m_Offset);
            ares::writeU16(info.m_Info.dynamicInfo.m_NameAndTypeIndex, m_ByteCode, m_Offset);
            break;
        case ConstantPoolInfo::PACKAGE:
        case ConstantPoolInfo::MODULE:
            ares::writeU16(info.m_Info.modulePackageInfo.m_NameIndex, m_ByteCode, m_Offset);
            break;
        default:
            break;
    }
}

void ares::ClassWriter::visitClassInterface(ares::ClassInfo &classFile,
                                            uint16_t interface) {

}

void ares::ClassWriter::visitClassField(ares::ClassInfo &classFile,
                                        ares::FieldInfo &fieldInfo) {
    ares::writeU16(fieldInfo.m_AccessFlags, m_ByteCode, m_Offset);
    ares::writeU16(fieldInfo.m_NameIndex, m_ByteCode, m_Offset);
    ares::writeU16(fieldInfo.m_DescriptorIndex, m_ByteCode, m_Offset);

    ares::writeU16(fieldInfo.m_AttributesCount, m_ByteCode, m_Offset);
    for (const auto &attributeInfo : fieldInfo.m_Attributes)
        ClassWriter::visitFieldAttribute(classFile, fieldInfo, *attributeInfo);
}

void ares::ClassWriter::visitClassMethod(ares::ClassInfo &classFile,
                                         ares::MethodInfo &methodInfo) {
    ares::writeU16(methodInfo.m_AccessFlags, m_ByteCode, m_Offset);
    ares::writeU16(methodInfo.m_NameIndex, m_ByteCode, m_Offset);
    ares::writeU16(methodInfo.m_DescriptorIndex, m_ByteCode, m_Offset);

    ares::writeU16(methodInfo.m_AttributesCount, m_ByteCode, m_Offset);
    for (const auto &attributeInfo : methodInfo.m_Attributes)
        ClassWriter::visitMethodAttribute(classFile, methodInfo, *attributeInfo);
}

void ares::ClassWriter::visitClassAttribute(ares::ClassInfo &classFile,
                                            ares::AttributeInfo &attributeInfo) {
    ares::writeU16(attributeInfo.m_AttributeNameIndex, m_ByteCode, m_Offset);
    ares::writeU32(attributeInfo.m_AttributeLength, m_ByteCode, m_Offset);

    for (auto &info : attributeInfo.m_Info)
        ares::writeU8(info, m_ByteCode, m_Offset);
}

void ares::ClassWriter::visitFieldAttribute(ares::ClassInfo &classFile,
                                            ares::FieldInfo &fieldInfo,
                                            ares::AttributeInfo &attributeInfo) {
    ClassWriter::visitClassAttribute(classFile, attributeInfo);
}

void ares::ClassWriter::visitMethodAttribute(ares::ClassInfo &classFile,
                                             ares::MethodInfo &methodInfo,
                                             ares::AttributeInfo &attributeInfo) {
    ClassWriter::visitClassAttribute(classFile, attributeInfo);
}

void ares::ClassWriter::getByteCode(std::vector<uint8_t> &byteCode) {
    byteCode = std::vector<uint8_t>(m_ByteCode.begin(), m_ByteCode.begin() + m_Offset);
}