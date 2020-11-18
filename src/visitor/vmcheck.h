//
// Created by timo on 15.11.20.
//

#pragma once

#include "visitor.h"

#include "../structure/constantinfo.h"

namespace ares {

    class VMCheck : Visitor {

    public:
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

        static void visitClassInfo(const ClassFile *classFile,
                                   const ConstantInfo::ClassInfo *info);

        static void visitFieldMethodInfo(const ClassFile *classFile,
                                         const ConstantInfo::FieldMethodInfo *info);

        static void visitNameAndTypeInfo(const ClassFile *classFile,
                                         const ConstantInfo::NameAndTypeInfo *info);

        static void visitStringInfo(const ClassFile *classFile,
                                    const ConstantInfo::StringInfo *info);

        static void visitMethodTypeInfo(const ClassFile *classFile,
                                        const ConstantInfo::MethodTypeInfo *info);

        static void visitMethodHandleInfo(const ClassFile *classFile,
                                          const ConstantInfo::MethodHandleInfo *info);

        static void visitDynamicInfo(const ClassFile *classFile,
                                     const ConstantInfo::DynamicInfo *info);

        static void visitModulePackageInfo(const ClassFile *classFile,
                                           const ConstantInfo::ModulePackageInfo *info);

    };

}