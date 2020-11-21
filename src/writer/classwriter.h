//
// Created by timo on 16.11.20.
//

#pragma once

#include "../visitor/visitor.h"

namespace ares {

    class ClassWriter : Visitor {

    private:
        std::vector<uint8_t> m_ByteCode{};
        unsigned int m_Offset;

    public:
        explicit ClassWriter(unsigned int size = 0, unsigned int offset = 0);

        virtual ~ClassWriter();

    public:
        void visitClass(ClassInfo &classFile) override;

        void getByteCode(std::vector<uint8_t> &byteCode);

    private:
        void visitClassCPInfo(ares::ClassInfo &classFile,
                              ares::ConstantPoolInfo &info) override;

        void visitClassInterface(ares::ClassInfo &classFile,
                                 uint16_t interface) override;

        void visitClassField(ares::ClassInfo &classFile,
                             ares::FieldInfo &fieldInfo) override;

        void visitClassMethod(ares::ClassInfo &classFile,
                              ares::MethodInfo &methodInfo) override;

        void visitClassAttribute(ares::ClassInfo &classFile,
                                 ares::AttributeInfo &attributeInfo) override;

        void visitFieldAttribute(ares::ClassInfo &classFile,
                                 ares::FieldInfo &fieldInfo,
                                 ares::AttributeInfo &attributeInfo) override;

        void visitMethodAttribute(ares::ClassInfo &classFile,
                                  ares::MethodInfo &methodInfo,
                                  ares::AttributeInfo &attributeInfo) override;

    };

}