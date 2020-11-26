//
// Created by timo on 21.11.20.
//

#pragma once

#include "../structure/classinfo.h"

namespace ares {

    class ClassPool;


    class ClassWrapper {

    public:
        std::shared_ptr<ares::ClassInfo> m_ClassFile{};
        ares::ClassPool *m_ClassPool;

    public:
        ClassWrapper(ares::ClassPool *classPool, std::shared_ptr<ares::ClassInfo> classInfo);

        virtual ~ClassWrapper();

    public:
        std::vector<std::optional<std::shared_ptr<ares::ClassWrapper>>> getInterfaces();

        std::optional<std::shared_ptr<ares::ClassWrapper>> getSuperClass();

        bool hasSuperClass();

        [[deprecated]]
        void setName(const std::string &name);

        std::string getName();

    };

}