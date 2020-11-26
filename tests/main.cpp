#include <algorithm>
#include <iostream>
#include <iterator>
#include <chrono>

#include "reader/classreader.h"
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
    }

    if (ares::writeJarFile("/home/timo/Desktop/Banana_OBF.jar",
                           classPool, configuration) == EXIT_FAILURE)
        return EXIT_FAILURE;

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "Time taken: " << duration.count() << "µs" << std::endl;

    return EXIT_SUCCESS;
}