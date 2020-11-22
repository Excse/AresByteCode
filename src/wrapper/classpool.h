//
// Created by timo on 21.11.20.
//

#pragma once

#include <unordered_map>
#include <vector>
#include <memory>
#include "../structure/classinfo.h"

namespace ares {

    class ClassWrapper;


    class ClassPool {

    public:
        std::unordered_map<std::string, std::shared_ptr<ares::ClassWrapper>> m_Classes{};

    public:
        explicit ClassPool(std::vector<std::shared_ptr<ClassInfo>> &classes);

        virtual ~ClassPool();

    public:
        std::shared_ptr<ares::ClassWrapper>
        getWrapper(const std::shared_ptr<ares::ClassInfo> &classInfo);

        std::shared_ptr<ares::ClassWrapper> getWrapper(const std::string &className);

    };

}
