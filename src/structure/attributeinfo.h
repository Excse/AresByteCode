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


    union AttributeType {
        struct ExceptionEntry {
            uint16_t m_StartPC;
            uint16_t m_EndPC;
            uint16_t m_HandlerPC;
            uint16_t m_CatchType;
        };
        struct Code {
            uint16_t m_MaxStack;
            uint16_t m_MaxLocals;
            uint32_t m_CodeLength;
            std::vector<uint8_t> m_Code;
            uint16_t m_ExceptionTableLength;
            std::vector<ExceptionEntry> m_ExceptionTable;
            uint16_t m_AttributesCount;
            std::vector<AttributeInfo> m_Attributes;
        };
    };

    struct AttributeInfo {

    public:
        uint16_t m_AttributeNameIndex{};
        uint32_t m_AttributeLength{};
        std::vector<uint8_t> m_Info{};

    public:
        AttributeInfo();

        virtual ~AttributeInfo();

    };

}