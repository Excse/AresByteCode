//
// Created by timo on 07.11.20.
//

#include "utils.h"

#include <filesystem>
#include <algorithm>
#include <iostream>

#include <zip.h>

#include "../reader/classreader.h"
#include "../writer/classwriter.h"

int ares::readJarFile(const std::string &path, ares::AresConfiguration &configuration) {
    auto correctSuffix = path.find(".jar", path.size() - 4) != -1;
    if (!correctSuffix) {
        std::cerr << "You can only enter \".jar\" files." << std::endl;
        return EXIT_FAILURE;
    }

    auto error = 0;
    auto zip = zip_open("/home/timo/Desktop/Banana.jar", 0, &error);
    if (!zip) {
        zip_error_t zipError;
        zip_error_init_with_code(&zipError, error);
        std::cerr << zip_error_strerror(&zipError) << std::endl;
        return EXIT_FAILURE;
    }

    auto entries = zip_get_num_entries(zip, 0);
    for (auto index = 0; index < entries; index++) {
        auto name = std::string(zip_get_name(zip, index, 0));

        struct zip_stat stat{};
        zip_stat_init(&stat);
        zip_stat(zip, name.c_str(), 0, &stat);

        auto contents = new uint8_t[stat.size];
        auto file = zip_fopen(zip, name.c_str(), 0);
        zip_fread(file, contents, stat.size);
        zip_fclose(file);

        auto jarFile = std::make_shared<ClassFile>();
        jarFile->m_ByteCode = contents;
        jarFile->m_Size = stat.size;

        if (name == "META-INF/MANIFEST.MF") {
            configuration.m_Manifest = jarFile;
        } else if (name.find(".class", name.size() - 6) != -1) {
            configuration.m_Classes.emplace(name, jarFile);

            ClassReader classReader;
            classReader.visitClass(*jarFile);

            if (classReader.getOffset() != jarFile->m_Size) {
                std::cerr << "The offset after reading the class doesn't match the class size"
                          << std::endl;
                return EXIT_FAILURE;
            }
        } else {
            configuration.m_Others.emplace(name, jarFile);
        }
    }

    zip_close(zip);

    return EXIT_SUCCESS;
}

int ares::writeJarFile(const std::string &path, const ares::AresConfiguration &configuration) {
    auto correctSuffix = path.find(".jar", path.size() - 4) != -1;
    if (!correctSuffix) {
        std::cerr << "You can only enter \".jar\" files." << std::endl;
        return EXIT_FAILURE;
    }

    if (std::filesystem::exists(path))
        std::filesystem::remove(path);

    auto error = 0;
    auto zip = zip_open(path.c_str(), ZIP_CREATE, &error);
    if (!zip) {
        zip_error_t zipError;
        zip_error_init_with_code(&zipError, error);
        std::cerr << zip_error_strerror(&zipError) << std::endl;
        return EXIT_FAILURE;
    }

    std::vector<uint8_t *> releaseHeap;
    for (const auto &classFile :configuration.m_Classes) {
        ares::ClassWriter classWriter(classFile.second->m_Size, 0);
        classWriter.visitClass(*classFile.second);

        std::vector<uint8_t> stackByteCode;
        classWriter.getByteCode(stackByteCode);

        auto heapByteCode = new uint8_t[stackByteCode.size()];
        for (auto index = 0; index < stackByteCode.size(); index++)
            heapByteCode[index] = stackByteCode[index];
        releaseHeap.push_back(heapByteCode);

        auto source = zip_source_buffer(zip, heapByteCode, stackByteCode.size(), 0);
        if (source == nullptr) {
            std::cerr << zip_strerror(zip) << std::endl;
            return EXIT_FAILURE;
        }

        if (zip_file_add(zip, classFile.first.c_str(), source, ZIP_FL_UNCHANGED) < 0) {
            zip_source_free(source);
            std::cerr << zip_strerror(zip) << std::endl;
            return EXIT_FAILURE;
        }
    }

    for (const auto &other : configuration.m_Others) {
        auto source = zip_source_buffer(zip, other.second->m_ByteCode, other.second->m_Size, 0);
        if (source == nullptr) {
            std::cerr << zip_strerror(zip) << std::endl;
            return EXIT_FAILURE;
        }

        if (zip_file_add(zip, other.first.c_str(), source, ZIP_FL_UNCHANGED) < 0) {
            zip_source_free(source);
            std::cerr << zip_strerror(zip) << std::endl;
            return EXIT_FAILURE;
        }
    }

    auto source = zip_source_buffer(zip, configuration.m_Manifest->m_ByteCode,
                                    configuration.m_Manifest->m_Size, 0);
    if (source == nullptr) {
        std::cerr << zip_strerror(zip) << std::endl;
        return EXIT_FAILURE;
    }

    if (zip_file_add(zip, "META-INF/MANIFEST.MF", source, ZIP_FL_UNCHANGED) < 0) {
        zip_source_free(source);
        std::cerr << zip_strerror(zip) << std::endl;
        return EXIT_FAILURE;
    }

    zip_close(zip);

    for(const auto &release : releaseHeap)
        delete [] release;

    return EXIT_FAILURE;
}

int ares::readU32(uint32_t &data, const ClassFile &classFile, unsigned int &offset) {
    if (offset + 4 > classFile.m_Size) {
        std::cerr << "Couldn't read u32 because it is out of bounds." << std::endl;
        return EXIT_FAILURE;
    }

    data = be32toh(*(uint32_t *) (classFile.m_ByteCode + offset));
    offset += 4;

    return EXIT_SUCCESS;
}

int ares::writeU32(uint32_t &data, std::vector<uint8_t> &byteCode, unsigned int &offset) {
    if (offset + 4 > byteCode.size())
        byteCode.resize(offset + 4);

    byteCode[offset + 0] = (data >> 24) & 0xFF;
    byteCode[offset + 1] = (data >> 16) & 0xFF;
    byteCode[offset + 2] = (data >> 8) & 0xFF;
    byteCode[offset + 3] = data & 0xFF;
    offset += 4;

    return EXIT_SUCCESS;
}

int ares::readU16(uint16_t &data, const ares::ClassFile &classFile, unsigned int &offset) {
    if (offset + 2 > classFile.m_Size) {
        std::cerr << "Couldn't read u16 because it is out of bounds." << std::endl;
        return EXIT_FAILURE;
    }

    data = be16toh(*(uint16_t *) (classFile.m_ByteCode + offset));
    offset += 2;

    return EXIT_SUCCESS;
}

int ares::writeU16(uint16_t &data, std::vector<uint8_t> &byteCode, unsigned int &offset) {
    if (offset + 2 > byteCode.size())
        byteCode.resize(offset + 2);

    byteCode[offset + 0] = (data >> 8) & 0xFF;
    byteCode[offset + 1] = data & 0xFF;
    offset += 2;

    return EXIT_SUCCESS;
}

int ares::readU8(uint8_t &data, const ares::ClassFile &classFile, unsigned int &offset) {
    if (offset + 1 > classFile.m_Size) {
        std::cerr << "Couldn't read u8 because it is out of bounds." << std::endl;
        return EXIT_FAILURE;
    }

    data = *(uint8_t *) (classFile.m_ByteCode + offset);
    offset += 1;

    return EXIT_SUCCESS;
}

int ares::writeU8(uint8_t &data, std::vector<uint8_t> &byteCode, unsigned int &offset) {
    if (offset + 1 > byteCode.size())
        byteCode.resize(offset + 1);

    byteCode[offset] = data & 0xFF;
    offset += 1;

    return EXIT_SUCCESS;
}

int ares::readU8Array(uint8_t *data, unsigned int size, const ares::ClassFile &classFile,
                      unsigned int &offset) {
    if (offset + (size * 1) > classFile.m_Size) {
        std::cerr << "Couldn't read the u8 array because it is out of bounds." << std::endl;
        return EXIT_FAILURE;
    }

    for (auto index = 0; index < size; index++)
        readU8(data[index], classFile, offset);

    return EXIT_SUCCESS;
}

int ares::writeU8Array(uint8_t *data, unsigned int dataSize, std::vector<uint8_t> &byteCode,
                       unsigned int &offset) {
    if (offset + (dataSize * 1) > byteCode.size())
        byteCode.resize(offset + (dataSize * 1));

    for (auto index = 0; index < dataSize; index++)
        writeU8(data[index], byteCode, offset);

    return EXIT_SUCCESS;
}