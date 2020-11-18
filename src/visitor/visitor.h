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
        virtual void visitClass(ClassFile &classFile) = 0;

        virtual void visitClassCPInfo(ClassFile &classFile,
                                      ConstantPoolInfo &constantPoolInfo) = 0;

        virtual void visitClassInterface(ClassFile &classFile,
                                         uint16_t interface) = 0;

        virtual void visitClassField(ClassFile &classFile,
                                     FieldInfo &fieldInfo) = 0;

        virtual void visitClassMethod(ClassFile &classFile,
                                      MethodInfo &methodInfo) = 0;

        virtual void visitClassAttribute(ClassFile &classFile,
                                         AttributeInfo &attributeInfo) = 0;

        virtual void visitFieldAttribute(ClassFile &classFile,
                                         FieldInfo &fieldInfo,
                                         AttributeInfo &attributeInfo) = 0;

        virtual void visitMethodAttribute(ClassFile &classFile,
                                          MethodInfo &methodInfo,
                                          AttributeInfo &attributeInfo) = 0;

    };

}