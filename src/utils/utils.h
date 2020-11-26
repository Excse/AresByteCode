//
// Created by timo on 07.11.20.
//

#pragma once

#include <unordered_map>
#include <string>
#include <vector>
#include <memory>

#include "../structure/classinfo.h"
#include "../wrapper/classpool.h"

namespace ares {

    class Manifest {

    public:
        std::unordered_map<std::string, std::string> m_Data{};

    public:
        Manifest();

        virtual ~Manifest();

    public:
        std::string getContent();

    };

    class AresConfiguration {

    public:
        std::unordered_map<std::string, std::pair<uint8_t *, unsigned int>> m_Others{};
        std::vector<std::shared_ptr<ares::ClassInfo>> m_Classes{};
        std::shared_ptr<ares::Manifest> m_Manifest{};

    public:
        AresConfiguration();

        virtual ~AresConfiguration();

    };

    int readManifest(std::string &content, ares::Manifest &manifest);

    int readJarFile(const std::string &path, ares::AresConfiguration &configuration);

    int writeJarFile(const std::string &path, ares::ClassPool &classPool,
                     const ares::AresConfiguration &configuration);

    int readU32(uint32_t &data, uint8_t *byteCode, unsigned int size, unsigned int &offset);

    int writeU32(uint32_t &data, uint8_t *byteCode, unsigned int size, unsigned int &offset);

    int readU16(uint16_t &data, uint8_t *byteCode, unsigned int size, unsigned int &offset);

    int writeU16(uint16_t &data, uint8_t *byteCode, unsigned int size, unsigned int &offset);

    int readU8(uint8_t &data, uint8_t *byteCode, unsigned int size, unsigned int &offset);

    int writeU8(uint8_t &data, uint8_t *byteCode, unsigned int size, unsigned int &offset);

    int readU8Array(uint8_t *data, unsigned int length, uint8_t *byteCode, unsigned int size,
                    unsigned int &offset);

    int writeU8Array(uint8_t *data, unsigned int dataSize, uint8_t *byteCode, unsigned int size,
                     unsigned int &offset);

}