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

    class ConstantPoolInfo;

    class AttributeInfo;

    class MethodInfo;

    class FieldInfo;


    class ClassFile {

    public:
        enum AccessFlag : uint16_t {
            PUBLIC = 0x0001,
            FINAL = 0x0010,
            SUPER = 0x0020,
            INTERFACE = 0x0200,
            ABSTRACT = 0x0400,
            SYNTHETIC = 0x1000,
            ANNOTATION = 0x2000,
            ENUM = 0x4000,
            MODULE = 0x8000,
        };

        enum ClassVersion : uint16_t {
            UNDEFINED = 0,
            VERSION_1_1 = 45,
            VERSION_1_2 = 46,
            VERSION_1_3 = 47,
            VERSION_1_4 = 48,
            VERSION_5 = 49,
            VERSION_6 = 50,
            VERSION_7 = 51,
            VERSION_8 = 52,
            VERSION_9 = 53,
            VERSION_10 = 54,
            VERSION_11 = 55,
            VERSION_12 = 56,
            VERSION_13 = 57,
            VERSION_14 = 58,
            VERSION_15 = 59,
        };

    public:
        uint8_t *m_ByteCode{};
        unsigned int m_Size{};

        uint32_t m_MagicNumber{};
        ClassVersion m_ClassVersion{};
        uint16_t m_ConstantPoolCount{};
        std::vector <std::shared_ptr<ConstantPoolInfo>> m_ConstantPool{};
        uint16_t m_AccessFlags{};
        uint16_t m_ThisClass{};
        uint16_t m_SuperClass{};
        uint16_t m_InterfacesCount{};
        std::vector <uint16_t> m_Interfaces{};
        uint16_t m_FieldsCount{};
        std::vector <std::shared_ptr<FieldInfo>> m_Fields{};
        uint16_t m_MethodCount{};
        std::vector <std::shared_ptr<MethodInfo>> m_Methods{};
        uint16_t m_AttributesCount{};
        std::vector <std::shared_ptr<AttributeInfo>> m_Attributes{};

    public:
        ClassFile();

        virtual ~ClassFile();

    public:
        [[nodiscard]]
        bool isIndexValid(unsigned int index) const;

        [[nodiscard]]
        bool hasAccessFlag(AccessFlag accessFlags) const;

    };

}