#include <iostream>
#include <chrono>
#include <iterator>

#include "reader/classreader.h"
#include "visitor/vmcheck.h"
#include "utils/utils.h"
#include "writer/classwriter.h"

int main() {
    auto start = std::chrono::high_resolution_clock::now();

    ares::AresConfiguration configuration{};
    if (ares::readJarFile("/home/timo/Desktop/Banana.jar", configuration) == EXIT_FAILURE)
        return EXIT_FAILURE;

    for (const auto &classFile :configuration.m_Classes) {
        ares::VMCheck vmCheck;
        vmCheck.visitClass(*classFile.second);

        auto byteCode = new uint8_t[classFile.second->m_Size];
        ares::ClassWriter classWriter(byteCode, classFile.second->m_Size);
        classWriter.visitClass(*classFile.second);

        std::cout << "Original: " << std::endl;
        for(int index = 0; index < 20; index++)
            std::cout << (int) classFile.second->m_ByteCode[index] << ", ";
        std::cout << std::endl;

        std::cout << "New: " << std::endl;
        for(int index = 0; index < 20; index++)
            std::cout << (int) byteCode[index] << ", ";
        std::cout << std::endl;

        delete[] byteCode;
    }

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

    std::cout << "Time taken: " << duration.count() << "µs" << std::endl;

    return EXIT_SUCCESS;
}
