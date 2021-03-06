//
// Created by timo on 07.11.20.
//

#include "../../include/utils.h"

#include <filesystem>
#include <algorithm>
#include <iostream>

#include <zip.h>

#include "../../include/classreader.h"
#include "../../include/classwriter.h"

ares::Manifest::Manifest() = default;

ares::Manifest::~Manifest() = default;

std::string ares::Manifest::getContent() {
    std::stringstream content;
    for (const auto &line : m_Data)
        content << line.first << ": " << line.second << std::endl;
    return content.str();
}

ares::AresConfiguration::AresConfiguration() = default;

ares::AresConfiguration::~AresConfiguration() = default;

int ares::readManifest(std::string &content, ares::Manifest &manifest) {
    std::size_t current = content.find('\n'), previous = 0;
    while (current != std::string::npos) {
        auto line = content.substr(previous, current - previous);
        previous = current + 1;
        current = content.find('\n', previous);

        line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());
        if (line[0] == '#' || line.empty())
            continue;

        auto delimiterPos = line.find(':');
        auto key = line.substr(0, delimiterPos);
        auto value = line.substr(delimiterPos + 1);

        manifest.m_Data.emplace(key, value);
    }

    return EXIT_SUCCESS;
}

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

        auto arrayContents = new uint8_t[stat.size];
        auto file = zip_fopen(zip, name.c_str(), 0);
        zip_fread(file, arrayContents, stat.size);
        zip_fclose(file);

        if (name == "META-INF/MANIFEST.MF") {
            auto manifest = std::make_shared<Manifest>();
            auto content = std::string((char *) arrayContents);
            ares::readManifest(content, *manifest);
            configuration.m_Manifest = manifest;
        } else if (name.find(".class", name.size() - 6) != -1) {
            auto classInfo = std::make_shared<ClassInfo>();
            classInfo->m_ByteCode = arrayContents;
            classInfo->m_Size = stat.size;

            configuration.m_Classes.emplace(name, classInfo);

            ClassReader classReader;
            classReader.visitClass(*classInfo);

            if (classReader.getOffset() != classInfo->m_Size) {
                std::cerr << "The offset after reading the class doesn't match the class size"
                          << std::endl;
                return EXIT_FAILURE;
            }
        } else {
            configuration.m_Others.emplace(name, std::pair<uint8_t *, unsigned int>(
                    arrayContents, stat.size));
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
    for (const auto &classInfo : configuration.m_Classes) {
        ares::ClassWriter classWriter;
        classWriter.visitClass(*classInfo.second);

        auto stackByteCode = classWriter.getByteCode();
        auto classSize = classInfo.second->getSize();

        auto heapByteCode = new uint8_t[classSize];
        for (auto index = 0; index < classSize; index++)
            heapByteCode[index] = stackByteCode[index];
        releaseHeap.push_back(heapByteCode);

        auto source = zip_source_buffer(zip, heapByteCode, classSize, 0);
        if (source == nullptr) {
            std::cerr << zip_strerror(zip) << std::endl;
            return EXIT_FAILURE;
        }

        if (zip_file_add(zip, classInfo.first.c_str(), source, ZIP_FL_UNCHANGED) < 0) {
            zip_source_free(source);
            std::cerr << zip_strerror(zip) << std::endl;
            return EXIT_FAILURE;
        }
    }

    for (const auto &other : configuration.m_Others) {
        auto source = zip_source_buffer(zip, other.second.first, other.second.second, 0);
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

    auto manifest = configuration.m_Manifest->getContent();
    auto source = zip_source_buffer(zip, manifest.data(), manifest.size(), 0);
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

    return EXIT_SUCCESS;
}

int
ares::readU32(uint32_t &data, const uint8_t *byteCode, unsigned int size, unsigned int &offset) {
    if (offset + 4 > size) {
        std::cerr << "Couldn't read u32 because it is out of bounds." << std::endl;
        return EXIT_FAILURE;
    }

    data = be32toh(*(uint32_t *) (byteCode + offset));
    offset += 4;

    return EXIT_SUCCESS;
}

int ares::writeU32(uint32_t &data, uint8_t *byteCode, unsigned int size, unsigned int &offset) {
    if (offset + 4 > size) {
        std::cerr << "Couldn't read u32 because it is out of bounds." << std::endl;
        return EXIT_FAILURE;
    }

    byteCode[offset++] = (data >> 24) & 0xFF;
    byteCode[offset++] = (data >> 16) & 0xFF;
    byteCode[offset++] = (data >> 8) & 0xFF;
    byteCode[offset++] = data & 0xFF;

    return EXIT_SUCCESS;
}

int
ares::readU16(uint16_t &data, const uint8_t *byteCode, unsigned int size, unsigned int &offset) {
    if (offset + 2 > size) {
        std::cerr << "Couldn't read u16 because it is out of bounds." << std::endl;
        return EXIT_FAILURE;
    }

    data = be16toh(*(uint16_t *) (byteCode + offset));
    offset += 2;

    return EXIT_SUCCESS;
}

int ares::writeU16(uint16_t &data, uint8_t *byteCode, unsigned int size, unsigned int &offset) {
    if (offset + 2 > size) {
        std::cerr << "Couldn't write u16 because it is out of bounds." << std::endl;
        return EXIT_FAILURE;
    }

    byteCode[offset++] = (data >> 8) & 0xFF;
    byteCode[offset++] = data & 0xFF;

    return EXIT_SUCCESS;
}

int ares::readU8(uint8_t &data, const uint8_t *byteCode, unsigned int size, unsigned int &offset) {
    if (offset + 1 > size) {
        std::cerr << "Couldn't read u8 because it is out of bounds." << std::endl;
        return EXIT_FAILURE;
    }

    data = *(uint8_t *) (byteCode + offset);
    offset += 1;

    return EXIT_SUCCESS;
}

int ares::writeU8(uint8_t &data, uint8_t *byteCode, unsigned int size, unsigned int &offset) {
    if (offset + 1 > size) {
        std::cerr << "Couldn't write u8 because it is out of bounds." << std::endl;
        return EXIT_FAILURE;
    }

    byteCode[offset++] = data & 0xFF;

    return EXIT_SUCCESS;
}

int ares::readU8Array(uint8_t *data, unsigned int length, uint8_t *byteCode, unsigned int size,
                      unsigned int &offset) {
    if (offset + (length * 1) > size) {
        std::cerr << "Couldn't read the u8 array because it is out of bounds." << std::endl;
        return EXIT_FAILURE;
    }

    for (auto index = 0; index < length; index++)
        readU8(data[index], byteCode, size, offset);

    return EXIT_SUCCESS;
}

int ares::writeU8Array(uint8_t *data, unsigned int dataSize, uint8_t *byteCode, unsigned int size,
                       unsigned int &offset) {
    if (offset + (dataSize * 1) > size) {
        std::cerr << "Couldn't write the u8 array because it is out of bounds." << std::endl;
        return EXIT_FAILURE;
    }

    for (auto index = 0; index < dataSize; index++)
        writeU8(data[index], byteCode, size, offset);

    return EXIT_SUCCESS;
}