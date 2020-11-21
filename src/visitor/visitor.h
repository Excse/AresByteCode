//
// Created by timo on 15.11.20.
//

#pragma once

#include "../structure/attributeinfo.h"
#include "../structure/methodinfo.h"
#include "../structure/fieldinfo.h"
#include "../structure/classinfo.h"

namespace ares {

    class Visitor {

    public:
        virtual void visitClass(ClassInfo &classFile) = 0;

        virtual void visitClassCPInfo(ClassInfo &classFile,
                                      ConstantPoolInfo &constantPoolInfo) = 0;

        virtual void visitClassInterface(ClassInfo &classFile,
                                         uint16_t interface) = 0;

        virtual void visitClassField(ClassInfo &classFile,
                                     FieldInfo &fieldInfo) = 0;

        virtual void visitClassMethod(ClassInfo &classFile,
                                      MethodInfo &methodInfo) = 0;

        virtual void visitClassAttribute(ClassInfo &classFile,
                                         AttributeInfo &attributeInfo) = 0;

        virtual void visitFieldAttribute(ClassInfo &classFile,
                                         FieldInfo &fieldInfo,
                                         AttributeInfo &attributeInfo) = 0;

        virtual void visitMethodAttribute(ClassInfo &classFile,
                                          MethodInfo &methodInfo,
                                          AttributeInfo &attributeInfo) = 0;

    };

}