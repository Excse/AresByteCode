#include <algorithm>
#include <iostream>
#include <chrono>

#include "gtest/gtest.h"

#include "vm_check.h"
#include "utils.h"

static const std::string TEST_FILE_OUTPUT = TEST_PATH "/resources/hello_world_out.jar";
static const std::string TEST_FILE_INPUT = TEST_PATH "/resources/hello_world_in.jar";

TEST(General, Works) {
    auto start = std::chrono::high_resolution_clock::now();

    ares::Configuration configuration{};
    if (ares::read_jar_file(TEST_FILE_INPUT, configuration) == EXIT_FAILURE) {
        std::cerr << "Failed to read the file" << std::endl;
        exit(EXIT_FAILURE);
    }

    ares::VMCheck vmCheck;
    for (const auto &classInfo : configuration.classes)
        vmCheck.visit_class(*classInfo.second);

    if (ares::write_jar_file(TEST_FILE_OUTPUT, configuration) == EXIT_FAILURE) {
        std::cerr << "Failed to write the file" << std::endl;
        exit(EXIT_FAILURE);
    }

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "Time taken: " << duration.count() << "µs" << std::endl;
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