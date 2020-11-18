//
// Created by timo on 15.11.20.
//

#pragma once

#include "visitor.h"

#include "../structure/constantinfo.h"

namespace ares {

    class VMCheck : Visitor {

    public:
        void visitClass(ares::ClassFile &classFile) override;

        void visitClassCPInfo(ares::ClassFile &classFile,
                              ares::ConstantPoolInfo &constantPoolInfo) override;

        void visitClassInterface(ares::ClassFile &classFile,
                                 uint16_t interface) override;

        void visitClassField(ares::ClassFile &classFile,
                             ares::FieldInfo &fieldInfo) override;

        void visitClassMethod(ares::ClassFile &classFile,
                              ares::MethodInfo &methodInfo) override;

        void visitClassAttribute(ares::ClassFile &classFile,
                                 ares::AttributeInfo &attributeInfo) override;

        void visitFieldAttribute(ares::ClassFile &classFile,
                                 ares::FieldInfo &fieldInfo,
                                 ares::AttributeInfo &attributeInfo) override;

        void visitMethodAttribute(ares::ClassFile &classFile,
                                  ares::MethodInfo &methodInfo,
                                  ares::AttributeInfo &attributeInfo) override;

        static void visitClassInfo(ares::ClassFile &classFile,
                                   ares::ConstantInfo::ClassInfo &info);

        static void visitFieldMethodInfo(ares::ClassFile &classFile,
                                         ares::ConstantInfo::FieldMethodInfo &info);

        static void visitNameAndTypeInfo(ares::ClassFile &classFile,
                                         ares::ConstantInfo::NameAndTypeInfo &info);

        static void visitStringInfo(ares::ClassFile &classFile,
                                    ares::ConstantInfo::StringInfo &info);

        static void visitMethodTypeInfo(ares::ClassFile &classFile,
                                        ares::ConstantInfo::MethodTypeInfo &info);

        static void visitMethodHandleInfo(ares::ClassFile &classFile,
                                          ares::ConstantInfo::MethodHandleInfo &info);

        static void visitDynamicInfo(ares::ClassFile &classFile,
                                     ares::ConstantInfo::DynamicInfo &info);

        static void visitModulePackageInfo(ares::ClassFile &classFile,
                                           ares::ConstantInfo::ModulePackageInfo &info);

    };

}