#pragma once

#include <unordered_map>
#include <sys/stat.h>
#include <unistd.h>
#include <memory>
#include <vector>
#include <list>

#include "attribute_info.h"
#include "constant_info.h"
#include "class_info.h"
#include "visitor.h"

namespace ares {

class ClassReader : Visitor {
public:
    explicit ClassReader(unsigned int offset = 0u);

public:
    void visit_class(ClassInfo &class_info) override;

    [[nodiscard]] auto offset() const -> unsigned int;

private:
    void read_class_attributes(ClassInfo &class_info);

    void visit_class_attribute(ClassInfo &class_info, AttributeInfo &attribute_info) override;

    void read_magic_number(ClassInfo &class_info);

    void read_class_version(ClassInfo &class_info);

    void read_constant_pool(ClassInfo &class_info);

    void visit_classpool_info(ClassInfo &class_info, ConstantPoolInfo &info) override;

    void read_class_info(ClassInfo &class_info, ConstantInfo::ClassInfo &info);

    void read_utf8_info(ClassInfo &class_info, ConstantInfo::UTF8Info &info);

    void read_field_method_info(ClassInfo &class_info, ConstantInfo::FieldMethodInfo &info);

    void read_name_and_type(ClassInfo &class_info, ConstantInfo::NameAndTypeInfo &info);

    void read_string_info(ClassInfo &class_info, ConstantInfo::StringInfo &info);

    void read_double_long(ClassInfo &class_info, ConstantInfo::DoubleLongInfo &info);

    void read_float_integer(ClassInfo &class_info, ConstantInfo::FloatIntegerInfo &info);

    void read_method_type(ClassInfo &class_info, ConstantInfo::MethodTypeInfo &info);

    void read_method_handle(ClassInfo &class_info, ConstantInfo::MethodHandleInfo &info);

    void read_dynamic(ClassInfo &class_info, ConstantInfo::DynamicInfo &info);

    void read_module_package(ClassInfo &class_info, ConstantInfo::ModulePackageInfo &info);

    void read_access_flags(ClassInfo &class_info);

    void read_this_class(ClassInfo &class_info);

    void read_super_class(ClassInfo &class_info);

    void read_interfaces(ClassInfo &class_info);

    void visit_class_interface(ClassInfo &class_info, uint16_t interface) override;

    void read_fields(ClassInfo &class_info);

    void visit_class_field(ClassInfo &class_info, FieldInfo &field_info) override;

    void read_field_attributes(ClassInfo &class_info, FieldInfo &field_info);

    void visit_field_attribute(ClassInfo &class_info, FieldInfo &field_info, AttributeInfo &attribute_info) override;

    void read_methods(ClassInfo &class_info);

    void visit_class_method(ClassInfo &class_info, MethodInfo &method_info) override;

    void read_method_attributes(ClassInfo &class_info, MethodInfo &method_info);

    void visit_method_attribute(ClassInfo &class_info, MethodInfo &method_info, AttributeInfo &attribute_info) override;

private:
    unsigned int _offset{};
};

} // namespace ares

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