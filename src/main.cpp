#include <iostream>
#include <iterator>

#include "reader/classreader.h"
#include "visitor/vmcheck.h"
#include "utils/utils.h"
#include <algorithm>

int main() {
    ares::AresConfiguration configuration{};
    if (ares::readJarFile("/home/timo/Desktop/Banana.jar", configuration) == EXIT_FAILURE)
        return EXIT_FAILURE;

    for (const auto &classFile :configuration.m_Classes) {
        ares::VMCheck vmCheck;
        vmCheck.visitClass(*classFile.second);
    }

    if (ares::writeJarFile("/home/timo/Desktop/Banana_OBF.jar", configuration) == EXIT_FAILURE)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
