#include "utils.h"

#include <filesystem>
#include <algorithm>
#include <iostream>

#include <boost/algorithm/string.hpp>

#include "class_reader.h"
#include "class_writer.h"

using namespace ares;

auto Manifest::content() const -> std::string {
    std::stringstream content;
    for (const auto &line: data) {
        content << line.first << ": " << line.second << std::endl;
    }
    return content.str();
}

auto Manifest::read_manifest(std::string &content) -> Manifest {
    std::istringstream stream(content);
    std::string line;

    Manifest manifest;
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

    return manifest;
}

auto JARFile::read_file(const std::string &path) -> JARFile {
    if (!boost::algorithm::iends_with(path, ".jar")) {
        throw std::invalid_argument("Warning: You can only enter \".jar\" files.");
    }

    int error = 0;
    zip_t *zip = zip_open(path.c_str(), 0, &error);

    if (!zip) {
        zip_error_t zip_error;
        zip_error_init_with_code(&zip_error, error);
        std::string error_message = zip_error_strerror(&zip_error);
        zip_error_fini(&zip_error);

        throw std::runtime_error("Warning: Couldn't open the ZIP File: " + error_message);
    }

    JARFile jar_file;

    zip_int64_t entries = zip_get_num_entries(zip, 0);
    for (zip_int64_t index = 0; index < entries; index++) {
        const char *file_name = zip_get_name(zip, index, 0);
        if (!file_name) continue;

        std::string name(file_name);

        zip_stat_t stat;
        zip_stat_init(&stat);
        if (zip_stat(zip, name.c_str(), 0, &stat) != 0) {
            throw std::runtime_error("Warning: Failed to retrieve ZIP entry info for: " + name);
        }

        auto data = std::vector<uint8_t>(stat.size);
        zip_file_t *file = zip_fopen(zip, name.c_str(), 0);
        if (!file) {
            throw std::runtime_error("Warning: Failed to open file in ZIP: " + name);
        }

        auto read = zip_fread(file, data.data(), stat.size);
        zip_fclose(file);

        if (read < 0) {
            throw std::runtime_error("Failed to read data from ZIP file: " + name);
        }

        if (name == "META-INF/MANIFEST.MF") {
            auto content = std::string(reinterpret_cast<char *>(data.data()), stat.size);
            jar_file.manifest = Manifest::read_manifest(content);
        } else if (boost::algorithm::iends_with(name, ".class")) {
            ClassFile class_file;
            class_file.byte_code = std::move(data);

            ClassReader classReader;
            classReader.visit_class(class_file);
            assert(classReader.offset() == stat.size);

            jar_file.classes.emplace(name, class_file);
        } else {
            jar_file.others.emplace(name, std::move(data));
        }
    }

    zip_close(zip);

    return jar_file;
}

auto JARFile::write_file(const std::string &path) -> void {
    if (!boost::algorithm::iends_with(path, ".jar")) {
        throw std::runtime_error("Warning: You can only enter \".jar\" files.");
    }

    if (std::filesystem::exists(path)) {
        std::filesystem::remove(path);
    }

    int error = 0;
    zip_t *zip = zip_open(path.c_str(), ZIP_CREATE | ZIP_TRUNCATE, &error);

    if (!zip) {
        zip_error_t zip_error;
        zip_error_init_with_code(&zip_error, error);
        std::string error_message = zip_error_strerror(&zip_error);
        zip_error_fini(&zip_error);

        throw std::runtime_error("Warning: Couldn't create the ZIP File: " + error_message);
    }

    for (auto &class_file: classes) {
        ClassWriter writer;
        writer.visit_class(class_file.second);

        auto &byte_code = writer.byte_code();
        _add_to_zip(zip, class_file.first, byte_code);
    }

    for (auto &item: others) {
        _add_to_zip(zip, item.first, item.second);
    }

    auto manifest_content = manifest.content();
    std::vector<uint8_t> content(manifest_content.begin(), manifest_content.end());

    _add_to_zip(zip, "META-INF/MANIFEST.MF", content);

    if (zip_close(zip) < 0) {
        throw std::runtime_error("Warning: Failed to close the ZIP archive.");
    }
}

void JARFile::_add_to_zip(zip_t *zip, const std::string &file_name, const std::vector<uint8_t> &data) {
    // Weird workaround. zip_file_add is not directly using the data and instead stores it until zip_close is being called.
    // The problem is that data is not always present (like generating bytecode using ClassWriter in a for loop).
    auto *data_copy = new uint8_t[data.size()];
    if (!data.empty()) {
        std::memcpy(data_copy, data.data(), data.size());
    }

    zip_source_t *source = zip_source_buffer(zip, data_copy, data.size(), 0);
    if (!source) {
        std::string error_message = zip_strerror(zip);
        throw std::runtime_error("Warning: Error creating source: " + error_message);
    }

    if (zip_file_add(zip, file_name.c_str(), source, ZIP_FL_OVERWRITE) < 0) {
        zip_source_free(source);

        std::string error_message = zip_strerror(zip);
        throw std::runtime_error("Warning: Error adding file to zip: " + error_message);
    }
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