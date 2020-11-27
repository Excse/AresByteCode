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

    class AttributeInfo;

    class ClassInfo;


    class FieldInfo {

    public:
        enum AccessFlag : uint16_t {
            PUBLIC = 0x0001,
            PRIVATE = 0x0002,
            PROTECTED = 0x0004,
            STATIC = 0x0008,
            FINAL = 0x0010,
            VOLATILE = 0x0040,
            TRANSIENT = 0x0080,
            SYNTHETIC = 0x1000,
            ENUM = 0x4000,
        };

    public:
        uint16_t m_AccessFlags{};
        uint16_t m_NameIndex{};
        uint16_t m_DescriptorIndex{};
        uint16_t m_AttributesCount{};
        std::vector<std::shared_ptr<AttributeInfo>> m_Attributes{};

    public:
        FieldInfo();

        virtual ~FieldInfo();

    public:
        [[nodiscard]]
        bool hasAccessFlag(AccessFlag accessFlags) const;

        [[nodiscard]]
        unsigned int getSize() const;

    };

}