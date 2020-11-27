//
// Created by timo on 07.11.20.
//

#pragma once

#include <unordered_map>
#include <sys/stat.h>
#include <unistd.h>
#include <memory>
#include <vector>
#include <list>

#include "attributeinfo.h"
#include "constantinfo.h"
#include "classinfo.h"
#include "visitor.h"

namespace ares {

    class ClassReader : Visitor {

    private:
        unsigned int m_Offset{};

    public:
        explicit ClassReader(unsigned int offset = 0u);

        virtual ~ClassReader();

    public:
        void visitClass(ares::ClassInfo &classInfo) override;

    private:
        void readClassAttributes(ClassInfo &classInfo);

        void visitClassAttribute(ares::ClassInfo &classInfo,
                                 ares::AttributeInfo &attributeInfo) override;

        void readMagicNumber(ares::ClassInfo &classInfo);

        void readClassVersion(ares::ClassInfo &classInfo);

        void readConstantPool(ares::ClassInfo &classInfo);

        void visitClassCPInfo(ares::ClassInfo &classInfo,
                              ares::ConstantPoolInfo &info) override;

        void readClassInfo(ares::ClassInfo &classInfo,
                           ConstantInfo::ClassInfo &info);

        void readUTF8Info(ares::ClassInfo &classInfo,
                          ConstantInfo::UTF8Info &info);

        void readFieldMethodInfo(ares::ClassInfo &classInfo,
                                 ConstantInfo::FieldMethodInfo &info);

        void readNameAndType(ares::ClassInfo &classInfo,
                             ConstantInfo::NameAndTypeInfo &info);

        void readStringInfo(ares::ClassInfo &classInfo,
                            ConstantInfo::StringInfo &info);

        void readDoubleLong(ares::ClassInfo &classInfo,
                            ConstantInfo::DoubleLongInfo &info);

        void readFloatInteger(ares::ClassInfo &classInfo,
                              ConstantInfo::FloatIntegerInfo &info);

        void readMethodType(ares::ClassInfo &classInfo,
                            ConstantInfo::MethodTypeInfo &info);

        void readMethodHandle(ares::ClassInfo &classInfo,
                              ConstantInfo::MethodHandleInfo &info);

        void readDynamic(ares::ClassInfo &classInfo,
                         ConstantInfo::DynamicInfo &info);

        void readModulePackage(ares::ClassInfo &classInfo,
                               ConstantInfo::ModulePackageInfo &info);

        void readAccessFlags(ClassInfo &classInfo);

        void readThisClass(ClassInfo &classInfo);

        void readSuperClass(ClassInfo &classInfo);

        void readInterfaces(ClassInfo &classInfo);

        void visitClassInterface(ClassInfo &classInfo,
                                 uint16_t interface) override;

        void readFields(ClassInfo &classInfo);

        void visitClassField(ares::ClassInfo &classInfo,
                             ares::FieldInfo &fieldInfo) override;

        void readFieldAttributes(ClassInfo &classInfo, FieldInfo &fieldInfo);

        void visitFieldAttribute(ares::ClassInfo &classInfo,
                                 ares::FieldInfo &fieldInfo,
                                 ares::AttributeInfo &attributeInfo) override;

        void readMethods(ClassInfo &classInfo);

        void visitClassMethod(ares::ClassInfo &classInfo,
                              ares::MethodInfo &methodInfo) override;

        void readMethodAttributes(ClassInfo &classInfo, MethodInfo &methodInfo);

        void visitMethodAttribute(ares::ClassInfo &classInfo,
                                  ares::MethodInfo &methodInfo,
                                  ares::AttributeInfo &attributeInfo) override;

    public:
        [[nodiscard]]
        unsigned int getOffset() const;

    };

}