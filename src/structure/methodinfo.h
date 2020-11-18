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


    class MethodInfo {

    public:
        enum AccessFlag : uint16_t {
            PUBLIC = 0x0001,
            PRIVATE = 0x0002,
            PROTECTED = 0x0004,
            STATIC = 0x0008,
            FINAL = 0x0010,
            SYNCHRONIZED = 0x0020,
            BRIDGE = 0x0040,
            VARARGS = 0x0080,
            NATIVE = 0x0100,
            ABSTRACT = 0x0400,
            STRICT = 0x0800,
            SYNTHETIC = 0x1000,
        };

    public:
        uint16_t m_AccessFlags{};
        uint16_t m_NameIndex{};
        uint16_t m_DescriptorIndex{};
        uint16_t m_AttributesCount{};
        std::vector<std::shared_ptr<AttributeInfo>> m_Attributes{};

    public:
        MethodInfo();

        virtual ~MethodInfo();

    public:
        [[nodiscard]]
        bool hasAccessFlag(AccessFlag accessFlags) const;

    };

}