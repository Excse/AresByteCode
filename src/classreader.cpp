#include "classreader.h"

#include <iostream>

#include "utils.h"

ares::ClassReader::ClassReader(unsigned int offset)
        : m_Offset(offset) {}

ares::ClassReader::~ClassReader() = default;

void ares::ClassReader::visit_class(ClassInfo &classInfo) {
    read_magic_number(classInfo);
    read_class_version(classInfo);
    read_constant_pool(classInfo);
    read_access_flags(classInfo);
    read_this_class(classInfo);
    read_super_class(classInfo);
    read_interfaces(classInfo);
    read_fields(classInfo);
    read_methods(classInfo);
    read_class_attributes(classInfo);
}

void ares::ClassReader::read_class_attributes(ClassInfo &classInfo) {
    if (ares::read_u16(classInfo.m_AttributesCount, classInfo.m_ByteCode,
                       classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the attribute count." << std::endl;
        abort();
    }

    classInfo.m_Attributes = std::vector<std::shared_ptr<AttributeInfo>>(
            classInfo.m_AttributesCount);
    for (auto &attribute : classInfo.m_Attributes) {
        auto attributeInfo = std::make_shared<AttributeInfo>();
        ClassReader::visit_class_attribute(classInfo, *attributeInfo);

        attribute = attributeInfo;
    }
}

void ares::ClassReader::visit_class_attribute(ClassInfo &classInfo,
                                              AttributeInfo &attributeInfo) {
    if (ares::read_u16(attributeInfo.m_AttributeNameIndex, classInfo.m_ByteCode,
                       classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the name index of the attribute." << std::endl;
        abort();
    }

    if (ares::read_u32(attributeInfo.m_AttributeLength, classInfo.m_ByteCode,
                       classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the length of the attribute." << std::endl;
        abort();
    }

    attributeInfo.m_Info = std::vector<uint8_t>(attributeInfo.m_AttributeLength);
    for (auto &info : attributeInfo.m_Info) {
        if (ares::read_u8(info, classInfo.m_ByteCode,
                          classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
            std::cerr << "Couldn't read the info of the attribute." << std::endl;
            abort();
        }
    }
}

void ares::ClassReader::read_magic_number(ares::ClassInfo &classInfo) {
    if (ares::read_u32(classInfo.m_MagicNumber, classInfo.m_ByteCode,
                       classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the magic number of the class file." << std::endl;
        abort();
    }
}

void ares::ClassReader::read_class_version(ares::ClassInfo &classInfo) {
    if (ares::read_u16(classInfo.m_MinorVersion, classInfo.m_ByteCode,
                       classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the minor version of the class file." << std::endl;
        abort();
    }

    if (ares::read_u16(classInfo.m_MajorVersion, classInfo.m_ByteCode,
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

void ares::ClassReader::read_constant_pool(ares::ClassInfo &classInfo) {
    if (ares::read_u16(classInfo.m_ConstantPoolCount, classInfo.m_ByteCode,
                       classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cout << "Couldn't read the constant pool count of this class file" << std::endl;
        abort();
    }

    classInfo.m_ConstantPool = std::vector<std::shared_ptr<ConstantPoolInfo>>(
            classInfo.m_ConstantPoolCount - 1);
    for (auto index = 0; index < classInfo.m_ConstantPoolCount - 1; index++) {
        auto info = std::make_shared<ConstantPoolInfo>();
        ClassReader::visit_classpool_info(classInfo, *info);

        classInfo.m_ConstantPool[index] = info;

        if (info->m_Tag == ConstantPoolInfo::DOUBLE || info->m_Tag == ConstantPoolInfo::LONG)
            index++;
    }
}

void ares::ClassReader::visit_classpool_info(ClassInfo &classInfo,
                                             ConstantPoolInfo &info) {
    uint8_t infoTag = 0;
    if (ares::read_u8(infoTag, classInfo.m_ByteCode,
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
        case ConstantPoolInfo::CLASS:read_class_info(classInfo, info.m_Info.class_info);
            break;
        case ConstantPoolInfo::METHOD_REF:
        case ConstantPoolInfo::FIELD_REF:
        case ConstantPoolInfo::INTERFACE_METHOD_REF:read_field_method_info(classInfo, info.m_Info.field_method_info);
            break;
        case ConstantPoolInfo::STRING:read_string_info(classInfo, info.m_Info.string_info);
            break;
        case ConstantPoolInfo::FLOAT:
        case ConstantPoolInfo::INTEGER:read_float_integer(classInfo, info.m_Info.integer_float_info);
            break;
        case ConstantPoolInfo::LONG:
        case ConstantPoolInfo::DOUBLE:read_double_long(classInfo, info.m_Info.long_double_info);
            break;
        case ConstantPoolInfo::NAME_AND_TYPE:read_name_and_type(classInfo, info.m_Info.name_and_type_info);
            break;
        case ConstantPoolInfo::UTF_8:read_utf8_info(classInfo, info.m_Info.utf8_info);
            break;
        case ConstantPoolInfo::METHOD_HANDLE:read_method_handle(classInfo, info.m_Info.method_handle_info);
            break;
        case ConstantPoolInfo::METHOD_TYPE:read_method_type(classInfo, info.m_Info.method_type_info);
            break;
        case ConstantPoolInfo::INVOKE_DYNAMIC:
        case ConstantPoolInfo::DYNAMIC:read_dynamic(classInfo, info.m_Info.dynamic_info);
            break;
        case ConstantPoolInfo::PACKAGE:
        case ConstantPoolInfo::MODULE:read_module_package(classInfo, info.m_Info.module_package_info);
            break;
        default:
            break;
    }
}

void ares::ClassReader::read_class_info(ares::ClassInfo &classInfo,
                                        ConstantInfo::ClassInfo &info) {
    if (ares::read_u16(info.m_NameIndex, classInfo.m_ByteCode,
                       classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the name index of the class pool info." << std::endl;
        abort();
    }
}

void ares::ClassReader::read_utf8_info(ares::ClassInfo &classInfo,
                                       ConstantInfo::UTF8Info &info) {
    if (ares::read_u16(info.m_Length, classInfo.m_ByteCode,
                       classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the length of the class pool info." << std::endl;
        abort();
    }

    info.m_Bytes = new uint8_t[info.m_Length];
    if (ares::read_u8_array(info.m_Bytes, info.m_Length, classInfo.m_ByteCode,
                            classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the bytes of the class pool info." << std::endl;
        abort();
    }
}

void ares::ClassReader::read_field_method_info(ares::ClassInfo &classInfo,
                                               ConstantInfo::FieldMethodInfo &info) {
    if (ares::read_u16(info.m_ClassIndex, classInfo.m_ByteCode,
                       classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the class index of the class pool info." << std::endl;
        abort();
    }

    if (ares::read_u16(info.m_NameAndTypeIndex, classInfo.m_ByteCode,
                       classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the name and type index of the class pool info." << std::endl;
        abort();
    }
}

void ares::ClassReader::read_name_and_type(ares::ClassInfo &classInfo,
                                           ConstantInfo::NameAndTypeInfo &info) {
    if (ares::read_u16(info.m_NameIndex, classInfo.m_ByteCode,
                       classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the name index of the class pool info." << std::endl;
        abort();
    }

    if (ares::read_u16(info.m_DescriptorIndex, classInfo.m_ByteCode,
                       classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the descriptor index of the class pool info." << std::endl;
        abort();
    }
}

void ares::ClassReader::read_string_info(ares::ClassInfo &classInfo,
                                         ConstantInfo::StringInfo &info) {
    if (ares::read_u16(info.m_StringIndex, classInfo.m_ByteCode,
                       classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the string index of the class pool info." << std::endl;
        abort();
    }
}

void ares::ClassReader::read_double_long(ares::ClassInfo &classInfo,
                                         ConstantInfo::DoubleLongInfo &info) {
    if (ares::read_u32(info.m_HighBytes, classInfo.m_ByteCode,
                       classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the high bytes of the class pool info." << std::endl;
        abort();
    }

    if (ares::read_u32(info.m_LowBytes, classInfo.m_ByteCode,
                       classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the low bytes of the class pool info." << std::endl;
        abort();
    }
}

void ares::ClassReader::read_float_integer(ares::ClassInfo &classInfo,
                                           ConstantInfo::FloatIntegerInfo &info) {
    if (ares::read_u32(info.m_Bytes, classInfo.m_ByteCode,
                       classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the bytes of the class pool info." << std::endl;
        abort();
    }
}

void ares::ClassReader::read_method_type(ares::ClassInfo &classInfo,
                                         ConstantInfo::MethodTypeInfo &info) {
    if (ares::read_u16(info.m_DescriptorIndex, classInfo.m_ByteCode,
                       classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the descriptor index of the class pool info." << std::endl;
        abort();
    }
}

void ares::ClassReader::read_method_handle(ares::ClassInfo &classInfo,
                                           ConstantInfo::MethodHandleInfo &info) {
    if (ares::read_u8(info.m_ReferenceKind, classInfo.m_ByteCode,
                      classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the reference kind of the class pool info." << std::endl;
        abort();
    }

    if (ares::read_u16(info.m_ReferenceIndex, classInfo.m_ByteCode,
                       classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the reference index of the class pool info." << std::endl;
        abort();
    }
}

void ares::ClassReader::read_dynamic(ares::ClassInfo &classInfo,
                                     ConstantInfo::DynamicInfo &info) {
    if (ares::read_u16(info.m_BoostrapMethodAttrIndex, classInfo.m_ByteCode,
                       classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the bootstrap method attribute index of the class pool info."
                  << std::endl;
        abort();
    }

    if (ares::read_u16(info.m_NameAndTypeIndex, classInfo.m_ByteCode,
                       classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the name and type index for the class pool info." << std::endl;
        abort();
    }
}

void ares::ClassReader::read_module_package(ares::ClassInfo &classInfo,
                                            ConstantInfo::ModulePackageInfo &info) {
    if (ares::read_u16(info.m_NameIndex, classInfo.m_ByteCode,
                       classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the name index of the class pool info." << std::endl;
        abort();
    }
}

void ares::ClassReader::read_access_flags(ClassInfo &classInfo) {
    if (ares::read_u16(classInfo.m_AccessFlags, classInfo.m_ByteCode,
                       classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the access flags of the class file." << std::endl;
        abort();
    }
}

void ares::ClassReader::read_this_class(ClassInfo &classInfo) {
    if (ares::read_u16(classInfo.m_ThisClass, classInfo.m_ByteCode,
                       classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the \"this class\" of this class file." << std::endl;
        abort();
    }
}

void ares::ClassReader::read_super_class(ClassInfo &classInfo) {
    if (ares::read_u16(classInfo.m_SuperClass, classInfo.m_ByteCode,
                       classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the \"super class\" of the class file." << std::endl;
        abort();
    }
}

void ares::ClassReader::read_interfaces(ClassInfo &classInfo) {
    if (ares::read_u16(classInfo.m_InterfacesCount, classInfo.m_ByteCode,
                       classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the interface count of the class file." << std::endl;
        abort();
    }

    classInfo.m_Interfaces = std::vector<uint16_t>(classInfo.m_InterfacesCount);
    for (auto &interface : classInfo.m_Interfaces) {
        if (ares::read_u16(interface, classInfo.m_ByteCode,
                           classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
            std::cerr << "Couldn't read the interface of this class." << std::endl;
            abort();
        }
    }
}

void ares::ClassReader::visit_class_interface(ClassInfo &, uint16_t) {}

void ares::ClassReader::read_fields(ClassInfo &classInfo) {
    if (ares::read_u16(classInfo.m_FieldsCount, classInfo.m_ByteCode,
                       classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the field count of the class file." << std::endl;
        abort();
    }

    classInfo.m_Fields = std::vector<std::shared_ptr<FieldInfo>>(classInfo.m_FieldsCount);
    for (auto &field : classInfo.m_Fields) {
        auto fieldInfo = std::make_shared<FieldInfo>();
        ClassReader::visit_class_field(classInfo, *fieldInfo);

        field = fieldInfo;
    }
}

void ares::ClassReader::visit_class_field(ClassInfo &classInfo,
                                          FieldInfo &fieldInfo) {
    if (ares::read_u16(fieldInfo.m_AccessFlags, classInfo.m_ByteCode,
                       classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the access flags of the field." << std::endl;
        abort();
    }

    if (ares::read_u16(fieldInfo.m_NameIndex, classInfo.m_ByteCode,
                       classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the name index of the field." << std::endl;
        abort();
    }

    if (ares::read_u16(fieldInfo.m_DescriptorIndex, classInfo.m_ByteCode,
                       classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the descriptor index of the field." << std::endl;
        abort();
    }

    read_field_attributes(classInfo, fieldInfo);
}

void ares::ClassReader::read_field_attributes(ClassInfo &classInfo,
                                              FieldInfo &fieldInfo) {
    if (ares::read_u16(fieldInfo.m_AttributesCount, classInfo.m_ByteCode,
                       classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the attribute count." << std::endl;
        abort();
    }

    fieldInfo.m_Attributes = std::vector<std::shared_ptr<AttributeInfo>>(
            fieldInfo.m_AttributesCount);
    for (auto &attribute : fieldInfo.m_Attributes) {
        auto attributeInfo = std::make_shared<AttributeInfo>();
        ClassReader::visit_field_attribute(classInfo, fieldInfo, *attributeInfo);

        attribute = attributeInfo;
    }
}

void ares::ClassReader::visit_field_attribute(ClassInfo &classInfo,
                                              FieldInfo &,
                                              AttributeInfo &attributeInfo) {
    ClassReader::visit_class_attribute(classInfo, attributeInfo);
}

void ares::ClassReader::read_methods(ClassInfo &classInfo) {
    if (ares::read_u16(classInfo.m_MethodCount, classInfo.m_ByteCode,
                       classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the method count of the class file." << std::endl;
        abort();
    }

    classInfo.m_Methods = std::vector<std::shared_ptr<MethodInfo>>(classInfo.m_MethodCount);
    for (auto &method : classInfo.m_Methods) {
        auto methodInfo = std::make_shared<MethodInfo>();
        ClassReader::visit_class_method(classInfo, *methodInfo);

        method = methodInfo;
    }
}

void ares::ClassReader::visit_class_method(ClassInfo &classInfo,
                                           MethodInfo &methodInfo) {
    if (ares::read_u16(methodInfo.m_AccessFlags, classInfo.m_ByteCode,
                       classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the access flags of the method." << std::endl;
        abort();
    }

    if (ares::read_u16(methodInfo.m_NameIndex, classInfo.m_ByteCode,
                       classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the name index of the method." << std::endl;
        abort();
    }

    if (ares::read_u16(methodInfo.m_DescriptorIndex, classInfo.m_ByteCode,
                       classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the descriptor index of the method." << std::endl;
        abort();
    }

    read_method_attributes(classInfo, methodInfo);
}

void ares::ClassReader::read_method_attributes(ClassInfo &classInfo,
                                               MethodInfo &methodInfo) {
    if (ares::read_u16(methodInfo.m_AttributesCount, classInfo.m_ByteCode,
                       classInfo.m_Size, m_Offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the attribute count." << std::endl;
        abort();
    }

    methodInfo.m_Attributes = std::vector<std::shared_ptr<AttributeInfo>>(
            methodInfo.m_AttributesCount);
    for (auto &attribute : methodInfo.m_Attributes) {
        auto attributeInfo = std::make_shared<AttributeInfo>();
        ClassReader::visit_method_attribute(classInfo, methodInfo, *attributeInfo);

        attribute = attributeInfo;
    }
}

void ares::ClassReader::visit_method_attribute(ClassInfo &classInfo,
                                               MethodInfo &,
                                               AttributeInfo &attributeInfo) {
    ClassReader::visit_class_attribute(classInfo, attributeInfo);
}

unsigned int ares::ClassReader::offset() const {
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