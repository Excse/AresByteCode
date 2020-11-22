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
        virtual void visitClass(ClassInfo &classInfo) = 0;

        virtual void visitClassCPInfo(ClassInfo &classInfo,
                                      ConstantPoolInfo &constantPoolInfo) = 0;

        virtual void visitClassInterface(ClassInfo &classInfo,
                                         uint16_t interface) = 0;

        virtual void visitClassField(ClassInfo &classInfo,
                                     FieldInfo &fieldInfo) = 0;

        virtual void visitClassMethod(ClassInfo &classInfo,
                                      MethodInfo &methodInfo) = 0;

        virtual void visitClassAttribute(ClassInfo &classInfo,
                                         AttributeInfo &attributeInfo) = 0;

        virtual void visitFieldAttribute(ClassInfo &classInfo,
                                         FieldInfo &fieldInfo,
                                         AttributeInfo &attributeInfo) = 0;

        virtual void visitMethodAttribute(ClassInfo &classInfo,
                                          MethodInfo &methodInfo,
                                          AttributeInfo &attributeInfo) = 0;

    };

}