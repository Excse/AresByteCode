//
// Created by timo on 21.11.20.
//

#include "classwrapper.h"

#include <iostream>
#include <utility>

#include "../structure/constantinfo.h"
#include "classpool.h"

ares::ClassWrapper::ClassWrapper(ares::ClassPool *classPool,
                                 std::shared_ptr<ares::ClassInfo> classInfo)
        : m_ClassPool(classPool), m_ClassFile(std::move(classInfo)) {}

ares::ClassWrapper::~ClassWrapper() = default;

std::vector<std::optional<std::shared_ptr<ares::ClassWrapper>>>
ares::ClassWrapper::getInterfaces() {
    std::vector<std::optional<std::shared_ptr<ares::ClassWrapper>>> interfaces;

    for (auto const &interface : m_ClassFile->m_Interfaces) {
        auto classInfo = m_ClassFile->m_ConstantPool[interface - 1];
        if (classInfo->m_Tag != ares::ConstantPoolInfo::CLASS)
            abort();

        auto utf8Info = m_ClassFile->m_ConstantPool[classInfo->m_Info.classInfo.m_NameIndex - 1];
        if (utf8Info->m_Tag != ares::ConstantPoolInfo::UTF_8)
            abort();

        std::string name((char *) utf8Info->m_Info.utf8Info.m_Bytes,
                         utf8Info->m_Info.utf8Info.m_Length);

        auto classWrapper = m_ClassPool->getWrapper(name);
        if (!classWrapper) {
            interfaces.emplace_back(std::nullopt);
            continue;
        }

        interfaces.emplace_back(classWrapper);
    }

    return interfaces;
}

std::optional<std::shared_ptr<ares::ClassWrapper>> ares::ClassWrapper::getSuperClass() {
    if (m_ClassFile->m_SuperClass == 0)
        return std::nullopt;

    auto classInfo = m_ClassFile->m_ConstantPool[m_ClassFile->m_SuperClass - 1];
    if (classInfo->m_Tag != ares::ConstantPoolInfo::CLASS)
        abort();

    auto utf8Info = m_ClassFile->m_ConstantPool[classInfo->m_Info.classInfo.m_NameIndex - 1];
    if (utf8Info->m_Tag != ares::ConstantPoolInfo::UTF_8)
        abort();

    std::string name((char *) utf8Info->m_Info.utf8Info.m_Bytes,
                     utf8Info->m_Info.utf8Info.m_Length);

    auto classWrapper = m_ClassPool->getWrapper(name);
    if (!classWrapper)
        return std::nullopt;
    return classWrapper;
}

bool ares::ClassWrapper::hasSuperClass() {
    return m_ClassFile->m_SuperClass != 0;
}

void ares::ClassWrapper::setName(const std::string &name) {
    auto classInfo = m_ClassFile->m_ConstantPool[m_ClassFile->m_ThisClass - 1];
    if (classInfo->m_Tag != ares::ConstantPoolInfo::CLASS)
        abort();

    auto utf8Info = m_ClassFile->m_ConstantPool[classInfo->m_Info.classInfo.m_NameIndex - 1];
    if (utf8Info->m_Tag != ares::ConstantPoolInfo::UTF_8)
        abort();

//    auto oldName = std::string((char *) utf8Info->m_Info.utf8Info.m_Bytes,
//                               utf8Info->m_Info.utf8Info.m_Length);
//    auto oldPointer = m_ClassPool->m_Classes[oldName];
//    m_ClassPool->m_Classes.erase(oldName);
//    m_ClassPool->m_Classes[oldName] = oldPointer;

    delete[] utf8Info->m_Info.utf8Info.m_Bytes;

    auto newName = new uint8_t[name.size()];
    for (auto index = 0; index < name.size(); index++)
        newName[index] = name[index];

    utf8Info->m_Info.utf8Info.m_Bytes = newName;
    utf8Info->m_Info.utf8Info.m_Length = name.size();
}

std::string ares::ClassWrapper::getName() {
    auto classInfo = m_ClassFile->m_ConstantPool[m_ClassFile->m_ThisClass - 1];
    if (classInfo->m_Tag != ares::ConstantPoolInfo::CLASS)
        abort();

    auto utf8Info = m_ClassFile->m_ConstantPool[classInfo->m_Info.classInfo.m_NameIndex - 1];
    if (utf8Info->m_Tag != ares::ConstantPoolInfo::UTF_8)
        abort();

    return std::string((char *) utf8Info->m_Info.utf8Info.m_Bytes,
                       utf8Info->m_Info.utf8Info.m_Length);
}