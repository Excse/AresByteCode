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
    void visitClass(ares::ClassInfo &classInfo) override;

    [[nodiscard]] unsigned int getOffset() const;

private:
    void readClassAttributes(ClassInfo &classInfo);

    void visitClassAttribute(ares::ClassInfo &classInfo,
                             ares::AttributeInfo &attributeInfo) override;

    void readMagicNumber(ares::ClassInfo &classInfo);

    void readClassVersion(ares::ClassInfo &classInfo);

    void readConstantPool(ares::ClassInfo &classInfo);

    void visitClassCPInfo(ares::ClassInfo &classInfo,
                          ares::ConstantPoolInfo &info) override;

    void readClassInfo(ares::ClassInfo &classInfo,
                       ConstantInfo::ClassInfo &info);

    void readUTF8Info(ares::ClassInfo &classInfo,
                      ConstantInfo::UTF8Info &info);

    void readFieldMethodInfo(ares::ClassInfo &classInfo,
                             ConstantInfo::FieldMethodInfo &info);

    void readNameAndType(ares::ClassInfo &classInfo,
                         ConstantInfo::NameAndTypeInfo &info);

    void readStringInfo(ares::ClassInfo &classInfo,
                        ConstantInfo::StringInfo &info);

    void readDoubleLong(ares::ClassInfo &classInfo,
                        ConstantInfo::DoubleLongInfo &info);

    void readFloatInteger(ares::ClassInfo &classInfo,
                          ConstantInfo::FloatIntegerInfo &info);

    void readMethodType(ares::ClassInfo &classInfo,
                        ConstantInfo::MethodTypeInfo &info);

    void readMethodHandle(ares::ClassInfo &classInfo,
                          ConstantInfo::MethodHandleInfo &info);

    void readDynamic(ares::ClassInfo &classInfo,
                     ConstantInfo::DynamicInfo &info);

    void readModulePackage(ares::ClassInfo &classInfo,
                           ConstantInfo::ModulePackageInfo &info);

    void readAccessFlags(ClassInfo &classInfo);

    void readThisClass(ClassInfo &classInfo);

    void readSuperClass(ClassInfo &classInfo);

    void readInterfaces(ClassInfo &classInfo);

    void visitClassInterface(ClassInfo &classInfo,
                             uint16_t interface) override;

    void readFields(ClassInfo &classInfo);

    void visitClassField(ares::ClassInfo &classInfo,
                         ares::FieldInfo &fieldInfo) override;

    void readFieldAttributes(ClassInfo &classInfo, FieldInfo &fieldInfo);

    void visitFieldAttribute(ares::ClassInfo &classInfo,
                             ares::FieldInfo &fieldInfo,
                             ares::AttributeInfo &attributeInfo) override;

    void readMethods(ClassInfo &classInfo);

    void visitClassMethod(ares::ClassInfo &classInfo,
                          ares::MethodInfo &methodInfo) override;

    void readMethodAttributes(ClassInfo &classInfo, MethodInfo &methodInfo);

    void visitMethodAttribute(ares::ClassInfo &classInfo,
                              ares::MethodInfo &methodInfo,
                              ares::AttributeInfo &attributeInfo) override;

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