//
// Created by timo on 15.11.20.
//

#pragma once

#include "attributeinfo.h"
#include "constantinfo.h"
#include "methodinfo.h"
#include "classinfo.h"
#include "fieldinfo.h"
#include "visitor.h"

namespace ares {

    class VMCheck : Visitor {

    public:
        void visitClass(ares::ClassInfo &classInfo) override;

        void visitClassCPInfo(ares::ClassInfo &classInfo,
                              ares::ConstantPoolInfo &constantPoolInfo) override;

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

        static void visitClassInfo(ares::ClassInfo &classInfo,
                                   ares::ConstantInfo::ClassInfo &info);

        static void visitFieldMethodInfo(ares::ClassInfo &classInfo,
                                         ares::ConstantInfo::FieldMethodInfo &info);

        static void visitNameAndTypeInfo(ares::ClassInfo &classInfo,
                                         ares::ConstantInfo::NameAndTypeInfo &info);

        static void visitStringInfo(ares::ClassInfo &classInfo,
                                    ares::ConstantInfo::StringInfo &info);

        static void visitMethodTypeInfo(ares::ClassInfo &classInfo,
                                        ares::ConstantInfo::MethodTypeInfo &info);

        static void visitMethodHandleInfo(ares::ClassInfo &classInfo,
                                          ares::ConstantInfo::MethodHandleInfo &info);

        static void visitDynamicInfo(ares::ClassInfo &classInfo,
                                     ares::ConstantInfo::DynamicInfo &info);

        static void visitModulePackageInfo(ares::ClassInfo &classInfo,
                                           ares::ConstantInfo::ModulePackageInfo &info);

    };

}