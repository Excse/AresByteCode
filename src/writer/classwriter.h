//
// Created by timo on 16.11.20.
//

#pragma once

#include "../visitor/visitor.h"

namespace ares {

    class ClassWriter : Visitor {

    private:
        unsigned int m_Offset{}, m_Size{};
        uint8_t *m_ByteCode{};

    public:
        explicit ClassWriter(unsigned int offset = 0);

        virtual ~ClassWriter();

    public:
        void visitClass(ClassInfo &classInfo) override;

    private:
        void visitClassCPInfo(ares::ClassInfo &classInfo,
                              ares::ConstantPoolInfo &info) override;

        void visitClassInterface(ares::ClassInfo &classInfo,
                                 uint16_t interface) override;

        void visitClassField(ares::ClassInfo &classInfo,
                             ares::FieldInfo &fieldInfo) override;

        void visitClassMethod(ares::ClassInfo &classInfo,
                              ares::MethodInfo &methodInfo) override;

        void visitClassAttribute(ares::ClassInfo &classInfo,
                                 ares::AttributeInfo &attributeInfo) override;

        void visitFieldAttribute(ares::ClassInfo &classInfo,
                                 ares::FieldInfo &fieldInfo,
                                 ares::AttributeInfo &attributeInfo) override;

        void visitMethodAttribute(ares::ClassInfo &classInfo,
                                  ares::MethodInfo &methodInfo,
                                  ares::AttributeInfo &attributeInfo) override;

    public:
        [[nodiscard]]
        uint8_t *getByteCode() const;

    };

}