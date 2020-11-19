#include <iostream>
#include <chrono>
#include <iterator>
#include <zip.h>

#include "reader/classreader.h"
#include "visitor/vmcheck.h"
#include "utils/utils.h"
#include "writer/classwriter.h"

int noMatchIndex(const uint8_t *firstData, unsigned int firstLength,
                 const uint8_t *secondData, unsigned int secondLength) {
    auto index = 0;
    for (; index < std::min(firstLength, secondLength); index++) {
        if (firstData[index] != secondData[index])
            break;
    }

    return index;
}

int main() {
    ares::AresConfiguration configuration{};
    if (ares::readJarFile("/home/timo/Desktop/Banana.jar", configuration) == EXIT_FAILURE)
        return EXIT_FAILURE;

    auto error = 0;
    auto zipper = zip_open("/home/timo/Desktop/Banana_OBF.jar", ZIP_CREATE, &error);
    if (!zipper) {
        zip_error_t zipError;
        zip_error_init_with_code(&zipError, error);
        throw std::runtime_error(zip_error_strerror(&zipError));
    }

    for (const auto &classFile :configuration.m_Classes) {
        ares::VMCheck vmCheck;
        vmCheck.visitClass(*classFile.second);

        ares::ClassWriter classWriter(classFile.second->m_Size, 0);
        classWriter.visitClass(*classFile.second);

        std::vector<uint8_t> byteCode;
        classWriter.getByteCode(byteCode);

        int index = noMatchIndex(classFile.second->m_ByteCode, classFile.second->m_Size,
                                 byteCode.data(), byteCode.size());
        std::cout << index << " -> " << classFile.second->m_Size << std::endl;

        auto source = zip_source_buffer(zipper, byteCode.data(), byteCode.size(), 0);
        if (source == nullptr)
            throw std::runtime_error(std::string(zip_strerror(zipper)));
        if (zip_file_add(zipper, classFile.first.c_str(), source, ZIP_FL_UNCHANGED) < 0) {
            zip_source_free(source);
            throw std::runtime_error(std::string(zip_strerror(zipper)));
        }
    }

    for (const auto &other : configuration.m_Others) {
        auto source = zip_source_buffer(zipper, other.second->m_ByteCode, other.second->m_Size, 0);
        if (source == nullptr)
            throw std::runtime_error(std::string(zip_strerror(zipper)));
        if (zip_file_add(zipper, other.first.c_str(), source, ZIP_FL_ENC_UTF_8) < 0) {
            zip_source_free(source);
            throw std::runtime_error(std::string(zip_strerror(zipper)));
        }
    }

    auto source = zip_source_buffer(zipper, configuration.m_Manifest->m_ByteCode,
                                    configuration.m_Manifest->m_Size, 0);
    if (source == nullptr)
        throw std::runtime_error(std::string(zip_strerror(zipper)));
    if (zip_file_add(zipper, "META-INF/MANIFEST.MF", source, ZIP_FL_ENC_UTF_8) < 0) {
        zip_source_free(source);
        throw std::runtime_error(std::string(zip_strerror(zipper)));
    }

    zip_close(zipper);

    return EXIT_SUCCESS;
}
