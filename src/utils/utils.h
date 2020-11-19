//
// Created by timo on 07.11.20.
//

#pragma once

#include <string>
#include <vector>

namespace ares {

    class AresConfiguration;

    class ClassFile;

    int readJarFile(const std::string &path, ares::AresConfiguration &configuration);

    int readU32(uint32_t &data, const ares::ClassFile &classFile, unsigned int &offset);

    int writeU32(uint32_t &data, std::vector<uint8_t> &byteCode, unsigned int &offset);

    int readU16(uint16_t &data, const ares::ClassFile &classFile, unsigned int &offset);

    int writeU16(uint16_t &data, std::vector<uint8_t> &byteCode, unsigned int &offset);

    int readU8(uint8_t &data, const ares::ClassFile &classFile, unsigned int &offset);

    int writeU8(uint8_t &data, std::vector<uint8_t> &byteCode, unsigned int &offset);

    int readU8Array(uint8_t *data, unsigned int size, const ares::ClassFile &classFile,
                    unsigned int &offset);

    int writeU8Array(uint8_t *data, unsigned int dataSize, std::vector<uint8_t>& byteCode,
                     unsigned int &offset);

}