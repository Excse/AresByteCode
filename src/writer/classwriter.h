//
// Created by timo on 16.11.20.
//

#pragma once

#include "../visitor/visitor.h"

namespace ares {

    class ClassWriter : Visitor {

        void visitClass(const ClassFile *classFile) const override;

        void visitClassCPInfo(const ClassFile *classFile,
                              const ConstantPoolInfo *constantPoolInfo) const override;

        void visitClassInterface(const ClassFile *classFile,
                                 uint16_t interface) const override;

        void visitClassField(const ClassFile *classFile,
                             const FieldInfo *fieldInfo) const override;

        void visitClassMethod(const ClassFile *classFile,
                              const MethodInfo *methodInfo) const override;

        void visitClassAttribute(const ClassFile *classFile,
                                 const AttributeInfo *attributeInfo) const override;

        void visitFieldAttribute(const ClassFile *classFile, const FieldInfo *fieldInfo,
                                 const AttributeInfo *attributeInfo) const override;

        void visitMethodAttribute(const ClassFile *classFile, const MethodInfo *methodInfo,
                                  const AttributeInfo *attributeInfo) const override;

    };

}