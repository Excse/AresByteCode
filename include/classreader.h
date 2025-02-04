#pragma once

#include <unordered_map>
#include <sys/stat.h>
#include <unistd.h>
#include <memory>
#include <vector>
#include <list>

#include "attributeinfo.h"
#include "constantinfo.h"
#include "classinfo.h"
#include "visitor.h"

namespace ares {

class ClassReader : Visitor {
public:
    explicit ClassReader(unsigned int offset = 0u);

    virtual ~ClassReader();

public:
    void visit_class(ClassInfo &classInfo) override;

    [[nodiscard]] auto offset() const -> unsigned int;

private:
    void read_class_attributes(ClassInfo &classInfo);

    void visit_class_attribute(ClassInfo &classInfo, AttributeInfo &attributeInfo) override;

    void read_magic_number(ClassInfo &classInfo);

    void read_class_version(ClassInfo &classInfo);

    void read_constant_pool(ClassInfo &classInfo);

    void visit_classpool_info(ClassInfo &classInfo, ConstantPoolInfo &info) override;

    void read_class_info(ClassInfo &classInfo, ConstantInfo::ClassInfo &info);

    void read_utf8_info(ClassInfo &classInfo, ConstantInfo::UTF8Info &info);

    void read_field_method_info(ClassInfo &classInfo, ConstantInfo::FieldMethodInfo &info);

    void read_name_and_type(ClassInfo &classInfo, ConstantInfo::NameAndTypeInfo &info);

    void read_string_info(ClassInfo &classInfo, ConstantInfo::StringInfo &info);

    void read_double_long(ClassInfo &classInfo, ConstantInfo::DoubleLongInfo &info);

    void read_float_integer(ClassInfo &classInfo, ConstantInfo::FloatIntegerInfo &info);

    void read_method_type(ClassInfo &classInfo, ConstantInfo::MethodTypeInfo &info);

    void read_method_handle(ClassInfo &classInfo, ConstantInfo::MethodHandleInfo &info);

    void read_dynamic(ClassInfo &classInfo, ConstantInfo::DynamicInfo &info);

    void read_module_package(ClassInfo &classInfo, ConstantInfo::ModulePackageInfo &info);

    void read_access_flags(ClassInfo &classInfo);

    void read_this_class(ClassInfo &classInfo);

    void read_super_class(ClassInfo &classInfo);

    void read_interfaces(ClassInfo &classInfo);

    void visit_class_interface(ClassInfo &classInfo, uint16_t interface) override;

    void read_fields(ClassInfo &classInfo);

    void visit_class_field(ClassInfo &classInfo, FieldInfo &fieldInfo) override;

    void read_field_attributes(ClassInfo &classInfo, FieldInfo &fieldInfo);

    void visit_field_attribute(ClassInfo &classInfo, FieldInfo &fieldInfo, AttributeInfo &attributeInfo) override;

    void read_methods(ClassInfo &classInfo);

    void visit_class_method(ClassInfo &classInfo, MethodInfo &methodInfo) override;

    void read_method_attributes(ClassInfo &classInfo, MethodInfo &methodInfo);

    void visit_method_attribute(ClassInfo &classInfo, MethodInfo &methodInfo, AttributeInfo &attributeInfo) override;

private:
    unsigned int m_Offset{};
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