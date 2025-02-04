#pragma once

#include "attribute_info.h"
#include "constant_info.h"
#include "method_info.h"
#include "class_info.h"
#include "field_info.h"
#include "visitor.h"

namespace ares {

class VMCheck : Visitor {
public:
    void visit_class(ClassInfo &classInfo) override;

    void visit_classpool_info(ClassInfo &classInfo, ConstantPoolInfo &constantPoolInfo) override;

    void visit_class_interface(ClassInfo &classInfo, uint16_t interface) override;

    void visit_class_field(ClassInfo &classInfo, FieldInfo &fieldInfo) override;

    void visit_class_method(ClassInfo &classInfo, MethodInfo &methodInfo) override;

    void visit_class_attribute(ClassInfo &classInfo, AttributeInfo &attributeInfo) override;

    void visit_field_attribute(ClassInfo &classInfo, FieldInfo &fieldInfo, AttributeInfo &attributeInfo) override;

    void visit_method_attribute(ClassInfo &classInfo, MethodInfo &methodInfo, AttributeInfo &attributeInfo) override;

    static void visit_class_info(ares::ClassInfo &classInfo, ares::ConstantInfo::ClassInfo &info);

    static void visit_field_method_info(ares::ClassInfo &classInfo, ConstantInfo::FieldMethodInfo &info);

    static void visit_name_and_type_info(ClassInfo &classInfo, ConstantInfo::NameAndTypeInfo &info);

    static void visit_string_info(ClassInfo &classInfo, ConstantInfo::StringInfo &info);

    static void visit_method_type_info(ClassInfo &classInfo, ConstantInfo::MethodTypeInfo &info);

    static void visit_method_handle_info(ClassInfo &classInfo, ConstantInfo::MethodHandleInfo &info);

    static void visit_dynamic_info(ClassInfo &classInfo, ConstantInfo::DynamicInfo &info);

    static void visit_module_package_info(ClassInfo &classInfo, ConstantInfo::ModulePackageInfo &info);
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