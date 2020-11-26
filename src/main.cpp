#include <algorithm>
#include <iostream>
#include <iterator>
#include <chrono>

#include "wrapper/classwrapper.h"
#include "reader/classreader.h"
#include "wrapper/classpool.h"
#include "visitor/vmcheck.h"
#include "utils/utils.h"

int main() {
    auto start = std::chrono::high_resolution_clock::now();

    ares::AresConfiguration configuration{};
    if (ares::readJarFile("/home/timo/Desktop/Banana.jar", configuration) == EXIT_FAILURE)
        return EXIT_FAILURE;

    ares::ClassPool classPool(configuration.m_Classes);
    for (const auto &classInfo : configuration.m_Classes) {
        ares::VMCheck vmCheck;
        vmCheck.visitClass(*classInfo);

//        auto classWrapper = classPool.getWrapper(classInfo);
//        std::cout << classWrapper->getName() << std::endl;
//
//        std::cout << " - Superclass: " << std::endl;
//        if(classWrapper->hasSuperClass()) {
//            if(auto superClass = classWrapper->getSuperClass()) {
//                std::cout << "  - " << (*superClass)->getName() << std::endl;
//            } else {
//                std::cout << "  - Not in the class pool." << std::endl;
//            }
//        }
//
//        std::cout << " - Interfaces: " << std::endl;
//        for(auto &interface : classWrapper->getInterfaces()) {
//            if(interface) {
//                std::cout << "  - " << (*interface)->getName() << std::endl;
//            } else {
//                std::cout << "  - Not in the class pool." << std::endl;
//            }
//        }
    }

    if (ares::writeJarFile("/home/timo/Desktop/Banana_OBF.jar",
                           classPool, configuration) == EXIT_FAILURE)
        return EXIT_FAILURE;

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "Time taken: " << duration.count() << "µs" << std::endl;

    return EXIT_SUCCESS;
}