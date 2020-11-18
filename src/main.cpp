#include <iostream>
#include <chrono>

#include "reader/classreader.h"
#include "visitor/vmcheck.h"
#include "utils/utils.h"

int main() {
    auto start = std::chrono::high_resolution_clock::now();

    ares::AresConfiguration configuration{};
    if (ares::readJarFile("/home/timo/Desktop/Banana.jar", configuration) == EXIT_FAILURE)
        return EXIT_FAILURE;

    ares::VMCheck vmCheck;
    for (const auto &classFile :configuration.m_Classes)
        vmCheck.visitClass(classFile.second.get());

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

    std::cout << "Time taken: " << duration.count() << "µs" << std::endl;

    return EXIT_SUCCESS;
}
