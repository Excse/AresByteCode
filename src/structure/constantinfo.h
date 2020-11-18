//
// Created by timo on 16.11.20.
//

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
        } classInfo;
        struct FieldMethodInfo {
            uint16_t m_ClassIndex;
            uint16_t m_NameAndTypeIndex;
        } fieldMethodInfo;
        struct StringInfo {
            uint16_t m_StringIndex;
        } stringInfo;
        struct FloatIntegerInfo {
            uint32_t m_Bytes;
        } integerFloatInfo;
        struct DoubleLongInfo {
            uint32_t m_HighBytes;
            uint32_t m_LowBytes;
        } longDoubleInfo;
        struct NameAndTypeInfo {
            uint16_t m_NameIndex;
            uint16_t m_DescriptorIndex;
        } nameAndTypeInfo;
        struct UTF8Info {
            uint16_t m_Length;
            uint8_t *m_Bytes;
        } utf8Info;
        struct MethodHandleInfo {
            uint8_t m_ReferenceKind;
            uint16_t m_ReferenceIndex;
        } methodHandleInfo;
        struct MethodTypeInfo {
            uint16_t m_DescriptorIndex;
        } methodTypeInfo;
        struct DynamicInfo {
            uint16_t m_BoostrapMethodAttrIndex;
            uint16_t m_NameAndTypeIndex;
        } dynamicInfo;
        struct ModulePackageInfo {
            uint16_t m_NameIndex;
        } modulePackageInfo;
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
        ConstantInfo m_Info{};
        ConstantTag m_Tag{};

    public:
        ConstantPoolInfo();

        virtual ~ConstantPoolInfo();

    };

}