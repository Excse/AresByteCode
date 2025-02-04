#include "class_reader.h"

#include <iostream>

#include "utils.h"

ares::ClassReader::ClassReader(unsigned int offset) : _offset(offset) {}

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
    if (ares::read_u16(classInfo.attributes_count, classInfo.byte_code,
                       classInfo.info_size, _offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the attribute count." << std::endl;
        abort();
    }

    classInfo.attributes = std::vector<std::shared_ptr<AttributeInfo>>(
            classInfo.attributes_count);
    for (auto &attribute : classInfo.attributes) {
        auto attributeInfo = std::make_shared<AttributeInfo>();
        ClassReader::visit_class_attribute(classInfo, *attributeInfo);

        attribute = attributeInfo;
    }
}

void ares::ClassReader::visit_class_attribute(ClassInfo &classInfo,
                                              AttributeInfo &attributeInfo) {
    if (ares::read_u16(attributeInfo.attribute_name_index, classInfo.byte_code,
                       classInfo.info_size, _offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the name index of the attribute." << std::endl;
        abort();
    }

    if (ares::read_u32(attributeInfo.attribute_length, classInfo.byte_code,
                       classInfo.info_size, _offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the length of the attribute." << std::endl;
        abort();
    }

    attributeInfo.info = std::vector<uint8_t>(attributeInfo.attribute_length);
    for (auto &info : attributeInfo.info) {
        if (ares::read_u8(info, classInfo.byte_code,
                          classInfo.info_size, _offset) == EXIT_FAILURE) {
            std::cerr << "Couldn't read the info of the attribute." << std::endl;
            abort();
        }
    }
}

void ares::ClassReader::read_magic_number(ares::ClassInfo &classInfo) {
    if (ares::read_u32(classInfo.magic_number, classInfo.byte_code,
                       classInfo.info_size, _offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the magic number of the class file." << std::endl;
        abort();
    }
}

void ares::ClassReader::read_class_version(ares::ClassInfo &classInfo) {
    if (ares::read_u16(classInfo.minor_version, classInfo.byte_code,
                       classInfo.info_size, _offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the minor version of the class file." << std::endl;
        abort();
    }

    if (ares::read_u16(classInfo.major_version, classInfo.byte_code,
                       classInfo.info_size, _offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the major version of the class file." << std::endl;
        abort();
    }

    classInfo.class_version = ClassInfo::UNDEFINED;
    if (classInfo.major_version >= ClassInfo::VERSION_1_1
        && classInfo.major_version <= ClassInfo::VERSION_15) {
        classInfo.class_version = ClassInfo::ClassVersion(classInfo.major_version);
    }
}

void ares::ClassReader::read_constant_pool(ares::ClassInfo &classInfo) {
    if (ares::read_u16(classInfo.constant_pool_count, classInfo.byte_code,
                       classInfo.info_size, _offset) == EXIT_FAILURE) {
        std::cout << "Couldn't read the constant pool count of this class file" << std::endl;
        abort();
    }

    classInfo.constant_pool = std::vector<std::shared_ptr<ConstantPoolInfo>>(
        classInfo.constant_pool_count - 1);
    for (auto index = 0; index < classInfo.constant_pool_count - 1; index++) {
        auto info = std::make_shared<ConstantPoolInfo>();
        ClassReader::visit_classpool_info(classInfo, *info);

        classInfo.constant_pool[index] = info;

        if (info->tag == ConstantPoolInfo::DOUBLE || info->tag == ConstantPoolInfo::LONG)
            index++;
    }
}

void ares::ClassReader::visit_classpool_info(ClassInfo &classInfo,
                                             ConstantPoolInfo &info) {
    uint8_t infoTag = 0;
    if (ares::read_u8(infoTag, classInfo.byte_code,
                      classInfo.info_size, _offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the tag of the constant pool info of this class file"
                  << std::endl;
        abort();
    }

    if (infoTag >= ConstantPoolInfo::UTF_8 && infoTag <= ConstantPoolInfo::PACKAGE
        && infoTag != 13 && infoTag != 14)
        info.tag = ConstantPoolInfo::ConstantTag(infoTag);
    else
        info.tag = ConstantPoolInfo::UNDEFINED;

    switch (info.tag) {
        case ConstantPoolInfo::CLASS:read_class_info(classInfo, info.info.class_info);
            break;
        case ConstantPoolInfo::METHOD_REF:
        case ConstantPoolInfo::FIELD_REF:
        case ConstantPoolInfo::INTERFACE_METHOD_REF:read_field_method_info(classInfo, info.info.field_method_info);
            break;
        case ConstantPoolInfo::STRING:read_string_info(classInfo, info.info.string_info);
            break;
        case ConstantPoolInfo::FLOAT:
        case ConstantPoolInfo::INTEGER:read_float_integer(classInfo, info.info.integer_float_info);
            break;
        case ConstantPoolInfo::LONG:
        case ConstantPoolInfo::DOUBLE:read_double_long(classInfo, info.info.long_double_info);
            break;
        case ConstantPoolInfo::NAME_AND_TYPE:read_name_and_type(classInfo, info.info.name_and_type_info);
            break;
        case ConstantPoolInfo::UTF_8:read_utf8_info(classInfo, info.info.utf8_info);
            break;
        case ConstantPoolInfo::METHOD_HANDLE:read_method_handle(classInfo, info.info.method_handle_info);
            break;
        case ConstantPoolInfo::METHOD_TYPE:read_method_type(classInfo, info.info.method_type_info);
            break;
        case ConstantPoolInfo::INVOKE_DYNAMIC:
        case ConstantPoolInfo::DYNAMIC:read_dynamic(classInfo, info.info.dynamic_info);
            break;
        case ConstantPoolInfo::PACKAGE:
        case ConstantPoolInfo::MODULE:read_module_package(classInfo, info.info.module_package_info);
            break;
        default:
            break;
    }
}

void ares::ClassReader::read_class_info(ares::ClassInfo &classInfo,
                                        ConstantInfo::ClassInfo &info) {
    if (ares::read_u16(info.name_index, classInfo.byte_code,
                       classInfo.info_size, _offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the name index of the class pool info." << std::endl;
        abort();
    }
}

void ares::ClassReader::read_utf8_info(ares::ClassInfo &classInfo,
                                       ConstantInfo::UTF8Info &info) {
    if (ares::read_u16(info.length, classInfo.byte_code,
                       classInfo.info_size, _offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the length of the class pool info." << std::endl;
        abort();
    }

    info.bytes = new uint8_t[info.length];
    if (ares::read_u8_array(info.bytes, info.length, classInfo.byte_code,
                            classInfo.info_size, _offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the bytes of the class pool info." << std::endl;
        abort();
    }
}

void ares::ClassReader::read_field_method_info(ares::ClassInfo &classInfo,
                                               ConstantInfo::FieldMethodInfo &info) {
    if (ares::read_u16(info.class_index, classInfo.byte_code,
                       classInfo.info_size, _offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the class index of the class pool info." << std::endl;
        abort();
    }

    if (ares::read_u16(info.name_and_type_index, classInfo.byte_code,
                       classInfo.info_size, _offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the name and type index of the class pool info." << std::endl;
        abort();
    }
}

void ares::ClassReader::read_name_and_type(ares::ClassInfo &classInfo,
                                           ConstantInfo::NameAndTypeInfo &info) {
    if (ares::read_u16(info.name_index, classInfo.byte_code,
                       classInfo.info_size, _offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the name index of the class pool info." << std::endl;
        abort();
    }

    if (ares::read_u16(info.descriptor_index, classInfo.byte_code,
                       classInfo.info_size, _offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the descriptor index of the class pool info." << std::endl;
        abort();
    }
}

void ares::ClassReader::read_string_info(ares::ClassInfo &classInfo,
                                         ConstantInfo::StringInfo &info) {
    if (ares::read_u16(info.string_index, classInfo.byte_code,
                       classInfo.info_size, _offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the string index of the class pool info." << std::endl;
        abort();
    }
}

void ares::ClassReader::read_double_long(ares::ClassInfo &classInfo,
                                         ConstantInfo::DoubleLongInfo &info) {
    if (ares::read_u32(info.high_bytes, classInfo.byte_code,
                       classInfo.info_size, _offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the high bytes of the class pool info." << std::endl;
        abort();
    }

    if (ares::read_u32(info.low_bytes, classInfo.byte_code,
                       classInfo.info_size, _offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the low bytes of the class pool info." << std::endl;
        abort();
    }
}

void ares::ClassReader::read_float_integer(ares::ClassInfo &classInfo,
                                           ConstantInfo::FloatIntegerInfo &info) {
    if (ares::read_u32(info.bytes, classInfo.byte_code,
                       classInfo.info_size, _offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the bytes of the class pool info." << std::endl;
        abort();
    }
}

void ares::ClassReader::read_method_type(ares::ClassInfo &classInfo,
                                         ConstantInfo::MethodTypeInfo &info) {
    if (ares::read_u16(info.descriptor_index, classInfo.byte_code,
                       classInfo.info_size, _offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the descriptor index of the class pool info." << std::endl;
        abort();
    }
}

void ares::ClassReader::read_method_handle(ares::ClassInfo &classInfo,
                                           ConstantInfo::MethodHandleInfo &info) {
    if (ares::read_u8(info.reference_kind, classInfo.byte_code,
                      classInfo.info_size, _offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the reference kind of the class pool info." << std::endl;
        abort();
    }

    if (ares::read_u16(info.reference_index, classInfo.byte_code,
                       classInfo.info_size, _offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the reference index of the class pool info." << std::endl;
        abort();
    }
}

void ares::ClassReader::read_dynamic(ares::ClassInfo &classInfo,
                                     ConstantInfo::DynamicInfo &info) {
    if (ares::read_u16(info.boostrap_method_attr_index, classInfo.byte_code,
                       classInfo.info_size, _offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the bootstrap method attribute index of the class pool info."
                  << std::endl;
        abort();
    }

    if (ares::read_u16(info.name_and_type_index, classInfo.byte_code,
                       classInfo.info_size, _offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the name and type index for the class pool info." << std::endl;
        abort();
    }
}

void ares::ClassReader::read_module_package(ares::ClassInfo &classInfo,
                                            ConstantInfo::ModulePackageInfo &info) {
    if (ares::read_u16(info.name_index, classInfo.byte_code,
                       classInfo.info_size, _offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the name index of the class pool info." << std::endl;
        abort();
    }
}

void ares::ClassReader::read_access_flags(ClassInfo &classInfo) {
    if (ares::read_u16(classInfo.access_flags, classInfo.byte_code,
                       classInfo.info_size, _offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the access flags of the class file." << std::endl;
        abort();
    }
}

void ares::ClassReader::read_this_class(ClassInfo &classInfo) {
    if (ares::read_u16(classInfo.this_class, classInfo.byte_code,
                       classInfo.info_size, _offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the \"this class\" of this class file." << std::endl;
        abort();
    }
}

void ares::ClassReader::read_super_class(ClassInfo &classInfo) {
    if (ares::read_u16(classInfo.super_class, classInfo.byte_code,
                       classInfo.info_size, _offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the \"super class\" of the class file." << std::endl;
        abort();
    }
}

void ares::ClassReader::read_interfaces(ClassInfo &classInfo) {
    if (ares::read_u16(classInfo.interfaces_count, classInfo.byte_code,
                       classInfo.info_size, _offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the interface count of the class file." << std::endl;
        abort();
    }

    classInfo.interfaces = std::vector<uint16_t>(classInfo.interfaces_count);
    for (auto &interface : classInfo.interfaces) {
        if (ares::read_u16(interface, classInfo.byte_code,
                           classInfo.info_size, _offset) == EXIT_FAILURE) {
            std::cerr << "Couldn't read the interface of this class." << std::endl;
            abort();
        }
    }
}

void ares::ClassReader::visit_class_interface(ClassInfo &, uint16_t) {}

void ares::ClassReader::read_fields(ClassInfo &classInfo) {
    if (ares::read_u16(classInfo.fields_count, classInfo.byte_code,
                       classInfo.info_size, _offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the field count of the class file." << std::endl;
        abort();
    }

    classInfo.fields = std::vector<std::shared_ptr<FieldInfo>>(classInfo.fields_count);
    for (auto &field : classInfo.fields) {
        auto fieldInfo = std::make_shared<FieldInfo>();
        ClassReader::visit_class_field(classInfo, *fieldInfo);

        field = fieldInfo;
    }
}

void ares::ClassReader::visit_class_field(ClassInfo &classInfo,
                                          FieldInfo &fieldInfo) {
    if (ares::read_u16(fieldInfo.access_flags, classInfo.byte_code,
                       classInfo.info_size, _offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the access flags of the field." << std::endl;
        abort();
    }

    if (ares::read_u16(fieldInfo.name_index, classInfo.byte_code,
                       classInfo.info_size, _offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the name index of the field." << std::endl;
        abort();
    }

    if (ares::read_u16(fieldInfo.descriptor_index, classInfo.byte_code,
                       classInfo.info_size, _offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the descriptor index of the field." << std::endl;
        abort();
    }

    read_field_attributes(classInfo, fieldInfo);
}

void ares::ClassReader::read_field_attributes(ClassInfo &classInfo,
                                              FieldInfo &fieldInfo) {
    if (ares::read_u16(fieldInfo.attributes_count, classInfo.byte_code,
                       classInfo.info_size, _offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the attribute count." << std::endl;
        abort();
    }

    fieldInfo.attributes = std::vector<std::shared_ptr<AttributeInfo>>(
            fieldInfo.attributes_count);
    for (auto &attribute : fieldInfo.attributes) {
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
    if (ares::read_u16(classInfo.method_count, classInfo.byte_code,
                       classInfo.info_size, _offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the method count of the class file." << std::endl;
        abort();
    }

    classInfo.methods = std::vector<std::shared_ptr<MethodInfo>>(classInfo.method_count);
    for (auto &method : classInfo.methods) {
        auto methodInfo = std::make_shared<MethodInfo>();
        ClassReader::visit_class_method(classInfo, *methodInfo);

        method = methodInfo;
    }
}

void ares::ClassReader::visit_class_method(ClassInfo &classInfo,
                                           MethodInfo &methodInfo) {
    if (ares::read_u16(methodInfo.access_flags, classInfo.byte_code,
                       classInfo.info_size, _offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the access flags of the method." << std::endl;
        abort();
    }

    if (ares::read_u16(methodInfo.name_index, classInfo.byte_code,
                       classInfo.info_size, _offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the name index of the method." << std::endl;
        abort();
    }

    if (ares::read_u16(methodInfo.descriptor_index, classInfo.byte_code,
                       classInfo.info_size, _offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the descriptor index of the method." << std::endl;
        abort();
    }

    read_method_attributes(classInfo, methodInfo);
}

void ares::ClassReader::read_method_attributes(ClassInfo &classInfo,
                                               MethodInfo &methodInfo) {
    if (ares::read_u16(methodInfo.attributes_count, classInfo.byte_code,
                       classInfo.info_size, _offset) == EXIT_FAILURE) {
        std::cerr << "Couldn't read the attribute count." << std::endl;
        abort();
    }

    methodInfo.attributes = std::vector<std::shared_ptr<AttributeInfo>>(
            methodInfo.attributes_count);
    for (auto &attribute : methodInfo.attributes) {
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

auto ares::ClassReader::offset() const -> unsigned int {
    return _offset;
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