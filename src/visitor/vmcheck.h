//
// Created by timo on 15.11.20.
//

#pragma once

#include "visitor.h"

#include "../structure/constantinfo.h"

namespace ares {

    class VMCheck : Visitor {

    public:
        void visitClass(ares::ClassInfo &classFile) override;

        void visitClassCPInfo(ares::ClassInfo &classFile,
                              ares::ConstantPoolInfo &constantPoolInfo) override;

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

        static void visitClassInfo(ares::ClassInfo &classFile,
                                   ares::ConstantInfo::ClassInfo &info);

        static void visitFieldMethodInfo(ares::ClassInfo &classFile,
                                         ares::ConstantInfo::FieldMethodInfo &info);

        static void visitNameAndTypeInfo(ares::ClassInfo &classFile,
                                         ares::ConstantInfo::NameAndTypeInfo &info);

        static void visitStringInfo(ares::ClassInfo &classFile,
                                    ares::ConstantInfo::StringInfo &info);

        static void visitMethodTypeInfo(ares::ClassInfo &classFile,
                                        ares::ConstantInfo::MethodTypeInfo &info);

        static void visitMethodHandleInfo(ares::ClassInfo &classFile,
                                          ares::ConstantInfo::MethodHandleInfo &info);

        static void visitDynamicInfo(ares::ClassInfo &classFile,
                                     ares::ConstantInfo::DynamicInfo &info);

        static void visitModulePackageInfo(ares::ClassInfo &classFile,
                                           ares::ConstantInfo::ModulePackageInfo &info);

    };

}