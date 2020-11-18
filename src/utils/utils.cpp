//
// Created by timo on 07.11.20.
//

#include "utils.h"

#include <iostream>

#include <zip.h>

#include "../reader/classreader.h"
#include "../structure/classfile.h"

int ares::readJarFile(const std::string &path, ares::AresConfiguration &configuration) {
    auto correctSuffix = path.find(".jar", path.size() - 4) != -1;
    if (!correctSuffix) {
        std::cerr << "You can only enter \".jar\" files." << std::endl;
        return EXIT_FAILURE;
    }

    if (access(path.c_str(), F_OK) == -1) {
        std::cerr << "The given file doesn't exist: \"" << path << "\"" << std::endl;
        return EXIT_FAILURE;
    }

    auto error = 0;
    auto zip = zip_open("/home/timo/Desktop/Banana.jar", 0, &error);

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

            auto offset = 0u;
            if (ClassReader::readClass(*jarFile, offset) == EXIT_FAILURE)
                return EXIT_FAILURE;

            if (offset != jarFile->m_Size) {
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

int ares::readU32(uint32_t &data, const ClassFile &classFile, unsigned int &offset) {
    if (offset + 4 > classFile.m_Size) {
        std::cerr << "Couldn't read u32 because it is out of bounds." << std::endl;
        return EXIT_FAILURE;
    }

    data = be32toh(*(uint32_t *) (classFile.m_ByteCode + offset));
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

int ares::readU8(uint8_t &data, const ares::ClassFile &classFile, unsigned int &offset) {
    if (offset + 1 > classFile.m_Size) {
        std::cerr << "Couldn't read u8 because it is out of bounds." << std::endl;
        return EXIT_FAILURE;
    }

    data = *(uint8_t *) (classFile.m_ByteCode + offset);
    offset += 1;

    return EXIT_SUCCESS;
}

int ares::readU8Array(uint8_t *data, unsigned int size, const ares::ClassFile &classFile,
                      unsigned int &offset) {
    if (offset + (size * 1) > classFile.m_Size) {
        std::cerr << "Couldn't read u8 array because it is out of bounds." << std::endl;
        return EXIT_FAILURE;
    }

    for (auto index = 0; index < size; index++)
        readU8(data[index], classFile, offset);

    return EXIT_SUCCESS;
}
