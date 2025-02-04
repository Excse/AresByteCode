#pragma once

#include <unordered_map>
#include <sys/stat.h>
#include <unistd.h>
#include <memory>
#include <vector>
#include <list>

namespace ares {

union ConstantInfo {
    enum MethodHandleKind : uint16_t {
        GetField = 1,
        GetStatic = 2,
        PutField = 3,
        PutStatic = 4,
        InvokeVirtual = 5,
        InvokeStatic = 6,
        InvokeSpecial = 7,
        NewInvokeSpecial = 8,
        InvokeInterface = 9,
    };

    struct ClassInfo {
        uint16_t m_NameIndex;
    } class_info;

    struct FieldMethodInfo {
        uint16_t m_ClassIndex;
        uint16_t m_NameAndTypeIndex;
    } field_method_info;

    struct StringInfo {
        uint16_t m_StringIndex;
    } string_info;

    struct FloatIntegerInfo {
        uint32_t m_Bytes;
    } integer_float_info;

    struct DoubleLongInfo {
        uint32_t m_HighBytes;
        uint32_t m_LowBytes;
    } long_double_info;

    struct NameAndTypeInfo {
        uint16_t m_NameIndex;
        uint16_t m_DescriptorIndex;
    } name_and_type_info;

    struct UTF8Info {
        uint16_t m_Length;
        uint8_t *m_Bytes;
    } utf8_info;

    struct MethodHandleInfo {
        uint8_t m_ReferenceKind;
        uint16_t m_ReferenceIndex;
    } method_handle_info;

    struct MethodTypeInfo {
        uint16_t m_DescriptorIndex;
    } method_type_info;

    struct DynamicInfo {
        uint16_t m_BoostrapMethodAttrIndex;
        uint16_t m_NameAndTypeIndex;
    } dynamic_info;

    struct ModulePackageInfo {
        uint16_t m_NameIndex;
    } module_package_info;
};

class ConstantPoolInfo {
public:
    enum ConstantTag : uint8_t {
        UNDEFINED = 0,
        UTF_8 = 1,
        INTEGER = 3,
        FLOAT = 4,
        LONG = 5,
        DOUBLE = 6,
        CLASS = 7,
        STRING = 8,
        FIELD_REF = 9,
        METHOD_REF = 10,
        INTERFACE_METHOD_REF = 11,
        NAME_AND_TYPE = 12,
        METHOD_HANDLE = 15,
        METHOD_TYPE = 16,
        DYNAMIC = 17,
        INVOKE_DYNAMIC = 18,
        MODULE = 19,
        PACKAGE = 20,
    };

public:
    ConstantPoolInfo();

    virtual ~ConstantPoolInfo();

public:
    [[nodiscard]] auto size() const -> unsigned int;

public:
    ConstantInfo m_Info{};
    ConstantTag m_Tag{};
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