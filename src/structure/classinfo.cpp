#include "classinfo.h"

#include "attributeinfo.h"
#include "constantinfo.h"
#include "methodinfo.h"
#include "fieldinfo.h"

ares::ClassInfo::ClassInfo() = default;

ares::ClassInfo::~ClassInfo() = default;

bool ares::ClassInfo::isIndexValid(unsigned int index) const {
    return index > 0 && index < m_ConstantPoolCount;
}

bool ares::ClassInfo::hasAccessFlag(ares::ClassInfo::AccessFlag accessFlags) const {
    return m_AccessFlags & accessFlags;
}

unsigned int ares::ClassInfo::getSize() const {
    auto size = 24 + 2 * m_InterfacesCount;
    for(const auto &constantPoolInfo : m_ConstantPool) {
        if(!constantPoolInfo)
            continue;
        size += constantPoolInfo->getSize();
    }
    for(const auto &fieldInfo : m_Fields)
        size += fieldInfo->getSize();
    for(const auto &methodInfo : m_Methods)
        size += methodInfo->getSize();
    for(const auto &attributeInfo : m_Attributes)
        size += attributeInfo->getSize();
    return size;
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