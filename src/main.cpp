#include <iostream>
#include <iterator>
#include <algorithm>

#include "wrapper/classwrapper.h"
#include "reader/classreader.h"
#include "visitor/vmcheck.h"
#include "utils/utils.h"
#include "wrapper/classpool.h"

int main() {
    ares::AresConfiguration configuration{};
    if (ares::readJarFile("/home/timo/Desktop/Banana.jar", configuration) == EXIT_FAILURE)
        return EXIT_FAILURE;

    ares::ClassPool classPool(configuration.m_Classes);
    for (const auto &classFile : configuration.m_Classes) {
        auto classWrapper = classPool.getWrapper(classFile);
        std::cout << classWrapper->getName() << std::endl;

        if (classWrapper->hasSuperClass()) {
            auto superClass = classWrapper->getSuperClass();
            if(superClass)
                std::cout << " Super: " << superClass->getName() << std::endl;
            else
                std::cout << " Super: Not in the class pool." << std::endl;
        }

        auto interfaces = classWrapper->getInterfaces();
        std::cout << " Interfaces: "
                  << (interfaces.empty() ? "N/A" : std::to_string(interfaces.size())) << std::endl;
        for(auto const &interface : interfaces) {
            if(interface == nullptr) {
                std::cout << "  - Not in the class pool." << std::endl;
                continue;
            }

            std::cout << "  - " << interface->getName() << std::endl;
        }

        ares::VMCheck vmCheck;
        vmCheck.visitClass(*classFile);
    }

    if (ares::writeJarFile("/home/timo/Desktop/Banana_OBF.jar",
                           classPool, configuration) == EXIT_FAILURE)
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}