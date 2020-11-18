//
// Created by timo on 15.11.20.
//

#pragma once

#include "../structure/attributeinfo.h"
#include "../structure/methodinfo.h"
#include "../structure/fieldinfo.h"
#include "../structure/classfile.h"

namespace ares {

    class Visitor {

    public:
        virtual void visitClass(const ClassFile *classFile) const = 0;

        virtual void visitClassCPInfo(const ClassFile *classFile,
                                      const ConstantPoolInfo *constantPoolInfo) const = 0;

        virtual void visitClassInterface(const ClassFile *classFile,
                                         uint16_t interface) const = 0;

        virtual void visitClassField(const ClassFile *classFile,
                                     const FieldInfo *fieldInfo) const = 0;

        virtual void visitClassMethod(const ClassFile *classFile,
                                      const MethodInfo *methodInfo) const = 0;

        virtual void visitClassAttribute(const ClassFile *classFile,
                                         const AttributeInfo *attributeInfo) const = 0;

        virtual void visitFieldAttribute(const ClassFile *classFile,
                                         const FieldInfo *fieldInfo,
                                         const AttributeInfo *attributeInfo) const = 0;

        virtual void visitMethodAttribute(const ClassFile *classFile,
                                          const MethodInfo *methodInfo,
                                          const AttributeInfo *attributeInfo) const = 0;

    };

}