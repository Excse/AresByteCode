#include <algorithm>
#include <iostream>
#include <iterator>
#include <chrono>

#include "../include/vmcheck.h"
#include "../include/utils.h"

int main() {
    auto start = std::chrono::high_resolution_clock::now();

    ares::AresConfiguration configuration{};
    if (ares::readJarFile("/home/timo/Desktop/Banana.jar", configuration) == EXIT_FAILURE)
        return EXIT_FAILURE;

    ares::VMCheck vmCheck;
    for (const auto &classInfo : configuration.m_Classes)
        vmCheck.visitClass(*classInfo.second);

    if (ares::writeJarFile("/home/timo/Desktop/Banana_OBF.jar", configuration) == EXIT_FAILURE)
        return EXIT_FAILURE;

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "Time taken: " << duration.count() << "µs" << std::endl;

    return EXIT_SUCCESS;
}