#include "utils.h"

#include <filesystem>
#include <algorithm>
#include <iostream>

#include <boost/algorithm/string.hpp>
#include <zip.h>

#include "class_reader.h"
#include "class_writer.h"

auto ares::Manifest::content() -> std::string {
    std::stringstream content;
    for (const auto &line: data) {
        content << line.first << ": " << line.second << std::endl;
    }
    return content.str();
}

auto ares::read_manifest(std::string &content, Manifest &manifest) -> int {
    std::istringstream stream(content);
    std::string line;

    while (std::getline(stream, line)) {
        boost::algorithm::trim(line);

        // Ignore comments and empty lines
        if (line.empty() || line[0] == '#') continue;

        std::vector<std::string> tokens;
        boost::split(tokens, line, boost::is_any_of(":"), boost::token_compress_on);

        // Skip malformed lines without a key-value pair
        if (tokens.size() < 2) continue;

        auto key = tokens[0];
        boost::algorithm::trim(key);

        if (key.empty()) continue;

        auto value = tokens[1];
        boost::algorithm::trim(value);

        manifest.data.emplace(std::move(key), std::move(value));
    }

    return EXIT_SUCCESS;
}

auto ares::read_jar_file(const std::string &path, Configuration &configuration) -> int {
    if (!boost::algorithm::iends_with(path, ".jar")) {
        std::cerr << "You can only enter \".jar\" files." << std::endl;
        return EXIT_FAILURE;
    }

    int error = 0;
    zip_t *zip = zip_open(path.c_str(), 0, &error);

    if (!zip) {
        zip_error_t zip_error;
        zip_error_init_with_code(&zip_error, error);
        std::cerr << zip_error_strerror(&zip_error) << std::endl;
        zip_error_fini(&zip_error);
        return EXIT_FAILURE;
    }

    zip_int64_t entries = zip_get_num_entries(zip, 0);
    for (zip_int64_t index = 0; index < entries; index++) {
        const char *file_name = zip_get_name(zip, index, 0);
        if (!file_name) continue;

        std::string name(file_name);

        zip_stat_t stat;
        zip_stat_init(&stat);
        if (zip_stat(zip, name.c_str(), 0, &stat) != 0) {
            std::cerr << "Warning: Failed to retrieve ZIP entry info for: " << name << "\n";
            continue;
        }

        auto data = std::vector<uint8_t>(stat.size);
        zip_file_t *file = zip_fopen(zip, name.c_str(), 0);
        if (!file) {
            std::cerr << "Warning: Failed to open file in ZIP: " << name << "\n";
            continue;
        }

        if (zip_fread(file, data.data(), stat.size) < 0) {
            std::cerr << "Warning: Failed to read file from ZIP: " << name << "\n";
            zip_fclose(file);
            continue;
        }

        zip_fclose(file);

        if (name == "META-INF/MANIFEST.MF") {
            auto manifest = std::make_shared<Manifest>();
            auto content = std::string(reinterpret_cast<char *>(data.data()), stat.size);
            ares::read_manifest(content, *manifest);
            configuration.manifest = manifest;
        } else if (boost::algorithm::iends_with(name, ".class")) {
            auto classInfo = std::make_shared<ClassInfo>();
            classInfo->byte_code = std::move(data);

            configuration.classes.emplace(name, classInfo);

            ClassReader classReader;
            classReader.visit_class(*classInfo);

            if (classReader.offset() != stat.size) {
                std::cerr << "The offset after reading the class doesn't match the class size" << std::endl;
                return EXIT_FAILURE;
            }
        } else {
            configuration.others.emplace(name, std::move(data));
        }
    }

    zip_close(zip);

    return EXIT_SUCCESS;
}

auto ares::write_jar_file(const std::string &path, const Configuration &configuration) -> int {
    if (!boost::algorithm::iends_with(path, ".jar")) {
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
    for (const auto &classInfo : configuration.classes) {
        ares::ClassWriter classWriter;
        classWriter.visit_class(*classInfo.second);

        auto stackByteCode = classWriter.byte_code();
        auto classSize = classInfo.second->size();

        auto heapByteCode = new uint8_t[classSize];
        for (size_t index = 0; index < classSize; index++) {
            heapByteCode[index] = stackByteCode[index];
        }
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

    for (const auto &other : configuration.others) {
        auto source = zip_source_buffer(zip, other.second.data(), other.second.size(), 0);
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

    auto manifest = configuration.manifest->content();
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

auto ares::read_u32(uint32_t &data, const std::vector<uint8_t> &byte_code, unsigned int &offset) -> bool {
    if (offset + 4 > byte_code.size()) {
        std::cerr << "Couldn't read u32 because it is out of bounds." << std::endl;
        return false;
    }

    data = be32toh(*((uint32_t *) &byte_code[offset]));
    offset += 4;

    return true;
}

auto ares::write_u32(uint32_t &data, std::vector<uint8_t> &byte_code, unsigned int &offset) -> bool {
    if (offset + 4 > byte_code.size()) {
        std::cerr << "Couldn't read u32 because it is out of bounds." << std::endl;
        return false;
    }

    byte_code[offset++] = (data >> 24) & 0xFF;
    byte_code[offset++] = (data >> 16) & 0xFF;
    byte_code[offset++] = (data >> 8) & 0xFF;
    byte_code[offset++] = data & 0xFF;

    return true;
}

auto ares::read_u16(uint16_t &data, const std::vector<uint8_t> &byte_code, unsigned int &offset) -> bool {
    if (offset + 2 > byte_code.size()) {
        std::cerr << "Couldn't read u16 because it is out of bounds." << std::endl;
        return false;
    }

    data = be16toh(*((uint16_t *) &byte_code[offset]));
    offset += 2;

    return true;
}

auto ares::write_u16(uint16_t &data, std::vector<uint8_t> &byte_code, unsigned int &offset) -> bool {
    if (offset + 2 > byte_code.size()) {
        std::cerr << "Couldn't write u16 because it is out of bounds." << std::endl;
        return false;
    }

    byte_code[offset++] = (data >> 8) & 0xFF;
    byte_code[offset++] = data & 0xFF;

    return true;
}

auto ares::read_u8(uint8_t &data, const std::vector<uint8_t> &byte_code, unsigned int &offset) -> bool {
    if (offset + 1 > byte_code.size()) {
        std::cerr << "Couldn't read u8 because it is out of bounds." << std::endl;
        return false;
    }

    data = *((uint8_t *) &byte_code[offset]);
    offset += 1;

    return true;
}

auto ares::write_u8(uint8_t &data, std::vector<uint8_t> &byte_code, unsigned int &offset) -> bool {
    if (offset + 1 > byte_code.size()) {
        std::cerr << "Couldn't write u8 because it is out of bounds." << std::endl;
        return false;
    }

    byte_code[offset++] = data & 0xFF;

    return true;
}

auto ares::read_u8_array(uint8_t *data,
                         unsigned int length,
                         const std::vector<uint8_t> &byteCode,
                         unsigned int &offset) -> bool {
    if ((offset + length) > byteCode.size()) {
        std::cerr << "Couldn't read the u8 array because it is out of bounds." << std::endl;
        return false;
    }

    for (size_t index = 0; index < length; index++)
        read_u8(data[index], byteCode, offset);

    return true;
}

auto ares::write_u8_array(uint8_t *data,
                          unsigned int data_size,
                          std::vector<uint8_t> &byte_code,
                          unsigned int &offset) -> bool {
    if ((offset + data_size) > byte_code.size()) {
        std::cerr << "Couldn't write the u8 array because it is out of bounds." << std::endl;
        return false;
    }

    for (size_t index = 0; index < data_size; index++)
        write_u8(data[index], byte_code, offset);

    return true;
}

//==============================================================================
// BSD 3-Clause License
//
// Copyright (c) 2025, Timo Behrend
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//==============================================================================