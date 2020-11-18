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