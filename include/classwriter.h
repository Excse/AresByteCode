#pragma once

#include "visitor.h"

namespace ares {

    class ClassWriter : Visitor {

    private:
        unsigned int m_Offset{}, m_Size{};
        uint8_t *m_ByteCode{};

    public:
        explicit ClassWriter(unsigned int offset = 0);

        virtual ~ClassWriter();

    public:
        void visitClass(ClassInfo &classInfo) override;

    private:
        void visitClassCPInfo(ares::ClassInfo &classInfo,
                              ares::ConstantPoolInfo &info) override;

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

    public:
        [[nodiscard]]
        uint8_t *getByteCode() const;

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