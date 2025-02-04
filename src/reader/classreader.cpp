#include "classreader.h"

#include <iostream>

#include "utils.h"

ares::ClassReader::ClassReader(unsigned int offset)
        : m_Offset(offset) {}

ares::ClassReader::~ClassReader() = default;

void ares::ClassReader::visitClass(ares::ClassInfo &classInfo) {
    readMagicNumber(classInfo);
    readClassVersion(classInfo);
    readConstantPool(classInfo);
    readAccessFlags(classInfo);
    readThisClass(classInfo);
    readSuperClass(classInfo);
    readInterfaces(classInfo);
    readFields(classInfo);
    readMethods(classInfo);
    readClassAttributes(classInfo);
}

void ares::ClassReader::readClassAttributes(ares::ClassInfo &classInfo) {
    if (ares::readU16(classInfo.m_AttributesCount, classInfo.m_ByteCode,
                      classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the attribute count." << std::endl;
        abort();
    }

    classInfo.m_Attributes = std::vector<std::shared_ptr<AttributeInfo>>(
            classInfo.m_AttributesCount);
    for (auto &attribute : classInfo.m_Attributes) {
        auto attributeInfo = std::make_shared<AttributeInfo>();
        ClassReader::visitClassAttribute(classInfo, *attributeInfo);

        attribute = attributeInfo;
    }
}

void ares::ClassReader::visitClassAttribute(ares::ClassInfo &classInfo,
                                            ares::AttributeInfo &attributeInfo) {
    if (ares::readU16(attributeInfo.m_AttributeNameIndex, classInfo.m_ByteCode,
                      classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the name index of the attribute." << std::endl;
        abort();
    }

    if (ares::readU32(attributeInfo.m_AttributeLength, classInfo.m_ByteCode,
                      classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the length of the attribute." << std::endl;
        abort();
    }

    attributeInfo.m_Info = std::vector<uint8_t>(attributeInfo.m_AttributeLength);
    for (auto &info : attributeInfo.m_Info) {
        if (ares::readU8(info, classInfo.m_ByteCode,
                         classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
            std::cerr << "Couldn't read the info of the attribute." << std::endl;
            abort();
        }
    }
}

void ares::ClassReader::readMagicNumber(ClassInfo &classInfo) {
    if (ares::readU32(classInfo.m_MagicNumber, classInfo.m_ByteCode,
                      classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the magic number of the class file." << std::endl;
        abort();
    }
}

void ares::ClassReader::readClassVersion(ClassInfo &classInfo) {
    if (ares::readU16(classInfo.m_MinorVersion, classInfo.m_ByteCode,
                      classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the minor version of the class file." << std::endl;
        abort();
    }

    if (ares::readU16(classInfo.m_MajorVersion, classInfo.m_ByteCode,
                      classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the major version of the class file." << std::endl;
        abort();
    }

    classInfo.m_ClassVersion = ClassInfo::UNDEFINED;
    if (classInfo.m_MajorVersion >= ClassInfo::VERSION_1_1
        && classInfo.m_MajorVersion <= ClassInfo::VERSION_15) {
        classInfo.m_ClassVersion = ClassInfo::ClassVersion(classInfo.m_MajorVersion);
    }
}

void ares::ClassReader::readConstantPool(ClassInfo &classInfo) {
    if (ares::readU16(classInfo.m_ConstantPoolCount, classInfo.m_ByteCode,
                      classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cout << "Couldn't read the constant pool count of this class file" << std::endl;
        abort();
    }

    classInfo.m_ConstantPool = std::vector<std::shared_ptr<ConstantPoolInfo>>(
            classInfo.m_ConstantPoolCount - 1);
    for (auto index = 0; index < classInfo.m_ConstantPoolCount - 1; index++) {
        auto info = std::make_shared<ConstantPoolInfo>();
        ClassReader::visitClassCPInfo(classInfo, *info);

        classInfo.m_ConstantPool[index] = info;

        if (info->m_Tag == ConstantPoolInfo::DOUBLE || info->m_Tag == ConstantPoolInfo::LONG)
            index++;
    }
}

void ares::ClassReader::visitClassCPInfo(ares::ClassInfo &classInfo,
                                         ares::ConstantPoolInfo &info) {
    uint8_t infoTag = 0;
    if (ares::readU8(infoTag, classInfo.m_ByteCode,
                     classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the tag of the constant pool info of this class file"
                  << std::endl;
        abort();
    }

    if (infoTag >= ConstantPoolInfo::UTF_8 && infoTag <= ConstantPoolInfo::PACKAGE
        && infoTag != 13 && infoTag != 14)
        info.m_Tag = ConstantPoolInfo::ConstantTag(infoTag);
    else
        info.m_Tag = ConstantPoolInfo::UNDEFINED;

    switch (info.m_Tag) {
        case ConstantPoolInfo::CLASS:
            readClassInfo(classInfo, info.m_Info.classInfo);
            break;
        case ConstantPoolInfo::METHOD_REF:
        case ConstantPoolInfo::FIELD_REF:
        case ConstantPoolInfo::INTERFACE_METHOD_REF:
            readFieldMethodInfo(classInfo, info.m_Info.fieldMethodInfo);
            break;
        case ConstantPoolInfo::STRING:
            readStringInfo(classInfo, info.m_Info.stringInfo);
            break;
        case ConstantPoolInfo::FLOAT:
        case ConstantPoolInfo::INTEGER:
            readFloatInteger(classInfo, info.m_Info.integerFloatInfo);
            break;
        case ConstantPoolInfo::LONG:
        case ConstantPoolInfo::DOUBLE:
            readDoubleLong(classInfo, info.m_Info.longDoubleInfo);
            break;
        case ConstantPoolInfo::NAME_AND_TYPE:
            readNameAndType(classInfo, info.m_Info.nameAndTypeInfo);
            break;
        case ConstantPoolInfo::UTF_8:
            readUTF8Info(classInfo, info.m_Info.utf8Info);
            break;
        case ConstantPoolInfo::METHOD_HANDLE:
            readMethodHandle(classInfo, info.m_Info.methodHandleInfo);
            break;
        case ConstantPoolInfo::METHOD_TYPE:
            readMethodType(classInfo, info.m_Info.methodTypeInfo);
            break;
        case ConstantPoolInfo::INVOKE_DYNAMIC:
        case ConstantPoolInfo::DYNAMIC:
            readDynamic(classInfo, info.m_Info.dynamicInfo);
            break;
        case ConstantPoolInfo::PACKAGE:
        case ConstantPoolInfo::MODULE:
            readModulePackage(classInfo, info.m_Info.modulePackageInfo);
            break;
        default:
            break;
    }
}

void ares::ClassReader::readClassInfo(ares::ClassInfo &classInfo,
                                      ConstantInfo::ClassInfo &info) {
    if (ares::readU16(info.m_NameIndex, classInfo.m_ByteCode,
                      classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the name index of the class pool info." << std::endl;
        abort();
    }
}

void ares::ClassReader::readUTF8Info(ares::ClassInfo &classInfo,
                                     ConstantInfo::UTF8Info &info) {
    if (ares::readU16(info.m_Length, classInfo.m_ByteCode,
                      classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the length of the class pool info." << std::endl;
        abort();
    }

    info.m_Bytes = new uint8_t[info.m_Length];
    if (ares::readU8Array(info.m_Bytes, info.m_Length, classInfo.m_ByteCode,
                          classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the bytes of the class pool info." << std::endl;
        abort();
    }
}

void ares::ClassReader::readFieldMethodInfo(ares::ClassInfo &classInfo,
                                            ConstantInfo::FieldMethodInfo &info) {
    if (ares::readU16(info.m_ClassIndex, classInfo.m_ByteCode,
                      classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the class index of the class pool info." << std::endl;
        abort();
    }

    if (ares::readU16(info.m_NameAndTypeIndex, classInfo.m_ByteCode,
                      classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the name and type index of the class pool info." << std::endl;
        abort();
    }
}

void ares::ClassReader::readNameAndType(ares::ClassInfo &classInfo,
                                        ConstantInfo::NameAndTypeInfo &info) {
    if (ares::readU16(info.m_NameIndex, classInfo.m_ByteCode,
                      classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the name index of the class pool info." << std::endl;
        abort();
    }

    if (ares::readU16(info.m_DescriptorIndex, classInfo.m_ByteCode,
                      classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the descriptor index of the class pool info." << std::endl;
        abort();
    }
}

void ares::ClassReader::readStringInfo(ares::ClassInfo &classInfo,
                                       ConstantInfo::StringInfo &info) {
    if (ares::readU16(info.m_StringIndex, classInfo.m_ByteCode,
                      classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the string index of the class pool info." << std::endl;
        abort();
    }
}

void ares::ClassReader::readDoubleLong(ares::ClassInfo &classInfo,
                                       ConstantInfo::DoubleLongInfo &info) {
    if (ares::readU32(info.m_HighBytes, classInfo.m_ByteCode,
                      classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the high bytes of the class pool info." << std::endl;
        abort();
    }

    if (ares::readU32(info.m_LowBytes, classInfo.m_ByteCode,
                      classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the low bytes of the class pool info." << std::endl;
        abort();
    }
}

void ares::ClassReader::readFloatInteger(ares::ClassInfo &classInfo,
                                         ConstantInfo::FloatIntegerInfo &info) {
    if (ares::readU32(info.m_Bytes, classInfo.m_ByteCode,
                      classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the bytes of the class pool info." << std::endl;
        abort();
    }
}

void ares::ClassReader::readMethodType(ares::ClassInfo &classInfo,
                                       ConstantInfo::MethodTypeInfo &info) {
    if (ares::readU16(info.m_DescriptorIndex, classInfo.m_ByteCode,
                      classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the descriptor index of the class pool info." << std::endl;
        abort();
    }
}

void ares::ClassReader::readMethodHandle(ares::ClassInfo &classInfo,
                                         ConstantInfo::MethodHandleInfo &info) {
    if (ares::readU8(info.m_ReferenceKind, classInfo.m_ByteCode,
                     classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the reference kind of the class pool info." << std::endl;
        abort();
    }

    if (ares::readU16(info.m_ReferenceIndex, classInfo.m_ByteCode,
                      classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the reference index of the class pool info." << std::endl;
        abort();
    }
}

void ares::ClassReader::readDynamic(ares::ClassInfo &classInfo,
                                    ConstantInfo::DynamicInfo &info) {
    if (ares::readU16(info.m_BoostrapMethodAttrIndex, classInfo.m_ByteCode,
                      classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the bootstrap method attribute index of the class pool info."
                  << std::endl;
        abort();
    }

    if (ares::readU16(info.m_NameAndTypeIndex, classInfo.m_ByteCode,
                      classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the name and type index for the class pool info." << std::endl;
        abort();
    }
}

void ares::ClassReader::readModulePackage(ares::ClassInfo &classInfo,
                                          ConstantInfo::ModulePackageInfo &info) {
    if (ares::readU16(info.m_NameIndex, classInfo.m_ByteCode,
                      classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the name index of the class pool info." << std::endl;
        abort();
    }
}

void ares::ClassReader::readAccessFlags(ClassInfo &classInfo) {
    if (ares::readU16(classInfo.m_AccessFlags, classInfo.m_ByteCode,
                      classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the access flags of the class file." << std::endl;
        abort();
    }
}

void ares::ClassReader::readThisClass(ClassInfo &classInfo) {
    if (ares::readU16(classInfo.m_ThisClass, classInfo.m_ByteCode,
                      classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the \"this class\" of this class file." << std::endl;
        abort();
    }
}

void ares::ClassReader::readSuperClass(ClassInfo &classInfo) {
    if (ares::readU16(classInfo.m_SuperClass, classInfo.m_ByteCode,
                      classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the \"super class\" of the class file." << std::endl;
        abort();
    }
}

void ares::ClassReader::readInterfaces(ClassInfo &classInfo) {
    if (ares::readU16(classInfo.m_InterfacesCount, classInfo.m_ByteCode,
                      classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the interface count of the class file." << std::endl;
        abort();
    }

    classInfo.m_Interfaces = std::vector<uint16_t>(classInfo.m_InterfacesCount);
    for (auto &interface : classInfo.m_Interfaces) {
        if (ares::readU16(interface, classInfo.m_ByteCode,
                          classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
            std::cerr << "Couldn't read the interface of this class." << std::endl;
            abort();
        }
    }
}

void ares::ClassReader::visitClassInterface(ares::ClassInfo &, uint16_t) {}

void ares::ClassReader::readFields(ClassInfo &classInfo) {
    if (ares::readU16(classInfo.m_FieldsCount, classInfo.m_ByteCode,
                      classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the field count of the class file." << std::endl;
        abort();
    }

    classInfo.m_Fields = std::vector<std::shared_ptr<FieldInfo>>(classInfo.m_FieldsCount);
    for (auto &field : classInfo.m_Fields) {
        auto fieldInfo = std::make_shared<FieldInfo>();
        ClassReader::visitClassField(classInfo, *fieldInfo);

        field = fieldInfo;
    }
}

void ares::ClassReader::visitClassField(ares::ClassInfo &classInfo,
                                        ares::FieldInfo &fieldInfo) {
    if (ares::readU16(fieldInfo.m_AccessFlags, classInfo.m_ByteCode,
                      classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the access flags of the field." << std::endl;
        abort();
    }

    if (ares::readU16(fieldInfo.m_NameIndex, classInfo.m_ByteCode,
                      classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the name index of the field." << std::endl;
        abort();
    }

    if (ares::readU16(fieldInfo.m_DescriptorIndex, classInfo.m_ByteCode,
                      classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the descriptor index of the field." << std::endl;
        abort();
    }

    readFieldAttributes(classInfo, fieldInfo);
}

void ares::ClassReader::readFieldAttributes(ares::ClassInfo &classInfo,
                                            ares::FieldInfo &fieldInfo) {
    if (ares::readU16(fieldInfo.m_AttributesCount, classInfo.m_ByteCode,
                      classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the attribute count." << std::endl;
        abort();
    }

    fieldInfo.m_Attributes = std::vector<std::shared_ptr<AttributeInfo>>(
            fieldInfo.m_AttributesCount);
    for (auto &attribute : fieldInfo.m_Attributes) {
        auto attributeInfo = std::make_shared<AttributeInfo>();
        ClassReader::visitFieldAttribute(classInfo, fieldInfo, *attributeInfo);

        attribute = attributeInfo;
    }
}

void ares::ClassReader::visitFieldAttribute(ares::ClassInfo &classInfo,
                                            ares::FieldInfo &,
                                            ares::AttributeInfo &attributeInfo) {
    ClassReader::visitClassAttribute(classInfo, attributeInfo);
}

void ares::ClassReader::readMethods(ares::ClassInfo &classInfo) {
    if (ares::readU16(classInfo.m_MethodCount, classInfo.m_ByteCode,
                      classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the method count of the class file." << std::endl;
        abort();
    }

    classInfo.m_Methods = std::vector<std::shared_ptr<MethodInfo>>(classInfo.m_MethodCount);
    for (auto &method : classInfo.m_Methods) {
        auto methodInfo = std::make_shared<MethodInfo>();
        ClassReader::visitClassMethod(classInfo, *methodInfo);

        method = methodInfo;
    }
}

void ares::ClassReader::visitClassMethod(ares::ClassInfo &classInfo,
                                         ares::MethodInfo &methodInfo) {
    if (ares::readU16(methodInfo.m_AccessFlags, classInfo.m_ByteCode,
                      classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the access flags of the method." << std::endl;
        abort();
    }

    if (ares::readU16(methodInfo.m_NameIndex, classInfo.m_ByteCode,
                      classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the name index of the method." << std::endl;
        abort();
    }

    if (ares::readU16(methodInfo.m_DescriptorIndex, classInfo.m_ByteCode,
                      classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the descriptor index of the method." << std::endl;
        abort();
    }

    readMethodAttributes(classInfo, methodInfo);
}

void ares::ClassReader::readMethodAttributes(ares::ClassInfo &classInfo,
                                             ares::MethodInfo &methodInfo) {
    if (ares::readU16(methodInfo.m_AttributesCount, classInfo.m_ByteCode,
                      classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the attribute count." << std::endl;
        abort();
    }

    methodInfo.m_Attributes = std::vector<std::shared_ptr<AttributeInfo>>(
            methodInfo.m_AttributesCount);
    for (auto &attribute : methodInfo.m_Attributes) {
        auto attributeInfo = std::make_shared<AttributeInfo>();
        ClassReader::visitMethodAttribute(classInfo, methodInfo, *attributeInfo);

        attribute = attributeInfo;
    }
}

void ares::ClassReader::visitMethodAttribute(ares::ClassInfo &classInfo,
                                             ares::MethodInfo &,
                                             ares::AttributeInfo &attributeInfo) {
    ClassReader::visitClassAttribute(classInfo, attributeInfo);
}

unsigned int ares::ClassReader::getOffset() const {
    return m_Offset;
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