#pragma once

#include <unordered_map>
#include <string>
#include <vector>
#include <memory>

#include "classinfo.h"

namespace ares {

class Manifest {
public:
    Manifest();

    virtual ~Manifest();

public:
    [[nodiscard]] auto content() -> std::string;

public:
    std::unordered_map <std::string, std::string> m_Data{};
};

class Configuration {
public:
    Configuration();

    virtual ~Configuration();

public:
    std::unordered_map <std::string, std::pair<uint8_t *, unsigned int>> m_Others{};
    std::unordered_map <std::string, std::shared_ptr<ClassInfo>> m_Classes{};
    std::shared_ptr <Manifest> m_Manifest{};
};

auto read_manifest(std::string &content, Manifest &manifest) -> int;

auto read_jar_file(const std::string &path, Configuration &configuration) -> int;

auto write_jar_file(const std::string &path, const Configuration &configuration) -> int;

auto read_u32(uint32_t & data,
              const uint8_t *byteCode,
              unsigned int size,
              unsigned int &offset) -> int;

auto write_u32(uint32_t & data,
               uint8_t * byteCode,
               unsigned int size,
               unsigned int &offset) -> int;

auto read_u16(uint16_t & data,
              const uint8_t *byteCode,
              unsigned int size,
              unsigned int &offset) -> int;

auto write_u16(uint16_t & data,
               uint8_t * byteCode,
               unsigned int size,
               unsigned int &offset) -> int;

auto read_u8(uint8_t & data,
             const uint8_t *byteCode,
             unsigned int size,
             unsigned int &offset) -> int;

auto write_u8(uint8_t & data,
              uint8_t * byteCode,
              unsigned int size,
              unsigned int &offset) -> int;

auto read_u8_array(uint8_t * data,
                   unsigned int length, uint8_t *byteCode,
                   unsigned int size,
                   unsigned int &offset) -> int;

auto write_u8_array(uint8_t * data,
                    unsigned int dataSize,
                    uint8_t *byteCode,
                    unsigned int size,
                    unsigned int &offset) -> int;

} // namespace ares

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