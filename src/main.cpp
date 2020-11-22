#include <iostream>
#include <iterator>
#include <algorithm>

#include "wrapper/classwrapper.h"
#include "reader/classreader.h"
#include "wrapper/classpool.h"
#include "visitor/vmcheck.h"
#include "utils/utils.h"

int main() {
    ares::AresConfiguration configuration{};
    if (ares::readJarFile("/home/timo/Desktop/Banana.jar", configuration) == EXIT_FAILURE)
        return EXIT_FAILURE;

    ares::ClassPool classPool(configuration.m_Classes);
    for (const auto &classInfo : configuration.m_Classes) {
//        auto classWrapper = classPool.getWrapper(classInfo);
//        std::cout << classWrapper->getName() << std::endl;

        ares::VMCheck vmCheck;
        vmCheck.visitClass(*classInfo);
    }

    if (ares::writeJarFile("/home/timo/Desktop/Banana_OBF.jar",
                           classPool, configuration) == EXIT_FAILURE)
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}