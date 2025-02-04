#pragma once

#include "attributeinfo.h"
#include "constantinfo.h"
#include "methodinfo.h"
#include "classinfo.h"
#include "fieldinfo.h"
#include "visitor.h"

namespace ares {

    class VMCheck : Visitor {

    public:
        void visitClass(ares::ClassInfo &classInfo) override;

        void visitClassCPInfo(ares::ClassInfo &classInfo,
                              ares::ConstantPoolInfo &constantPoolInfo) override;

        void visitClassInterface(ares::ClassInfo &classInfo,
                                 uint16_t interface) override;

        void visitClassField(ares::ClassInfo &classInfo,
                             ares::FieldInfo &fieldInfo) override;

        void visitClassMethod(ares::ClassInfo &classInfo,
                              ares::MethodInfo &methodInfo) override;

        void visitClassAttribute(ares::ClassInfo &classInfo,
                                 ares::AttributeInfo &attributeInfo) override;

        void visitFieldAttribute(ares::ClassInfo &classInfo,
                                 ares::FieldInfo &fieldInfo,
                                 ares::AttributeInfo &attributeInfo) override;

        void visitMethodAttribute(ares::ClassInfo &classInfo,
                                  ares::MethodInfo &methodInfo,
                                  ares::AttributeInfo &attributeInfo) override;

        static void visitClassInfo(ares::ClassInfo &classInfo,
                                   ares::ConstantInfo::ClassInfo &info);

        static void visitFieldMethodInfo(ares::ClassInfo &classInfo,
                                         ares::ConstantInfo::FieldMethodInfo &info);

        static void visitNameAndTypeInfo(ares::ClassInfo &classInfo,
                                         ares::ConstantInfo::NameAndTypeInfo &info);

        static void visitStringInfo(ares::ClassInfo &classInfo,
                                    ares::ConstantInfo::StringInfo &info);

        static void visitMethodTypeInfo(ares::ClassInfo &classInfo,
                                        ares::ConstantInfo::MethodTypeInfo &info);

        static void visitMethodHandleInfo(ares::ClassInfo &classInfo,
                                          ares::ConstantInfo::MethodHandleInfo &info);

        static void visitDynamicInfo(ares::ClassInfo &classInfo,
                                     ares::ConstantInfo::DynamicInfo &info);

        static void visitModulePackageInfo(ares::ClassInfo &classInfo,
                                           ares::ConstantInfo::ModulePackageInfo &info);

    };

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