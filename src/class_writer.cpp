#include "class_writer.h"

#include <iostream>

#include "constant_info.h"
#include "utils.h"

ares::ClassWriter::ClassWriter(unsigned int offset) : _offset(offset) {}

void ares::ClassWriter::visit_class(ClassInfo &classInfo) {
    _size = classInfo.size();
    _byte_code = std::vector<uint8_t>(_size);

    ares::write_u32(classInfo.magic_number, _byte_code, _offset);
    ares::write_u16(classInfo.minor_version, _byte_code, _offset);
    ares::write_u16(classInfo.major_version, _byte_code, _offset);

    ares::write_u16(classInfo.constant_pool_count, _byte_code, _offset);
    for (auto &constantPoolInfo : classInfo.constant_pool) {
        if (constantPoolInfo == nullptr)
            continue;

        ClassWriter::visit_classpool_info(classInfo, *constantPoolInfo);
    }

    ares::write_u16(classInfo.access_flags, _byte_code, _offset);
    ares::write_u16(classInfo.this_class, _byte_code, _offset);
    ares::write_u16(classInfo.super_class, _byte_code, _offset);

    ares::write_u16(classInfo.interfaces_count, _byte_code, _offset);
    for (auto index = 0; index < classInfo.interfaces_count; index++)
        ares::write_u16(classInfo.interfaces[index], _byte_code, _offset);

    ares::write_u16(classInfo.fields_count, _byte_code, _offset);
    for (const auto &fieldInfo : classInfo.fields)
        ClassWriter::visit_class_field(classInfo, *fieldInfo);

    ares::write_u16(classInfo.method_count, _byte_code, _offset);
    for (const auto &methodInfo : classInfo.methods)
        ClassWriter::visit_class_method(classInfo, *methodInfo);

    ares::write_u16(classInfo.attributes_count, _byte_code, _offset);
    for (auto &attributeInfo : classInfo.attributes)
        ClassWriter::visit_class_attribute(classInfo, *attributeInfo);
}

void ares::ClassWriter::visit_classpool_info(ClassInfo &, ConstantPoolInfo &info) {
    ares::write_u8((uint8_t &) info.tag, _byte_code, _offset);

    switch (info.tag) {
        case ConstantPoolInfo::CLASS: {
            ares::write_u16(info.info.class_info.name_index, _byte_code, _offset);
            break;
        }
        case ConstantPoolInfo::METHOD_REF:
        case ConstantPoolInfo::FIELD_REF:
        case ConstantPoolInfo::INTERFACE_METHOD_REF: {
            ares::write_u16(info.info.field_method_info.class_index, _byte_code, _offset);
            ares::write_u16(info.info.field_method_info.name_and_type_index, _byte_code, _offset);
            break;
        }
        case ConstantPoolInfo::STRING: {
            ares::write_u16(info.info.string_info.string_index, _byte_code, _offset);
            break;
        }
        case ConstantPoolInfo::FLOAT:
        case ConstantPoolInfo::INTEGER: {
            ares::write_u32(info.info.integer_float_info.bytes, _byte_code, _offset);
            break;
        }
        case ConstantPoolInfo::LONG:
        case ConstantPoolInfo::DOUBLE: {
            ares::write_u32(info.info.long_double_info.high_bytes, _byte_code, _offset);
            ares::write_u32(info.info.long_double_info.low_bytes, _byte_code, _offset);
            break;
        }
        case ConstantPoolInfo::NAME_AND_TYPE: {
            ares::write_u16(info.info.name_and_type_info.name_index, _byte_code, _offset);
            ares::write_u16(info.info.name_and_type_info.descriptor_index, _byte_code, _offset);
            break;
        }
        case ConstantPoolInfo::UTF_8: {
            ares::write_u16(info.info.utf8_info.length, _byte_code, _offset);
            ares::write_u8_array(info.info.utf8_info.bytes, info.info.utf8_info.length, _byte_code, _offset);
            break;
        }
        case ConstantPoolInfo::METHOD_HANDLE: {
            ares::write_u8(info.info.method_handle_info.reference_kind, _byte_code, _offset);
            ares::write_u16(info.info.method_handle_info.reference_index, _byte_code, _offset);
            break;
        }
        case ConstantPoolInfo::METHOD_TYPE: {
            ares::write_u16(info.info.method_type_info.descriptor_index, _byte_code, _offset);
            break;
        }
        case ConstantPoolInfo::INVOKE_DYNAMIC:
        case ConstantPoolInfo::DYNAMIC:
            ares::write_u16(info.info.dynamic_info.boostrap_method_attr_index, _byte_code, _offset);
            ares::write_u16(info.info.dynamic_info.name_and_type_index, _byte_code, _offset);
            break;
        case ConstantPoolInfo::PACKAGE:
        case ConstantPoolInfo::MODULE: {
            ares::write_u16(info.info.module_package_info.name_index, _byte_code, _offset);
            break;
        }
        default:
            abort();
    }
}

void ares::ClassWriter::visit_class_interface(ClassInfo &, uint16_t) {}

void ares::ClassWriter::visit_class_field(ClassInfo &classInfo,
                                          FieldInfo &fieldInfo) {
    ares::write_u16(fieldInfo.access_flags, _byte_code, _offset);
    ares::write_u16(fieldInfo.name_index, _byte_code, _offset);
    ares::write_u16(fieldInfo.descriptor_index, _byte_code, _offset);

    ares::write_u16(fieldInfo.attributes_count, _byte_code, _offset);
    for (const auto &attributeInfo : fieldInfo.attributes)
        ClassWriter::visit_field_attribute(classInfo, fieldInfo, *attributeInfo);
}

void ares::ClassWriter::visit_class_method(ClassInfo &classInfo, MethodInfo &methodInfo) {
    ares::write_u16(methodInfo.access_flags, _byte_code, _offset);
    ares::write_u16(methodInfo.name_index, _byte_code, _offset);
    ares::write_u16(methodInfo.descriptor_index, _byte_code, _offset);

    ares::write_u16(methodInfo.attributes_count, _byte_code, _offset);
    for (const auto &attributeInfo : methodInfo.attributes)
        ClassWriter::visit_method_attribute(classInfo, methodInfo, *attributeInfo);
}

void ares::ClassWriter::visit_class_attribute(ClassInfo &, AttributeInfo &attributeInfo) {
    ares::write_u16(attributeInfo.attribute_name_index, _byte_code, _offset);
    ares::write_u32(attributeInfo.attribute_length, _byte_code, _offset);

    for (auto &info : attributeInfo.info)
        ares::write_u8(info, _byte_code, _offset);
}

void ares::ClassWriter::visit_field_attribute(ClassInfo &classInfo, FieldInfo &, AttributeInfo &attributeInfo) {
    ClassWriter::visit_class_attribute(classInfo, attributeInfo);
}

void ares::ClassWriter::visit_method_attribute(ClassInfo &classInfo, MethodInfo &, AttributeInfo &attributeInfo) {
    ClassWriter::visit_class_attribute(classInfo, attributeInfo);
}

auto ares::ClassWriter::byte_code() const -> const std::vector<uint8_t> & {
    return _byte_code;
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